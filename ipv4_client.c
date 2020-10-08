#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>

#include "ipv4.h"


int main(int argc, char *argv[]) {
    //Necesitamos como parametros el nombre de la interfaz
    //el nombre del archivo de text de la config y routas
    //y la ip al que se le debe enviar el mensaje

    if ((argc <= 4) || (argc > 5)) {
        printf("       <string.txt>: Nombre del archivo config.txt\n");
        printf("       <string.txt>: Nombre del archivo route_table.txt\n");
        printf("        <protocol>: tipo del mensaje a enviar\n");
        printf("        <ip>: ip del pc del cual necesitas su MAC\n");
        exit(-1);
    }

    //procesamos los argumentos

    char *config_name = argv[1];
    char *route_table_name = argv[2];
    uint8_t protocol = (uint8_t) argv[3];
    char *ip_str = argv[4];

    //comprobamos que la IP es valida
    ipv4_addr_t ip_addr;
    if (ipv4_str_addr(ip_str, ip_addr) != 0) {
        printf("Ip no valida\n");
        exit(-1);
    }

    //Abrimos la interfaz y comprobamos que se leyo el archivo
    ipv4_layer_t *ip_layer = ipv4_open(config_name, route_table_name);

    if (ip_layer == NULL) {
        printf("No se pudo leer correctamente el fichero config.txt\n");
        exit(-1);
    }

    //rellenamos el payload de numeros aleatorios
    unsigned char payload[MRU];
    int i;
    for (i = 0; i < MRU; i++) {
        payload[i] = (unsigned char) i;
    }

    //mandamos el mensaje, Que protocolo?
    if (ipv4_send(ip_layer, ip_addr, protocol, payload, sizeof(payload)) == -1) {
        printf("No se pudo enviar el paquete");
        exit(-1);
    }
    printf("enviado el paquete");



}