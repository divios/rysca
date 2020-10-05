#include "ipv4.h"
#include "ipv4_route_table.h"
#include <arp.h>
#include <eth.h>
#include <stdio.h>
#include <stdlib.h>

/* Dirección IPv4 a cero: "0.0.0.0" */
ipv4_addr_t IPv4_ZERO_ADDR = {0, 0, 0, 0};


typedef struct ipv4_layer {

   eth_iface *iface;
   ipv4_addr_t addr;
   ipv4_addr_t network;
   ipv4_route_table_t *routing_table;

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

   ipv4_layer_t ipv4_layer = (malloc(sizeof(ipv4_layer_t));
   ipv4_layer->routing_table = ;
   ipv4_layer->iface = eth_open();
   ipv4_layer->addr = ;
   ipv4_layer->network = ;

}


int ipv4_close(ipv4_layer_t *ipv4_layer) {

   //hacer bucle para liberar a cada ruta
   while(ipv4_layer->routing_table!=NULL){
   ipv4_route_table_free(ipv4_layer->routing_table);
   }
   if (!eth_close(ipv4_layer->iface)) {
       return -1;
   }
   free(ipv4_layer);
   return 1;
}
int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol,
unsigned char * payload, int payload_len){
 if(layer==NULL){
   fprintf(stderr, "Error en el IPv4 Layer.\n");
   return -1;
 }
 if( dst ==NULL){
   fprintf(stderr, "Error en la dirección IPv4 destino.Debe especificar una dirección válida.\n");
   return -1;
 }
 /*Ver cómo usar protocol para cifrar el protocolo*/
 if(sizeof(payload)==0){
   fprintf(stderr, "Error en el envío de datos.\n");
   return -1;
 }
 if(payload_len<0){
   fprintf(stderr, "Error en la longitud de Payload. Imposible enviar el datagrama.\n");
   return-1;
 }
 ipv4_route_t* my_route;
 my_route = malloc(sizeof(ipv4_route_t));
 my_route =ipv4_route_table_lookup(layer->routing_table, dst);
 if(my_route==NULL){
   fprintf(stderr, "No hay ruta disponible para transmitir los datos.");
   return -1;
 }
  mac_addr_t my_mac = "\0";
 mac_addr_t your_mac = "\0";
 eth_getaddr(layer->iface, my_mac);
 //eth_getaddr(m, my_mac);
 /*IMPLEMENTAR CABECERA IP*/
  ipv4_datagram_t * ipv4_data;
 ipv4_data = malloc(sizeof(ipv4_datagram_t));
 ipv4_data->mac_1 = my_mac;
 ipv4_data->payload = malloc(sizeof(char)*50);
 strcpy(ipv4_data->payload, payload);
  ipv4_data = (ipv4_datagram_t*) malloc(sizeof(ipv4_datagram_t));
 arp_resolve(layer->iface, dst, my_mac);
 eth_send(layer->iface, my_mac, 0, ipv4_data->payload, payload_len);
 return 1;
}
int ipv4_recv(){return 0;}


