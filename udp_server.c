#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>

#include "ipv4.h"
#include "udp.h"


int main(int argc, char *argv[]) {
    //Necesitamos como parametros el nombre de la interfaz
    //el nombre del archivo de text de la config y routas
    //y la ip al que se le debe enviar el mensaje

    if ((argc <= 2) || (argc > 3)) {
        printf("       <string.txt>: Nombre del archivo config.txt\n");
        printf("       <string.txt>: Nombre del archivo route_table.txt\n");
        exit(-1);
    }

    //procesamos los argumentos

    char *config_name = argv[1];
    char *route_table_name = argv[2];


    //Abrimos la interfaz y comprobamos que se leyo el archivo;
    udp_layer_t *udp_layer = upd_open(327, 1313, config_name, route_table_name);

    if (udp_layer == NULL) {
        printf("No se pudo leer correctamente el fichero config.txt\n");
        exit(-1);
    }

    unsigned char buffer[UDP_PACKET_LEN];
    ipv4_addr_t src_addr;

    while (1) {

        /* Recibir trama Ethernet del Cliente */

        long int timeout = -1;

        printf("Escuchando a tramas udp\n");
        int payload_len = udp_recv(udp_layer, timeout, UDP_PROTOCOL, buffer, UDP_PACKET_LEN);

        if (payload_len == -1) {
            printf("Error al recibir la trama\n");
            exit(-1);
        }

        printf("Recibido el paquete\n");
        break;

    }

}
