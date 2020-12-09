//
// Created by serbe on 01/12/2020.
//

#ifndef RYSCA_UDP_ROUTE_TABLE_H
#define RYSCA_UDP_ROUTE_TABLE_H

#include "ripv2.h"
#include <stdio.h>

#define RIP_ROUTE_DEFAULT_TIME 180

entrada_rip_t *ripv2_route_create
        (ipv4_addr_t subnet, ipv4_addr_t mask, ipv4_addr_t next_hop, int metric);

int ripv2_switch_lookup(unsigned char mask);

int ripv2_route_lookup(entrada_rip_t *route, ipv4_addr_t addr);

void ripv2_route_print(entrada_rip_t *route);

void ripv2_route_free(entrada_rip_t *route);

entrada_rip_t *ripv2_route_read(char *filename, int linenum, char *line);

int ripv2_route_output(entrada_rip_t *route, int header, FILE *out);

rip_route_table_t *ripv2_route_table_create();

int ripv2_route_table_add(rip_route_table_t *table, entrada_rip_t *route);

entrada_rip_t * ripv2_route_table_remove(rip_route_table_t *table, int index);

entrada_rip_t * ripv2_route_table_lookup(rip_route_table_t *table, entrada_rip_t *entrada);

entrada_rip_t * ripv2_route_table_get(rip_route_table_t *table, int index);

int ipv4_route_table_find(rip_route_table_t * table, entrada_rip_t *entry_to_find);

void ripv2_route_table_free(rip_route_table_t *table);

int ripv2_route_table_read(char * filename, rip_route_table_t* table);

int ripv2_route_table_output(rip_route_table_t * table, FILE * out);

int ripv2_route_table_write(rip_route_table_t *table, char * filename);

void ripv2_route_table_print(rip_route_table_t * entrada);


#endif //RYSCA_UDP_ROUTE_TABLE_H
