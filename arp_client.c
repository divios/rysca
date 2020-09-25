#include "arp.h"
#include "eth.h"
#include "ipv4.h"

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
    if ((argc < 1) || (argc > 2)) {
        printf("Uso: %s <iface> <ip> [<long>]\n", myself);
        printf("       <iface>: Nombre de la interfaz ARP\n");
        printf("        <ip>: ip del pc del cual necesitas su MAC\n");
        exit(-1);
    }

    //procesamos los argumentos
    char *iface_name = argv[1];
    ipv4_addr_t ipv4_addr_dest;

    if (ipv4_str_addr ( argv[2], ipv4_addr_dest)){
        printf("Direccion ip erronea");
        exit(-1);
    }


    //abrimos el puerto
    eth_iface_t * iface = eth_open(iface_name);
    if ( iface== NULL) {
        printf("No se pudo abrir la interfaz");
        exit(-1)
    }
    mac_addr_t mac;

    int resolve = arp_resolve(iface, ipv4_addr_dest, mac);

    if (resolve == -2){
        printf("No se pudo enviar el mensaje arp request");
        exit(-1);
    }else if(resolve == -1){
        printf("No se recibio ningun ARP reply");
        exit(-1);
    }

    printf("ip destino -> Mac destino");
    eth_close(iface);


}