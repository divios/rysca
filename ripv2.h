//
// Created by serbe on 01/12/2020.
//

#ifndef RYSCA_RIPV2_H
#define RYSCA_RIPV2_H

#include "udp.h"

typedef struct entrada_rip{

    ipv4_addr_t subnet;
    ipv4_addr_t mask;
    ipv4_addr_t gw;
    char iface[IFACE_NAME_MAX_LENGTH];
    int metric;

} entrada_rip_t;

typedef struct ripv2_msg{
        // son definiciones todos estos campos, rellenar con su tipo correspondiente
        int tipo;
        int version;
        int dominio_encaminamiento;
        entrada_rip_t vectores_distancia[25];
} ripv2_msg_t;

#endif //RYSCA_RIPV2_H
