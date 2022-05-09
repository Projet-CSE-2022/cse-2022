#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file
#include "CANMESSAGES.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include <SPI.h>
#include "mcp_can.h"
#include <math.h>

#define g 9.81
#define MPU6050_INTERRUPT_PIN 2  // Interrupt pin for CAN module
#define MCP2515_INTERRUPT_PIN 3 // Interrupt pin for MCP2515 module


MCP_CAN CAN(10);
volatile unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

// Wheelchair variables
const float a = 0.2; // Length of wheelchair
const float b = 0.1; // Width of wheelchair

const float CG_x = 0.0;
const float CG_y = 0.1;
const float CG_z = 0.1;

const float alphaCritPlus = PI / 2 - atan(CG_z / CG_y);
const float alphaCritMinus = - PI / 2 + atan(CG_z / (a - CG_y));
const float betaCritPlus = - PI / 2 + atan(CG_z / (b / 2 + CG_x));
const float betaCritMinus = PI / 2 - atan(CG_z / (b / 2 - CG_x));

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void MCP2515_ISR()
{
    Flag_Recv = 1;
    Serial.println("MCP2515_ISR");
}


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    Serial.begin(115200);

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(MPU6050_INTERRUPT_PIN, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    bool exit = false;
    while(!exit)                   // check if get data
    {
        if(Flag_Recv) {
            Flag_Recv = 0;                // clear flag
            CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

            if(strcmp((char *)buf, IMUON) == 0) {
                exit = true;
            }
            Serial.print("Received: ");
            Serial.println((char *)buf);
        }
    }

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(MPU6050_INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(MPU6050_INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
    
    attachInterrupt(digitalPinToInterrupt(MCP2515_INTERRUPT_PIN), MCP2515_ISR, FALLING); // start interrupt
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // if(Serial.available() > 0)
    // {
    //     Serial.read();
    //     strcpy((char*) buf, "hello");
    //     CAN.sendMsgBuf(0x00, 2, strlen((const char *)buf), buf);
    //     Serial.println("send data");
    // }

    if (mpuInterrupt) {
    // read a packet from FIFO
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet 
            
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

            Serial.print("ypr [°]\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[2] * 180/M_PI);

            // 1g = +8192 -> m/s^2 = /8192 * 9.81

            float acc_x = aaReal.x / 8192.0 * g;
            float acc_y = aaReal.y / 8192.0 * g;
            float acc_z = aaReal.z / 8192.0 * g;

            Serial.print("\tareal  [m/s^2]\t");
            Serial.print(acc_x);
            Serial.print("\t");
            Serial.print(acc_y);
            Serial.print("\t");
            Serial.print(acc_z);

            float alpha = ypr[1];
            float beta = ypr[2];
            
            float accCrit_y = (aaReal.y > 0 ? CG_y : (a - CG_y)) / CG_z * g * cos(alpha) - g * sin(alpha);
            float accCrit_x = (aaReal.x > 0 ? (b / 2 - CG_x) : (b / 2 + CG_x)) / CG_z * g * cos(beta) - g * sin(beta);

            Serial.print("\taccCrit_y [m/s^2]\t");
            Serial.print(accCrit_x);
            Serial.print("\t");
            Serial.print(accCrit_y);
            Serial.print("\r");

            if(abs(accCrit_y - acc_y) < 0.5) {
                strcpy((char*) buf, STOPY);
                CAN.sendMsgBuf(0x00, 2, strlen((const char *)buf), buf);
                Serial.print("send");
                Serial.println(STOPY);
                Serial.flush();
            }
            else if(accCrit_x - acc_x < 0.5) {
                strcpy((char*) buf, STOPX);
                CAN.sendMsgBuf(0x00, 2, strlen((const char *)buf), buf);
                Serial.print("send");
                Serial.println(STOPX);
                Serial.flush();
            }
        }
        mpuInterrupt = false;
    }
}