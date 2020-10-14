
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <timerms.h>

#include "ipv4.h"
#include "ipv4_route_table.h"
#include "ipv4_config.h"
#include "arp.h"
#include "udp.h"


udp_layer_t * upd_open(char * file_config, char * ip_config, char * route_config){
    /*Comprobamos que los archivos estén correctos*/
    if(strcmp(file_config, NULL)==0){
        fprintf(stderr, "Error al abrir el archivo. Error %s\n", strerror(ERROR));
    }
    if(strcmp(ip_config, NULL)==0){
        fprintf(stderr, "Error al abrir el archivo. Error %s\n", strerror(ERROR));
    }
    if(strcmp(route_config, NULL)==0){
        fprintf(stderr, "Error al abrir el archivo. Error %s\n", strerror(ERROR));
    }
    /*Reservamos memoria para el layer de UDP que retornaremos al final*/
    udp_layer_t * udp_layer = malloc(sizeof(udp_layer_t));
    /*Rellenamos los campos de nuestro udp_layer_t a retornar*/
    int i =0;
    char * aux_text = malloc(sizeof(char)*16);
    for(i=0; i<strlen(file_config);i++){
        if(file_config[i]=='\n'){
            break;
        }else{
            aux_text = concat(char[i], aux_text);
        }
    }
    udp_layer->source_port = atoi(aux_text);
    strcpy(aux_text, "");
    i=0;
    for(i=0; i<strlen(file_config);i++){
        if(file_config[i]=='\n'){
            break;
        }else{
            aux_text = concat(char[i], aux_text);
        }
    }
    udp_layer->destination_port = atoi(aux_text);
    /*NOTA: darse cuenta de que en estos puertos se está usando la función atoi()
    * para convertir el string leído en números para nuestros puertos (concretamente, uint16_t)*/
    strcpy(aux_text, "");
    i=0;
    for(i=0; i<strlen(file_config);i++){
        if(file_config[i]=='\n'){
            break;
        }else{
            aux_text = concat(char[i], aux_text);
        }
    }
    udp_layer->payload_len = atoi(aux_text);
    strcpy(aux_text, "");
    i=0;
    for(i=0; i<strlen(file_config);i++){
        if(file_config[i]=='\n'){
            break;
        }else{
            aux_text = concat(char[i], aux_text);
        }
    }
    udp_layer->payload = aux_text;
    strcpy(aux_text, "");
    i=0;
    /*Una vez rellenado el penúltimo campo, pasamos a rellenar la ip_layer_t con el método ipv4_open*/
    udp_layer->ipv4_layer = ipv4_open(ip_config, route_config);
    /*Y devolvemos nuesta udp_layer_t*/
    return udp_layer;
}


int udp_send(udp_layer_t *layer, ipv4_addr_t dst, uint16_t protocol, unsigned char data[], int payload_len) {

    if (layer == NULL) {
        printf("Hubo un fallo al inicializar el UDP layer\n");
        return -1
    }

    if (payload_len == 0 || payload_len < 0) {
        printf("Payload de UDP no valido\n");
        return -1;
    }

    //Rellenamos paquete
    udp_packet_t packet;

    packet.src_port = layer->source_port; //suponiendo que se hace un htons ya en el open
    packet.dst_port = layer->destination_port; //suponiendo que se hace un htons ya en el open
    packet.len = htons(UDP_HEADER_LEN);
    packet.payload = (unsigned char *) data;

    int packet_len_to_send = UDP_HEADER_LEN + payload_len; // se podria hacer un sizeof directamente pero queda mas
    //bonito asi parece ser

    if (ipv4_send(layer->ipv4_layer, dst, protocol, (unsigned char *) &packet, packet_len_to_send) == -1) {
        //ya manda el warning el ipv4_send, no hace falta hacer printf
        return -1;
    }


}

