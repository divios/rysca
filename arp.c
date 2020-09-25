#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <timerms.h>


#define IP_PROTOCOL 0x0800 //especificamos protocolo ip
#define HARDW_TYPE = 0x0001 //especificamos que el hardware es eth

#define ARP_TYPE 0x0800 //especificamos que el mensaje es de tipo ARP
#define ARP_REQUEST = 0x0001 //simbolo para ARP request
#define ARP_REPLY = 0x0002 //simbolo para ARP reply

extern mac_addr_t MAC_BCAST_ADDR; //mac de broadcast
#define UNKNOW_MAC = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //cuando no sabemos la mac a utilizar


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

    arp_message_t arp_payload;
    arp_payload.hard_addr = HARDW_TYPE;// correspondiente a eth
    arp_payload.protocol_type = IP_PROTOCOL; //correspondiente a ip
    arp_payload.hard_size = 6;//pq eth tiene 6 octetos
    arp_payload.protocol_length = 4;
    arp_payload.opcode = ARP_REQUEST; //1 request; 2 reply
    eth_getaddr(iface, arp_payload.mac_sender); //guardamos en mac_send la mac de la interfaz abierta
    arp_payload.ip_sender = {0, 0, 0, 0}; //hastq que no implementemos la capa ip dejamos esto a 0
    arp_payload.mac_target = UNKNOW_MAC;
    arp_payload.ip_target = destino;


    if (eth_send(iface, MAC_BCAST_ADDR, ARP_TYPE, (unsigned char *) &arp_payload, sizeof(arp_payload)) ==
        -1) { //enviamos en broadcast un arp request
        return -1; //si no se ha podido enviar retornamos -1
    }

    unsigned char buffer; //nos hemos quedado aquí

    timerms_reset(&timer, timeout); //arrancamos el timer

    while (timerms_left(&timer) != 0) {

        eth_recv(iface, arp_payload.mac_sender, ARP_TYPE, buffer, sizeof(struct arp_message_t),
                 timerms_left(&timer)); //solo recibimos si el mensaje es del tipo arp y esta dirigido a nuestra mac

        buffer = (struct arp_message) buffer; //eth_recv nos devuelve del tipo undefined char, asi que convertimos (no estoy muy seguro de esto)

        if (buffer.ip_sender == destino &&
            buffer.opcode == 2) {  //comprobamos que proviene de la ip que buscamos y ademas es arp reply
            mac = buffer.mac_sender;  //todo esto se puede hacer con memcpy...
            break;
        }
    }

    return 1;

}
