//
// Created by serbe on 13/10/2020.
//

#ifndef RYSCA_UDP_H
#define RYSCA_UDP_H

#include "ipv4.h"

//Variables para paquete UDP
#define UDP_PROTOCOL 0x17
#define UDP_HEADER_LEN 1478


typedef struct udp_layer {
    uint16_t source_port;
    uint16_t destination_port;
    /*uint16_t checksum: CHECKSUM_TO_ZERO;
    int payload_len;
    char * payload;*/
    ipv4_layer_t *ipv4_layer;

} udp_layer_t;


typedef struct udp_packet {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t checksum;
    unsigned char payload[MRU];
} udp_packet_t;


udp_layer_t *upd_open(char *file_config, char *ip_config, char *route_config);


int udp_send(udp_layer_t *layer, ipv4_addr_t dst, uint16_t protocol, unsigned char data[], int payload_len);


#endif //RYSCA_UDP_H


