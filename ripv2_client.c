
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>

#include "rip_route_table.h"
#include "ripv2.h"

int main(int argc, char *argv[]) {


    if ((argc <= 6) || (argc > 7)) {
        printf("       <string.txt>: Nombre del archivo config.txt\n");
        printf("       <string.txt>: Nombre del archivo route_table.txt\n");
        printf("       <string.txt>: Nombre del archivo rip_route_table.txt\n");
        printf("        <puerto_in>: nuestro puerto\n");
        exit(-1);
    }

    char *config_name = argv[1];
    char *route_table_name = argv[2];
    char *rip_route_table_name = argv[3];
    uint16_t port_in = atoi(argv[4]); /* Lo suyo es harcodearlo a RIP_PORT 55 */

    rip_route_table_t *rip_table = NULL;
    ripv2_route_table_read(rip_route_table_name, rip_table);

    udp_layer_t *udp_layer = udp_open(port_in, config_name, route_table_name);
    if (udp_layer == NULL) {
        printf("Fallo al abrir la interfaz");
        exit(-1);
    }

    while (1) {
        long int min_time = 0;//TODO: timeleft
        ripv2_msg_t *payload = malloc(sizeof(ripv2_msg_t));
        uint16_t *port_out = malloc(sizeof(uint16_t));

        int n = udp_recv(udp_layer, min_time, UDP_PROTOCOL, udp_layer->ipv4_layer->iface, port_out, (unsigned char *) payload, sizeof(payload));

        if (n > 0  && (*port_out == RIP_PORT) ) {
            //TODO: procesar el mensaje segun si es request o response
        }

        /*Devuelve las entradas con timers expirados */
        rip_route_table_t *expired = ripv2_route_table_get_expired(rip_table);

        /* TODO: Eliminamos las entradas expiradas (meter en una funcion? */
        for(int i = 0; i<RIP_ROUTE_TABLE_SIZE; i++) {
            entrada_rip_t *entry = expired->routes[i];
            if (entry == NULL) break; /*En esta tabla a la que sea null es que no hay mas */
            int index = ripv2_route_table_find(rip_table, entry);
            ripv2_route_table_remove(rip_table, index);

        }
        ripv2_route_table_free(expired);

    }


}