
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>

#include "ripv2_route_table.h"

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

    rip_route_table_t *table = NULL;
    timers_t *timers = NULL;

    int last_index = ripv2_route_table_read(rip_route_table_name, table);
    ripv2_inicialize_timers(last_index, timers);

    udp_layer_t *udp_layer = udp_open(port_in, config_name, route_table_name);
    if (udp_layer == NULL) {
        printf("Fallo al abrir la interfaz");
        exit(-1);
    }

    while (1) {

        long int min_time, n;
        ripv2_msg_t payload;
        uint16_t *port_out;
        ipv4_addr_t sender;

        min_time = ripv2_timeleft(timers); /* Si tabla esta vacia, -1, esperamos infinito */
        port_out = malloc(sizeof(uint16_t));

        n = udp_recv(udp_layer, min_time, UDP_PROTOCOL, sender, port_out, (unsigned char *) &payload, sizeof(payload));

        if (n > 0 && (*port_out == RIP_PORT) ) {
            //TODO: procesar el mensaje segun si es request o response
        }

        /* Devuelve las entradas con timers expirados */
        ripv2_route_table_remove_expired(table, timers);

        ripv2_route_table_write(table, route_table_name); /* Actualizamos el fichero */

    }


}