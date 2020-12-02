//
// Created by serbe on 01/12/2020.
//

#ifndef RYSCA_RIPV2_H
#define RYSCA_RIPV2_H

#include "udp.h"

#define UNUSED 0x0000
#define DEAFULT_FAMILY_DIRECTION 2
#define RIPv2_TYPE_VERSION 2


typedef struct entrada_rip{

    uint16_t family_directions;
    uint16_t route_label; //unused
    ipv4_addr_t subnet;
    ipv4_addr_t mask;
    ipv4_addr_t gw;
    int metric;

} entrada_rip_t;

typedef struct ripv2_msg{

        uint8_t type;
        uint8_t version;
        uint16_t routing_domain; //unused
        entrada_rip_t distance_vectors[25];

} ripv2_msg_t;

#endif //RYSCA_RIPV2_H
