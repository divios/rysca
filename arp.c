#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <timerms.h>

#define ARP_TYPE 0x0806
#define IP_CONSTANT 0x0800

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
    arp_payload.hard_addr = 1;// correspondiente a etf
    arp_payload.protocol_type = IP_CONSTANT; //correspondiente a ip
    arp_payload.hard_size = 6;//pq eth tiene 6 octetos
    arp_payload.protocol_length = 4;
    arp_payload.opcode = 1; //1 request; 2 reply
    arp_payload.mac_sender = mac;
    arp_payload.ip_sender = {0, 0, 0, 0};
    arp_payload.mac_target = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    arp_payload.ip_target = destino;


    if (eth_send(iface, destino, ARP_TYPE, (unsigned char *) &arp_payload, sizeof(arp_payload)) == -1) {
        return -1; //si no se ha podido enviar retornamos -1
    }

    arp_message_t buffer; //nos hemos quedado aquí
    int resolvedMac = -1;

    timerms_reset(&timer, timeout); //arrancamos el timer

    while (timerms_left(&timer) != 0) {

        eth_recv(iface, mac, ARP_TYPE, buffer, sizeof(struct arp_message_t),
                 timerms_left(&timer)); //solo recibimos si el mensaje es del tipo arp y esta dirigido a nuestra mac

        buffer = (struct arp_message) buffer; //eth_recv nos devuelve del tipo undefined char, asi que convertimos (no estoy muy seguro de esto)

        if (buffer.ip_sender == destino &&
            buffer.opcode == 2) {  //comprobamos que proviene de la ip que buscamos y ademas es arp reply
            resolvedMac = buffer.mac_sender;  //todo esto se puede hacer con memcpy...
            break;
        }
    }

    return resolvedMac;

}
