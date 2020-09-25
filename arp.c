#include "arp.h"
#include "eth.h"
#include "ipv4.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>



#define IP_PROTOCOL 0x0800 //especificamos protocolo ip
#define HARDW_TYPE 0x0001 //especificamos que el hardware es eth

#define ARP_TYPE 0x0800 //especificamos que el mensaje es de tipo ARP
#define ARP_REQUEST 0x0001 //simbolo para ARP request
#define ARP_REPLY 0x0002 //simbolo para ARP reply

extern mac_addr_t MAC_BCAST_ADDR; //mac de broadcast
#define UNKNOW_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //cuando no sabemos la mac a utilizar


timerms_t timer;
long int timeout = 5000; //5segundos
//definimos la cabecera

typedef struct arp_message {
    uint16_t hard_addr; //mac
    uint16_t protocol_type;
    uint8_t hard_size;
    uint8_t protocol_length;
    uint16_t opcode;
    mac_addr_t mac_sender;
    ipv4_addr_t ip_sender;
    mac_addr_t mac_target;
    ipv4_addr_t ip_target;

} arp_message_t;


int arp_resolve(eth_iface_t *iface, ipv4_addr_t destino, mac_addr_t mac) {

    //Creamos y rellenamos la estructura de tipo arp_message que se utilizara como payload
    arp_message_t arp_payload;
    arp_payload.hard_addr = htons(HARDW_TYPE);// correspondiente a eth
    arp_payload.protocol_type = htons(IP_PROTOCOL); //correspondiente a ip
    arp_payload.hard_size = 6;//pq eth tiene 6 octetos
    arp_payload.protocol_length = 4;
    arp_payload.opcode = htons(ARP_REQUEST); //1 request; 2 reply
    eth_getaddr(iface, arp_payload.mac_sender); //guardamos en mac_send la mac de la interfaz abierta
    arp_payload.ip_sender = {0, 0, 0, 0}; //hastq que no implementemos la capa ip dejamos esto a 0
    arp_payload.mac_target = UNKNOW_MAC;
    arp_payload.ip_target = destino;

    //enviamos en broadcast un arp request
    if (eth_send(iface, MAC_BCAST_ADDR, ARP_TYPE, (unsigned char *) &arp_payload, sizeof(arp_payload)) ==
        -1) {

        return -2; //si no se ha podido enviar retornamos -2
    }
    printf("Enviado arp request")

    unsigned char buffer;
    struct arp_message_t arp_message;
    int ecoARP = 0;

    timerms_reset(&timer, timeout); //arrancamos el timer

    //escuchamos a la respuesta mientras que el timer siga vivo
    while (timerms_left(&timer) != 0) {


        //si han pasado 2 segundos y no hemos recibido respuesta mandamos otra vez
        if (timerms_left(&timer) <= 3000 && secondARP == 0) {
            eth_send(iface, MAC_BCAST_ADDR, ARP_TYPE, (unsigned char *) &arp_payload, sizeof(arp_payload));
            ecoARP = 1;
        }

        eth_recv(iface, mac, ARP_TYPE, buffer, sizeof(struct arp_message_t),
                 timerms_left(&timer)); //solo recibimos si el mensaje es del tipo arp y esta dirigido a nuestra mac.
        // El segundo parametro no importa porque ni siquiera se comprueba

        arp_message = (struct arp_message) buffer; //eth_recv nos devuelve del tipo undefined char, asi que convertimos

        //comprobamos que proviene de la ip que buscamos y ademas es arp reply
        //seguramente no necesitamos comprobar que el destino puesto que nos puede responder cualquier pc
        if (arp_message.ip_sender == destino &&
                ntohs(arp_message.opcode) == ARP_REPLY) {
            //no hace falta guardar la mac puesto que ya lo hace eth_recv
            printf("ARP reply recibido")
            return 1;
        }
        mac = NULL; //si no es lo que esperamos liberamos la mac que guardamos
    }

    return -1;

}
