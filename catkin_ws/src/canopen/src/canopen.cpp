#include "canopenClass.h"
#include "ros/ros.h"
#include <thread>

#include <std_srvs/Empty.h>

#define ROS_RATE 100
#define NAME_CAN "can0"

#define IMUON 0x494d5531
#define STOP  0x53544f50


int main(int argc, char **argv) {

    ros::init(argc, argv, "crh_canopen");
    ros::NodeHandle n("~");
    ros::Rate rate(ROS_RATE);

    ros::ServiceClient cl_stop = n.serviceClient<std_srvs::Empty>("/stop_servos");
    std_srvs::Empty emptyMsg;

    CanopenClass canopen1;
    struct can_frame frameRead;

    if(canopen1.init_socket((std::string)NAME_CAN)){
        ROS_ERROR("Init socket");
        return -1;
    }

    // ros::Subscriber subTracker3  = n.subscribe("/vivetracker/tracker_3/pose", 1, &Calcul_traj::checkTracker3Func, &calcul_traj);

    // std::thread readerThread(CanopenClass::readFunc, &frameTest);

    printf("IMU1 %x\n", IMUON);

    printf("before write\n");
    while(ros::ok()){ 
        if(canopen1.writeFunc(580, IMUON, 4) < 0){
            perror("Error writeFunc canopen");
            return -1;
        }

        if(canopen1.readFunc(&frameRead) < 0){
            perror("Error readFunc canopen");
            return -1;
        }

        if(IMUON == canopen1.parse_can_frame(&frameRead)){
            printf("IMUON\n");
            break;
        }
        printf("IMUOFF");
    }
    printf("after write\n");


    while(ros::ok()){
        canopen1.readFunc(&frameRead);
        if(STOP == canopen1.parse_can_frame(&frameRead)){
            ROS_INFO("STOP before !!!!!!!!!!!!!!");
            cl_stop.call(emptyMsg);
            ROS_INFO("STOP after !!!!!!!!!!!!!!");
        }
        rate.sleep();
    }

    return 0;
}

