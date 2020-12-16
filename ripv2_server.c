#include <arpa/inet.h>
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
        printf("       <string.txt>: Nombre del archivo rip_route_table.txt\n");
        exit(-1);
    }

    char *config_name = argv[1];
    char *route_table_name = argv[2];
    char *rip_route_table_name = argv[3];

    rip_route_table_t *table = ripv2_route_table_create();
    timers_t *timers = malloc(sizeof(timers_t));

    int last_index = ripv2_route_table_read(rip_route_table_name, table);

    ripv2_inicialize_timers(last_index, timers);

    udp_layer_t *udp_layer = udp_open(RIP_PORT, config_name, route_table_name);
    if (udp_layer == NULL) {
        printf("Fallo al abrir la interfaz");
        exit(-1);
    }

    while (1) {

        ipv4_addr_t sender_ip;
        uint16_t port;
        ripv2_msg_t buffer;
        int min_time, bytes, n_entries, index;

        min_time = ripv2_timeleft(table, *timers);

        bytes = udp_recv(udp_layer, min_time, sender_ip, &port, (unsigned char *) &buffer, sizeof(buffer));

        n_entries = (bytes - RIP_HEADER_SIZE) / sizeof(entrada_rip_t);

        if (port != RIP_PORT || n_entries == 0) continue; //Añadir que sea de nuestra propia subred

        if (buffer.type == RIPv2_REQUEST) {

            ripv2_msg_t msg;
            msg.type = 1;
            msg.version = RIPv2_TYPE_VERSION;
            msg.routing_domain = UNUSED;

            if (n_entries == 1 && buffer.entrada[0].family_directions == 0 &&
                ripv2_is_infinite(ntohl(buffer.entrada[0].metric))) {

                index = 0;
                for (int i = 0; i < RIP_ROUTE_TABLE_SIZE; i++) {
                    entrada_rip_t *entry = table->routes[i];
                    if (entry != NULL) {
                        entry->family_directions = htons(entry->family_directions);
                        entry->metric = htonl(entry->metric);
                        msg.entrada[index] = *(entry);
                        index++;
                    }
                }

                udp_send(udp_layer, sender_ip, RIP_PORT, (unsigned char *) &msg, sizeof(entrada_rip_t) * index + RIP_HEADER_SIZE);

            }


        }


    }

}


