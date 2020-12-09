
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>

#include "udp.h"
#include "rip_route_table.h"
#include "ripv2.h"
#include "ipv4.h"


//INCIALIZAR TABLAS RIPV2

int main(int argc, char *argv[]) {


    if ((argc <= 6) || (argc > 7)) {
        printf("       <string.txt>: Nombre del archivo config.txt\n");
        printf("       <string.txt>: Nombre del archivo route_table.txt\n");
        printf("       <string.txt>: Nombre del archivo rip_route_table.txt\n")
        printf("        <puerto_in>: nuestro puerto\n");
        exit(-1);
    }

    char *config_name = argv[1];
    char *route_table_name = argv[2];
    char *rip_route_table_name = argv[3];
    uint16_t port_in = atoi(argv[4]);

    rip_route_table_t rip_table;



    udp_layer_t *udp_layer = udp_open(port_in, config_name, route_table_name);
    if (udp_layer == NULL) {
        printf("Fallo al abrir la interfaz");
        exit(-1);
    }


    while (1) {
        long int min_time = 0;
        //timeleft

        ripv2_msg_t payload;

        int n = udp_recv(udp_layer, min_time, UDP_PROTOCOL, udp_layer->ipv4_layer->addr, port_int, &payload, sizeof(payload));

        if (n > 0) {
            //procesar el mensaje segun si es request o response
        }

        //recorremos tabla en busca de timers a 0
        for (i=0, i< RIP_ROUTE_TABLE_SIZE, i++) {

            entrada_rip_t entrada = rip_tablet->routes[i];

            if (entrada != NULL) {

            }

        }

    }


}