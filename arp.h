#ifndef _ARP_H
#define _ARP_H

#include "eth.h"
#include "ipv4.h"
#include <stdint.h>

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>



struct arp_message;

int arp_resolve(eth_iface_t *iface, ipv4_addr_t destino, mac_addr_t mac);


#endif
