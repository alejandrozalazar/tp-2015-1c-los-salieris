#ifndef COMUNICACION_H
#define COMUNICACION_H

//include comunicacion.h
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/tcp.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include "mensajes.h"

#define ERROR		    0
#define EXITO		    1
#define WARNING         2

#pragma pack(1)
typedef struct
{
  char id[16];
  t_header tipo;
  int  largo_mensaje;
  int cantidad_paquetes;
}header_t;
#pragma pack(0)

header_t* crearHeader();
void initHeader(header_t* header);
int enviar(int sock, char *buffer, int tamano);
int recibir(int sock, char *buffer, int tamano);
int conectar(t_ip ip, int puerto, int *sock);
int aceptar_conexion(int *listener, int *nuevo_sock);

int enviar_header (int sock, header_t *header);
int recibir_header(int sock, header_t *header, fd_set *master/*por si se desconecta*/, int *seDesconecto);
int recibir_header_simple(int sock, header_t *header);
int agregar_descriptor(int desc, fd_set *listaDesc, int *maxDesc);
int quitar_descriptor(int desc, fd_set *listaDesc, int *maxDesc);
int crear_listener(int puerto, int *listener);
void genId(char idMsg[]);

int enviar_string(int sock, char* string);
int recibir_string(int sock, char* string, int tamanio, bool* seDesconecto);
int enviar_map_request(int sock, t_map_request* map_request);
int recibir_map_request(int sock, t_map_request* map_request, bool* seDesconecto);
int enviar_map_request_nodo(int sock, t_map_request_nodo* request);
int recibir_map_request_nodo(int sock, t_map_request_nodo* request, bool* seDesconecto);


#endif
