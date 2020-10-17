
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


int udp_send(udp_layer_t *layer, ipv4_addr_t dst, uint16_t protocol, unsigned char payload[], int payload_len) {

    if (layer == NULL) {
        printf("Hubo un fallo al inicializar el UDP layer\n");
        return -1;
    }

    if (payload_len <= 0) {
        printf("Payload de UDP no valido\n");
        return -1;
    }

    //Rellenamos paquete
    udp_packet_t udp_frame;
    udp_frame.src_port = htons(layer->source_port);
    udp_frame.dst_port = htons(layer->destination_port);
    udp_frame.checksum = 0x000;
    int udp_frame_len = UDP_HEADER_LEN + payload_len;
    udp_frame.len = htons(udp_frame_len);
    memcpy(udp_frame.payload, (unsigned char *) payload, payload_len);


    //bonito asi parece ser
    int bytes_send = ipv4_send(layer->ipv4_layer, dst, protocol, (unsigned char *) &udp_frame, udp_frame_len);

    if (bytes_send == -1) {
        //ya manda el warning el ipv4_send, no hace falta hacer printf
        return -1;
    }
    return (bytes_send - UDP_HEADER_LEN);
}

int udp_recv(udp_layer_t *layer, long int timeout, uint8_t protocol, unsigned char *buffer, int buffer_len) {

    //check_parametros_correctos()
    if (layer == NULL) {
        printf("Error al inicializar UDP layer. \n");
        return -1;
    }

    if (buffer_len <= 0) {
        printf("Payload de UDP erróneo. \n");
        return -1;
    }

    //crear_variable_timer()
    timerms_t timer_udp;
    timerms_reset(&timer_udp, timeout);

    ipv4_addr_t sender;
    udp_packet_t *udp_frame = NULL;
    int frame_len;
    int udp_buffer_len = buffer_len + UDP_HEADER_LEN;
    unsigned char udp_buffer[buffer_len];

    while (1) {
        //escuchar_puerto()
        long int time_left = timerms_left(&timer_udp);

        frame_len = ipv4_recv(layer->ipv4_layer, protocol, udp_buffer, sender, udp_buffer_len, time_left);

        if (frame_len == -1) {
            printf("Error al recibir el datagrama.\n");
            return -1;
        }

        udp_frame = (udp_packet_t *) udp_buffer;

        if (ntohs(udp_frame->dst_port) == layer->source_port) {
            break;
        }
    }

    if (buffer_len > frame_len) {
        buffer_len = frame_len;
    }

    memcpy(buffer, udp_frame->payload, buffer_len);

    return 1;

}

void udp_close(udp_layer_t *my_layer) {

    ipv4_close(my_layer->ipv4_layer);
    free(my_layer);

}
