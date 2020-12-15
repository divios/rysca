
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

    ripv2_route_table_print(table);

    while(1) {

        printf("%li  ", timerms_left(& (timers->list_timers[0]) ));
        printf("%li\n", timerms_left(& (timers->list_timers[1]) ));
        sleep(1);
    }

    /*while (1) {

        long int min_time, n;
        ripv2_msg_t payload;
        uint16_t *port_out;
        ipv4_addr_t sender, myIp;

        min_time = ripv2_timeleft(timers); // Si tabla esta vacia, -1, esperamos infinito
        port_out = malloc(sizeof(uint16_t));

        n = udp_recv(udp_layer, min_time, sender, port_out, (unsigned char *) &payload, sizeof(payload));


        if (n > 0 && (*port_out == RIP_PORT) && memcmp(sender)  ) {
            //TODO: procesar el mensaje segun si es request o response
            ripv2_msg_t *ripv2_msg = (ripv2_msg_t *) payload;

            if (ripv2_msg->type == 1) { // request
                ripv2_msg_t *rip_request = malloc(sizeof(ripv2_msg_t));
                if ( ripv2_route_table_request_all_table(ripv2_msg->table) == 1 ) { // Toda la tabla

                    rip_request->table = table;

                }
                else{
                    rip_route_table_t *received_table = ripv2_msg->table;

                    for (int i = 0; i>RIP_ROUTE_TABLE_SIZE; i++) {
                        if (received_table->routes[i] != NULL) {

                            int index = ripv2_route_table_find(table, received_table->routes[i]);

                            ripv2_route_table_add(rip_request->table, table->routes[i]);

                            // Si no esta en la tabla, mandar lo mismo pero metrica a 16


                            /*if (index == -1) {
                                ripv2_route_table_add(table, request_table->routes[i]);
                            }
                            else{
                                if( request_table->routes[i]->metric < table->routes[i]->metric ) {
                                    memcpy(table->routes[i]->gw, sender, sizeof(ipv4_addr_t));
                                    table->routes[i]->metric = (request_table->routes[i]->metric + 1);
                                }
                            }*/ /*


                        }
                    }

                    rip_request->table = requested_table;


                }
                // Enviamos el mensaje
                rip_request->type = 2;
                rip_request->version = RIPv2_TYPE_VERSION;
                rip_request->routing_domain = UNUSED;
                udp_send(udp_layer, sender, UDP_PROTOCOL, port_out, (unsigned char *) rip_request, sizeof(rip_request));
                free(rip_request);

            } else if ( ripv2_msg->type == 2 ) {


            }

            // Devuelve las entradas con timers expirados
            ripv2_route_table_remove_expired(table, timers);

            ripv2_route_table_write(table, route_table_name); // Actualizamos el fichero

        } */



    }


