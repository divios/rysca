//
// Created by serbe on 01/12/2020.
//

#ifndef RYSCA_UDP_ROUTE_TABLE_H
#define RYSCA_UDP_ROUTE_TABLE_H

#include "ripv2.h"
#include <stdio.h>

#define RIP_ROUTE_TABLE_SIZE 25
#define RIP_ROUTE_DEFAULT_TIME 180

/* ipv4_route_t * ipv4_route_create
 * ( ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t gw );
 *
 * DESCRIPCIÓN:
 *   Esta función crea una ruta IPv4 con los parámetros especificados:
 *   dirección de subred, máscara, nombre de interfaz y dirección de siguiente
 *   salto.
 *
 *   Esta función reserva memoria para la estructura creada. Debe utilizar la
 *   función 'ipv4_route_free()' para liberar dicha memoria.
 *
 * PARÁMETROS:
 *   'subnet': Dirección IPv4 de la subred destino de la nueva ruta.
 *     'mask': Máscara de la subred destino de la nueva ruta.
 *    'iface': Nombre del interfaz empleado para llegar a la subred destino de
 *             la nueva  ruta.
 *             Debe tener una longitud máxima de 'IFACE_NAME_LENGTH' caracteres.
 *       'gw': Dirección IPv4 del encaminador empleado para llegar a la subred
 *             destino de la nueva ruta.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la ruta creada.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la ruta.
 */
entrada_rip_t *ripv2_route_create
        (ipv4_addr_t subnet, ipv4_addr_t mask, ipv4_addr_t next_hop, int metric);


/*Esta funcion recibe como paraemtro un unsigned char y cuenta
 *todos los bits a 1 en este. Para hacer esto el for no para hasta
 *que mask este a 0s, y con cada itineracion hace un shift a la izquierda
 *de todos los bytes. Luego sumamos si el ultimo byte es 1
 */

int ripv2_switch_lookup(unsigned char mask);


/* int ipv4_route_lookup ( ipv4_route_t * route, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función indica si la dirección IPv4 especificada pertence a la
 *   subred indicada. En ese caso devuelve la longitud de la máscara de la
 *   subred.
 *
 * ************************************************************************
 * * Esta función NO está implementada, debe implementarla usted para que *
 * * funcione correctamente la función 'ipv4_route_table_lookup()'.       *
 * ************************************************************************
 *
 * PARÁMETROS:
 *   'route': Ruta a la subred que se quiere comprobar.
 *    'addr': Dirección IPv4 destino.
 *
 * VALOR DEVUELTO:
 *   Si la dirección IPv4 pertenece a la subred de la ruta especificada, debe
 *   devolver un número positivo que indica la longitud del prefijo de
 *   subred. Esto es, el número de bits a uno de la máscara de subred.
 *   La función devuelve '-1' si la dirección IPv4 no pertenece a la subred
 *   apuntada por la ruta especificada.
 */
int ripv2_route_lookup(entrada_rip_t *route, ipv4_addr_t addr);

/* void ipv4_route_print ( ipv4_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime la ruta especificada por la salida estándar.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea imprimir.
 */
void ripv2_route_print(entrada_rip_t *route);


/* void ipv4_route_free ( ipv4_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la ruta especificada, que
 *   ha sido creada con 'ipv4_route_create()'.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea liberar.
 */
void ripv2_route_free(entrada_rip_t *route);

/* ipv4_route_t* ipv4_route_read ( char* filename, int linenum, char * line )
 *
 * DESCRIPCIÓN:
 *   Esta función crea una ruta IPv4 a partir de la línea del fichero
 *   de la tabla de rutas especificada.
 *
 * PARÁMETROS:
 *   'filename': Nombre del fichero de la tabla de rutas
 *    'linenum': Número de línea del fichero de la tabal de rutas.
 *       'line': Línea del fichero de la tabla de rutas a procesar.
 *
 * VALOR DEVUELTO:
 *   La ruta leída, o NULL si no se ha leido ninguna ruta.
 *
 * ERRORES:
 *   La función imprime un mensaje de error y devuelve NULL si se ha
 *   producido algún error al leer la ruta.
 */
entrada_rip_t *ripv2_route_read(char *filename, int linenum, char *line);


/* void ipv4_route_output ( ipv4_route_t * route, FILE * out );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida indicada la ruta IPv4
 *   especificada.
 *
 * PARÁMETROS:
 *      'route': Ruta a imprimir.
 *     'header': '0' para imprimir una línea con la cabecera de la ruta.
 *        'out': Salida por la que imprimir la ruta.
 *
 * VALOR DEVUELTO:
 *   La función devuelve '0' si la ruta se ha impreso correctamente.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error al escribir por
 *   la salida indicada.
 */
int ripv2_route_output(entrada_rip_t *route, int header, FILE *out);


typedef struct rip_route_table rip_route_table_t;


/* ipv4_route_table_t * ipv4_route_table_create();
 *
 * DESCRIPCIÓN:
 *   Esta función crea una tabla de rutas IPv4 vacía.
 *
 *   Esta función reserva memoria para la tabla de rutas creada, para
 *   liberarla es necesario llamar a la función 'ipv4_route_table_free()'.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la tabla de rutas creada.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la tabla de rutas.
 */
rip_route_table_t *ripv2_route_table_create();

/* int ipv4_route_table_add ( ipv4_route_table_t * table,
 *                            ipv4_route_t * route );
 * DESCRIPCIÓN:
 *   Esta función añade la ruta especificada en la primera posición libre de
 *   la tabla de rutas.
 *
 * PARÁMETROS:
 *   'table': Tabla donde añadir la ruta especificada.
 *   'route': Ruta a añadir en la tabla de rutas.
 *
 * VALOR DEVUELTO:
 *   La función devuelve el indice de la posición [0,IPv4_ROUTE_TABLE_SIZE-1]
 *   donde se ha añadido la ruta especificada.
 *
 * ERRORES:
 *   La función devuelve '-1' si no ha sido posible añadir la ruta
 *   especificada.
 */

int ripv2_route_table_add(rip_route_table_t *table, entrada_rip_t *route);

/* Esta estructura almacena la información básica sobre la ruta a una subred.
 * Incluye la dirección y máscara de la subred destino, el nombre del interfaz
 * de salida, y la dirección IP del siguiente salto.
 *
 * Utilice los métodos 'ipv4_route_create()' e 'ipv4_route_free()' para crear
 * y liberar esta estrucutra. Adicionalmente debe completar la implementación
 * del método 'ipv4_route_lookup()'.
 *
 * Probablemente para construir una tabla de rutas de un protocolo de
 * encaminamiento sea necesario añadir más campos a esta estructura, así como
 * modificar las funciones asociadas.
 */

entrada_rip_t * ripv2_route_table_remove(rip_route_table_t *table, int index);

entrada_rip_t * ripv2_route_table_lookup(rip_route_table_t *table, entrada_rip_t *entrada);

entrada_rip_t * ripv2_route_table_get(rip_route_table_t *table, int index);

int ipv4_route_table_find(rip_route_table_t * table, entrada_rip_t entry_to_find);

void ripv2_route_table_free(rip_route_table_t *table);

int ripv2_route_table_read(char * filename, rip_route_table_t* table);

int ripv2_route_table_output(rip_route_table_t * table, FILE * out);

int ripv2_route_table_write(rip_route_table_t table, char * filename);

void ripv2_route_table_print(rip_route_table_t * entrada);


#endif //RYSCA_UDP_ROUTE_TABLE_H
