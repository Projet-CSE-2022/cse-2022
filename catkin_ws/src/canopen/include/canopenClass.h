#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>
#include "ros/ros.h"
#include <string>


#include <stdlib.h>
#include <unistd.h>

#define IFNAME "can0"

class CanopenClass{
    private:
        int sock_fd;

        int recv_can_frame(struct can_frame* frame);
        int send_can_frame(struct can_frame frame);

        
        void prepare_frame(struct can_frame* frame, int nb_bytes, int node_id, int cmd, long int data);

    public:

        CanopenClass();
        int init_socket(std::string nameCan);
        int writeFunc(int cobId, long cmd, int nbrByte);
        int readFunc(struct can_frame* frame);
        int parse_can_frame(struct can_frame* frame);
};