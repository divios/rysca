
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>

#include "ripv2_route_table.h"
#include "arp.h"

int main(int argc, char *argv[]) {


    if ((argc <= 3) || (argc > 4)) {
        printf("       <string.txt>: Nombre del archivo config.txt\n");
        printf("       <string.txt>: Nombre del archivo route_table.txt\n");
        printf("       <ipv4>: ip del servidor a enviar request\n");
        exit(-1);
    }

    char *config_name = argv[1];
    char *route_table_name = argv[2];
    char *ip_str = argv[3];

    //comprobamos que la IP es valida
    ipv4_addr_t ip_addr;
    if (ipv4_str_addr(ip_str, ip_addr) != 0) {
        printf("Ip no valida\n");
        exit(-1);
    }

    udp_layer_t *udp_layer = udp_open(RIP_PORT, config_name, route_table_name);
    if (udp_layer == NULL) {
        printf("Fallo al abrir la interfaz");
        exit(-1);
    }

    ripv2_msg_t msg;
    msg.type = RIPv2_REQUEST;
    msg.version = RIPv2_TYPE_VERSION;
    msg.routing_domain = UNUSED;

    entrada_rip_t request_all;
    request_all.family_directions = 0;
    request_all.metric = -1;

    msg.entrada[0] = request_all;

    udp_send(udp_layer, ip_addr, RIP_PORT, (unsigned char *) msg, sizeof(entrada_rip_t) + RIP_HEADER_SIZE);

    uint16_t *port;
    ripv2_msg_t msg_recv;

    int bytes = udp_recv(udp_layer, -1, ip_addr, port, (unsigned char *) &msg_recv, sizeof(msg_recv));

    if ( (*port) != RIP_PORT && msg_recv.type != RIPv2_REPLY) printf("baya\n"); exit(-1);

    for (int i= 0; i < (bytes - RIP_HEADER_SIZE) / sizeof(ripv2_msg_t); i++) {
        entrada_rip_t entry = msg_recv.entrada[i];
        printf("subnet: %s, mask: %s, gw: %s, metric: %s\n", entry.subnet, entry.mask, entry.gw, entry.metric)
    }


}