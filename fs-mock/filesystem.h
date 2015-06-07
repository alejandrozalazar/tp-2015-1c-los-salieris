/*
 * filesystem.h
 *
 *  Created on: 07/06/2015
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/process.h>
#include <commons/collections/list.h>
#include <mensajeria/sockets.h>
#include <mensajeria/mensajes.h>
#include <commons/temporal.h>

#define LOG_PATH "filesystem.log"
#define CONFIG_PATH "filesystem.conf"

t_log* logger;
t_config* config;

pthread_t tid_fs_marta;
pthread_t tid_fs_nodos;

int fs_marta();
int fs_nodos();
void init();
void finish();
void config_load();
void inicializar_estructuras();

// funciones fs_marta
t_nodo generar_nodo(int  i);
t_bloque_nodo generar_bloque_nodo(int  i);
char* serializarContenido(size_t tamanio);


#endif /* FILESYSTEM_H_ */
