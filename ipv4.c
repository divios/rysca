#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <timerms.h>

#include "ipv4.h"
#include "ipv4_route_table.h"
#include "ipv4_config.h"
#include "arp.h"

//Estructura que guarda toda la informacion de la interfaz
typedef struct ipv4_layer {

    eth_iface_t *iface;
    ipv4_addr_t addr;
    ipv4_addr_t network;
    ipv4_route_table_t *routing_table;

} ipv4_layer_t;

/* Dirección IPv4 a cero: "0.0.0.0" */
ipv4_addr_t IPv4_ZERO_ADDR = {0, 0, 0, 0};

//Estructura para la trama IPV4 (CONSULTAR)
typedef struct ipv4_message {

    uint8_t version;
    uint8_t type;
    uint16_t total_len;
    uint16_t id: 1;
    uint16_t flags_offset;
    uint8_t TTL;
    uint8_t protocol;
    uint16_t checksum;
    ipv4_addr_t source;
    ipv4_addr_t dest;
    unsigned char data[MRU];

} ipv4_message_t;

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

    //Reservamos memoria para el nombre de la interfaz y la struct
    //que guarda toda la informacion necesaria para la interfaz ipv4
    char ifname[IFACE_NAME_MAX_LENGTH];
    ipv4_layer_t *ipv4_layer = malloc(sizeof(ipv4_layer_t));

    //Leemos el fichero de config y guardamos el nombre de la interfaz, la ip y
    //la mascara asociadas a estas
    ipv4_config_read(file_config, ifname, ipv4_layer->addr, ipv4_layer->network);

    //reservamos memoria para una routing table y guardamos
    //la tabla leida del archivo de texto en esta variable
    ipv4_layer->routing_table = malloc(sizeof(IPv4_ROUTE_TABLE_SIZE));
    ipv4_route_table_read(file_conf_route, ipv4_layer->routing_table);

    //Finalmente abrimos a nivel eth con el nombre que nos pasaron
    ipv4_layer->iface = eth_open(ifname);

    return ipv4_layer;

}


int ipv4_send(ipv4_layer_t *layer, ipv4_addr_t dst, uint8_t protocol,
              unsigned char *payload, int payload_len) {

    //Hacemos comprobaciones de los datos
    if (layer == NULL) {
        fprintf(stderr, "Error en el IPv4 Layer.\n");
        return -1;
    }
    /*Ver cómo usar protocol para cifrar el protocolo*/
    if (sizeof(payload) == 0) {
        fprintf(stderr, "Error en el envío de datos.\n");
        return -1;
    }
    if (payload_len < 0) {
        fprintf(stderr, "Error en la longitud de Payload. Imposible enviar el datagrama.\n");
        return -1;
    }

    //Miramos en las tablas el siguiente salto para llegar a dst
    ipv4_route_t *next_jump = malloc(sizeof(ipv4_route_t));
    next_jump = ipv4_route_table_lookup(layer->routing_table, dst);

    if (next_jump->gateway_addr == NULL) {
        fprintf(stderr, "No hay ruta disponible para transmitir los datos.");
        return -1;
    }
        //Si nos devuelve 0.0.0.0, es que no hay siguiente salto y la ip esta en nuestra
        //subred, por lo tanto el siguiente salto es el propio dst
    else if (memcmp(next_jump->subnet_addr, IPv4_ZERO_ADDR, sizeof(ipv4_addr_t) == 0)) {
        memcpy(next_jump->gateway_addr, dst, sizeof(ipv4_addr_t));
    }

    mac_addr_t my_mac = "\0";
    mac_addr_t your_mac = "\0";

    eth_getaddr(layer->iface, my_mac);

    /*CABECERA IP*/

    ipv4_message_t *ipv4_data = malloc(sizeof(ipv4_message_t));

    //RELLENAR TODOS LOS VALORES
    memcpy(ipv4_data->protocol, htons(protocol), sizeof(uint8_t));
    memcpy(ipv4_data->dest, dst, sizeof(ipv4_addr_t));
    memcpy(ipv4_data->source, my_mac, sizeof(ipv4_addr_t));
    ipv4_data->checksum = ipv4_checksum(payload, payload_len);
    memcpy(ipv4_data->data, payload, payload_len);

    //Mandamos ARP resolve para conocer la MAC del siguiente salto
    if (arp_resolve(layer->iface, next_jump->gateway_addr, your_mac)) {
        //No hace falta mandar mensaje, ya lo hace arp_resolve
        return -1;
    }

    if (eth_send(layer->iface, your_mac, 0, (unsigned char *) ipv4_data,
                 sizeof(ipv4_message_t))) { //hay que mirar el tipo
        fprintf(stderr, "No se puedo enviar la trama IPv4");
        return -1;
    }

    return 1;
}


int ipv4_recv(ipv4_layer_t *layer, uint8_t protocol, unsigned char payload[], ipv4_addr_t sender, int payload_len,
              long int timeout) {

    //inicializamos variables

    timerms_t timer;
    timerms_reset(&timer, timeout);

    mac_addr_t mac;
    int buffer_len;

    //creamos variables auxiliares
    int ipv4_buffer_len = payload_len;
    unsigned char ipv4_buffer[ipv4_buffer_len];
    ipv4_message_t *ipv4_frame = NULL;

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
        else if (buffer_len < MRU) {
            printf("Tamaño de trama IPV4 invalida\n");
            continue;
        }

        //Hacemos casting para manejar el buffer como una estructura ip
        ipv4_frame = (ipv4_message_t *) ipv4_buffer;

        //Aqui comprobamos que en el datagram IP sea del tipo que esperamos
        //si es asi, guardamos la payload->sender en sender
        //hacemos break;
        if (!memcmp(ipv4_frame->protocol, protocol, sizeof(uint8_t))) {
            printf("Datagram Ip recibido");
            memcpy(payload, ipv4_frame->data, buffer_len);
            memcpy(sender, ipv4_frame->source, sizeof(ipv4_addr_t));
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
