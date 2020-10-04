#include "ipv4.h"
#include "ipv4_route_table.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <timerms.h>
#include "eth.h"

/* Dirección IPv4 a cero: "0.0.0.0" */
ipv4_addr_t IPv4_ZERO_ADDR = {0, 0, 0, 0};


typedef struct ipv4_layer {

    eth_iface *iface;
    ipv4_addr_t addr;
    ipv4_addr_t network;
    ipv4_route_table *routing_table;

} ipv4_layer_t;


/* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección IP que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'IPv4_STR_MAX_LENGTH' bytes.
 */
void ipv4_addr_str(ipv4_addr_t addr, char *str) {
    if (str != NULL) {
        sprintf(str, "%d.%d.%d.%d",
                addr[0], addr[1], addr[2], addr[3]);
    }
}


/* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección IPv4.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección IPv4 encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección IPv4.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección IPv4.
 */
int ipv4_str_addr(char *str, ipv4_addr_t addr) {
    int err = -1;

    if (str != NULL) {
        unsigned int addr_int[IPv4_ADDR_SIZE];
        int len = sscanf(str, "%d.%d.%d.%d",
                         &addr_int[0], &addr_int[1],
                         &addr_int[2], &addr_int[3]);

        if (len == IPv4_ADDR_SIZE) {
            int i;
            for (i = 0; i < IPv4_ADDR_SIZE; i++) {
                addr[i] = (unsigned char) addr_int[i];
            }

            err = 0;
        }
    }

    return err;
}


/*
 * uint16_t ipv4_checksum ( unsigned char * data, int len )
 *
 * DESCRIPCIÓN:
 *   Esta función calcula el checksum IP de los datos especificados.
 *
 * PARÁMETROS:
 *   'data': Puntero a los datos sobre los que se calcula el checksum.
 *    'len': Longitud en bytes de los datos.
 *
 * VALOR DEVUELTO:
 *   El valor del checksum calculado.
 */
uint16_t ipv4_checksum(unsigned char *data, int len) {
    int i;
    uint16_t word16;
    unsigned int sum = 0;

    /* Make 16 bit words out of every two adjacent 8 bit words in the packet
     * and add them up */
    for (i = 0; i < len; i = i + 2) {
        word16 = ((data[i] << 8) & 0xFF00) + (data[i + 1] & 0x00FF);
        sum = sum + (unsigned int) word16;
    }

    /* Take only 16 bits out of the 32 bit sum and add up the carries */
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    /* One's complement the result */
    sum = ~sum;

    return (uint16_t) sum;
}

ipv4_layer_t *ipv4_open(char *file_config, char *file_conf_route) {

    char ifname[IFACE_NAME_MAX_LENGTH];
    ipv4_layer_t *ipv4_layer = (malloc(sizeof(ipv4_layer_t));
    ipv4_config_read(file_config, ifname, ipv4_layer->addr, ipv4_layer->network);

    ipv4_layer->routing_table = malloc(sizeof(ipv4_route_table));
    ipv4_route_table_read(file_conf_route, ipv4_layer->routing_table);
    ipv4_layer->iface = eth_open(ifname);

}


//Aqui iria el send


int ipv4_recv(ipv4_layer_t *layer, uint8_t protocol, unsigned char payload[], ipv4_addr_t sender, int payload_len,
              long int timeout) {

    //inicializamos variables

    timerms_t timer;
    timerms_reset(&timer, timeout);

    mac_addr_t mac;
    int buffer_len = -1;

    //creamos variables auxiliares
    int ipv4_buffer_len = payload_len + ETH_HEADER_SIZE + IPV4_HEADER_SIZE;
    unsigned char ipv4_buffer[ipv4_buffer_len];
    ipv4_payload_t *ipv4_frame = NULL;

    while (1) {

        //Miramos cuanto tiempo nos falta
        long int time_left = timers_left(&timer);

        //recibimos el mensaje
        buffer_len = eth_recv(layer->iface, mac, protocol, ipv4_buffer, payload_len, time_left);

        //Si es un error (-1) y si el tiempo se ha acabado sin recibir ningun mensaje (0), retornamos -1
        //Se puede distinguir entre las dos si queremos...
        if (buffer_len == -1 || buffer_len == 0) {
            printf("No se recibio el paquete");
            return -1;
        }
            //si por alguna razon el buffer que nos devuelve es menor que
            //la longitud minima que deberia tener un datagram, es decir la cabecera de ipv4
            //seguimos con la siguiente itineracion
        else if (buffer_len < IPV4_HEADER_SIZE) {
            printf("Tamaño de trama IPV4 invalida\n");
            continue;
        }

        //Hacemos casting para manejar el buffer como una estructura ip
        ipv4_frame = (ipv4_payload_t *) ipv4_buffer;

        //Aqui comprobamos que en el datagram IP sea del tipo que esperamos
        //si es asi, guardamos la payload->sender en sender
        //hacemos break;
        if (!memcpr(ipv4_frame->protocol, protocol, sizeof(uint8_t))) {
            printf("Datagram Ip recibido");
            memcpy(payload, ipv4_frame->payload, buffer_len);
            memcpy(sender, ipv4_frame->sender, sizeof(ipv4_addr_t));
            break;
        }

    }

    return buffer_len;

}


int ipv4_close(ipv4_layer_t *ipv4_layer) {


    ipv4_route_table_free(ipv4_layer->routing_table);

    if (!eth_close(ipv4_layer->iface)) {
        return -1;
    }
    free(ipv4_layer);
    return 1;
}
