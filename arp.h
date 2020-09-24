#ifndef _ARP_H
#define _ARP_H

#include <stdint.h>


struct arp_message;

int arp_resolve(eth_iface_t *iface, ipv4_addr_t destino, mac_addr_t mac);

