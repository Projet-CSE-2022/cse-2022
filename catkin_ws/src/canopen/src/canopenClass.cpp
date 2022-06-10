#include "canopenClass.h"


CanopenClass::CanopenClass(){}


int CanopenClass::init_socket(std::string nameCan){
  struct ifreq ifr;
  struct sockaddr_can addr;

  //Création du socket CAN
  if ((sock_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) {
    perror("Error while opening socket");
    return -1;
  }

  //Récupération de l'index de l'interface can.
  strcpy(ifr.ifr_name, nameCan.c_str());
  ioctl(sock_fd, SIOCGIFINDEX, &ifr);

  //Assignation de l'adresse avec la foncton bind()
  addr.can_family  = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

//   printf("%s at index %d\n", nameCan.c_str(), ifr.ifr_ifindex);

  if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Error in socket bind");
    return -2;
  }

  printf("return 0\n");
  return 0;
}



int CanopenClass::writeFunc(int cobId, long cmd, int nbrByte){
    struct can_frame frame;

    prepare_frame(&frame, nbrByte, 0, cobId, cmd);
    ROS_INFO("SEND");
    return send_can_frame(frame);
}



int CanopenClass::readFunc(struct can_frame* frame){
    int er = recv_can_frame(frame);
    if(er < 0){
        return er;
    }
    // parse_can_frame(frame);
    ROS_INFO("RECEIVE");
    return er;
}



int CanopenClass::recv_can_frame(struct can_frame* frame){
    int nbytes = read(sock_fd, frame, sizeof(struct can_frame));

    if(nbytes < 0){
        ROS_INFO("CAN raw socket read");
        return -1;
    }

    if(nbytes < (int)sizeof(struct can_frame)){
        ROS_INFO("Read : incomplete CAN frame");
        return -1;
    }

    return nbytes;
}



int CanopenClass::send_can_frame(struct can_frame frame){
    // send the frame to the CAN bus
    int nbytes = write(sock_fd, &frame, sizeof(frame));

    if (nbytes < 0)
    {
        ROS_INFO("CAN raw socket write failed");
        return -1;
    }

    if (nbytes < (int)sizeof(struct can_frame))
    {
        ROS_INFO("Write : incomplete CAN frame");
        return -1;
    }

    return nbytes;
}



void CanopenClass::prepare_frame(struct can_frame* frame, int nb_bytes, int node_id, int cmd, long int data){
  frame->can_id  = cmd + node_id;
  frame->can_dlc = nb_bytes;

  for (int i = 0; i < nb_bytes; ++i){
    frame->data[i] = (uint8_t)(data >> (nb_bytes - 1 - i)*8);
  }
}



int CanopenClass::parse_can_frame(struct can_frame* can_frame){

    int tmp = 0;

    // if (can_frame->can_id & CAN_EFF_FLAG) {
    //     // printf("Type : 0x%x\n", CANOPEN_FLAG_EXTENDED);
    //     printf("Id   : 0x%x\n", can_frame->can_id & CAN_EFF_MASK);
    // }
    // else {
    //     // printf("Type          : 0x%x\n", CANOPEN_FLAG_STANDARD);
    //     printf("Function code : 0x%x\n", (can_frame->can_id & 0x00000780U) >> 7);
    //     printf("Id            : 0x%x\n", (can_frame->can_id & 0x0000007FU));
    // }

    // // printf("RTR     : 0x%x\n", (can_frame->can_id & CAN_RTR_FLAG) ? CANOPEN_FLAG_RTR : CANOPEN_FLAG_NORMAL);

    // printf("Data lenght : %d\n", can_frame->can_dlc);

    for (int i = 0; i < can_frame->can_dlc; i++) {
        printf("Data %d : 0x%x\n", i, can_frame->data[i]);
        tmp |= can_frame->data[i] << 8*(can_frame->can_dlc-1-i);
    }

    return tmp;
}