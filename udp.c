
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


udp_layer_t *upd_open(uint16_t src_port, uint16_t dst_port, char *ip_config, char *route_config) {
    /*Comprobamos que los archivos estén correctos
    if (file_config == NULL) {
        fprintf(stderr, "Error al abrir el archivo. Error %s\n", strerror(ERROR));
    }
    if (ip_config == NULL) {
        fprintf(stderr, "Error al abrir el archivo. Error %s\n", strerror(ERROR));
    }
    if (route_config == NULL) {
        fprintf(stderr, "Error al abrir el archivo. Error %s\n", strerror(ERROR));
    }
    Reservamos memoria para el layer de UDP que retornaremos al final*/
    udp_layer_t *udp_layer = malloc(sizeof(udp_layer_t));
    /*Rellenamos los campos de nuestro udp_layer_t a retornar*/
    /*int i = 0;
    char aux_text[sizeof(file_config)];
    char buffer[sizeof(aux_text)];
    strcpy(aux_text, file_config);
    for (i = 0; i < strlen(aux_text); i++) {
        if (aux_text[i] == '\n') {
            break;
            if (aux_text[i] == '\0') {
                strcat(buffer, aux_text[i]);
                break;
            }
        } else {
            strcat(buffer, aux_text[i]);
        }
    }
    udp_layer->source_port = atoi(buffer);
    int aux = i;
    char buff;
    for (i = aux; i < strlen(file_config); i++) {
        if (aux_text[i] == '\n') {
            break;
            if (aux_text[i] == '\0') {
                strcat(buff, aux_text[i]);
                break;
            }
        } else {
            strcat(buff, aux_text[i]);
        }
    }
    udp_layer->destination_port = atoi(buff); 
    NOTA: darse cuenta de que en estos puertos se está usando la función atoi()
    * para convertir el string leído en números para nuestros puertos (concretamente, uint16_t)*/
    //strcpy(aux_text, "");
    //i = 0;
    /*for(i=0; i<strlen(file_config);i++){
      if(file_config[i]=='\n'){
        break;
      }else{
        aux_text = concat(char[i], aux_text);
      }
    }
    udp_layer->payload_len = atoi(aux_text);
    strcpy(aux_text, "");
    i=0;*/
    /*for(i=0; i<strlen(file_config);i++){
      if(file_config[i]=='\n'){
        break;
      }else{
        aux_text = concat(char[i], aux_text);
      }
    }
    udp_layer->payload = aux_text;
    strcpy(aux_text, "");
    i=0;
    Una vez rellenado el penúltimo campo, pasamos a rellenar la ip_layer_t con el método ipv4_open*/
    udp_layer->ipv4_layer = ipv4_open(ip_config, route_config);
    /*Y devolvemos nuesta udp_layer_t*/
    udp_layer->source_port = src_port;
    udp_layer->destination_port = dst_port;
    return udp_layer;
}


int udp_send(udp_layer_t *layer, ipv4_addr_t dst, uint16_t protocol, unsigned char data[], int payload_len) {

    if (layer == NULL) {
        printf("Hubo un fallo al inicializar el UDP layer\n");
        return -1;
    }

    if (payload_len == 0 || payload_len < 0) {
        printf("Payload de UDP no valido\n");
        return -1;
    }

    //Rellenamos paquete
    udp_packet_t packet;
    printf("Hasta aqui bien\n");
    packet.src_port = htons(layer->source_port); //suponiendo que se hace un htons ya en el open
    packet.dst_port = htons(layer->destination_port); //suponiendo que se hace un htons ya en el open
    packet.checksum= 0x000;
    int packet_len_to_send = UDP_HEADER_LEN + payload_len; // se podria hacer un sizeof directamente pero queda mas
    packet.len = htons(packet_len_to_send);
    memcpy(packet.payload, (unsigned char *) data, payload_len);

    
    //bonito asi parece ser
    if (ipv4_send(layer->ipv4_layer, dst, protocol, (unsigned char *) &packet, packet_len_to_send) == -1) {
        //ya manda el warning el ipv4_send, no hace falta hacer printf
        return -1;
    }
       return 1;
}

int udp_recv(udp_layer_t *layer,long int timeout, uint8_t protocol, unsigned char * payload, int payload_len){

    //check_parametros_correctos()
    if (layer == NULL) {
        printf("Error al inicializar UDP layer. \n");
        return -1;
    }

    if (payload_len == 0 || payload_len < 0) {
        printf("Payload de UDP erróneo. \n");
        return -1;
    }


    //crear_variable_timer()
    timerms_t timer_udp;
    timerms_reset(&timer_udp, timeout);

    ipv4_addr_t sender;
    unsigned char buffer[1472 + 8];

    while (1) {
        //escuchar_puerto()
        long int time_left = timerms_left(&timer_udp);

        if (ipv4_recv(layer->ipv4_layer, protocol, buffer, sender, sizeof(buffer), time_left) == -1) {
            printf("Error al recibir el datagrama.\n");
            return -1;
        }
        udp_packet_t * udp_data = NULL;
        udp_data = (udp_packet_t *) buffer;
        
        if (ntohs(udp_data->dst_port) == layer->source_port) { 
            memcpy(payload, udp_data->payload, 1472); 
            printf("Recibido datgrama UDP\n");
            return 1;
        }else{printf("Vaia\n");}
    }


}

void udp_close(udp_layer_t *my_layer) {

    ipv4_close(my_layer->ipv4_layer);
    free(my_layer);

}
