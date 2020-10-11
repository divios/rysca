#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>

#include "ipv4.h"


int main(int argc, char *argv[]) {
    //Necesitamos como parametros el nombre de la interfaz
    //el nombre del archivo de text de la config y routas
    //y la ip al que se le debe enviar el mensaje

    if ((argc <= 3) || (argc > 4)) {
        printf("       <string.txt>: Nombre del archivo config.txt\n");
        printf("       <string.txt>: Nombre del archivo route_table.txt\n");
        printf("        <protocol>: tipo del mensaje a enviar\n");
        exit(-1);
    }

    //procesamos los argumentos
   
    char *config_name = argv[1];
    char *route_table_name = argv[2];
      
    //Abrimos la interfaz y comprobamos que se leyo el archivo;
    ipv4_layer_t *ip_layer = ipv4_open(config_name, route_table_name);
    
    if (ip_layer == NULL) {
        printf("No se pudo leer correctamente el fichero config.txt\n");
        exit(-1);
    }

        while (1) {

        /* Recibir trama Ethernet del Cliente */
        unsigned char buffer[1480];
        ipv4_addr_t src_addr;
        long int timeout = -1;
        
        printf("Escuchando a tramas ipv4\n");
        int payload_len = ipv4_recv (ip_layer, 0, buffer, src_addr, sizeof(buffer), timeout);

        if (payload_len == -1) {
            printf("Error al recibir la trama");
            exit(-1);
        }

        if (payload_len > 0 ) {
            printf("Recibido el paquete");
            exit(-1);
        }


    }
}