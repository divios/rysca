#include "arp.h"
#include "eth.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>


//generar fichero con el cache de ARP
//Inicializarlo a 0??


int main(int argc, char *argv[]) {
    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char *myself = basename(argv[0]);
    if ((argc < 4) || (argc > 5)) {
        printf("Uso: %s <iface> <mac> <dir> [<long>]\n", myself);
        printf("       <iface>: Nombre de la interfaz Ethernet\n");
        printf("        <tipo>: Campo 'Tipo' de las tramas Ethernet\n");
        printf("         <mac>: Dirección MAC del servidor Ethernet\n");
        printf("        <long>: Longitud de los datos enviados al servidor Ethernet\n");
        printf("                (%d bytes por defecto)\n", DEFAULT_PAYLOAD_LENGTH);
        exit(-1);
    }
