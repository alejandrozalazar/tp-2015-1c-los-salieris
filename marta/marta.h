/*
 * marta.h
 *
 *  Created on: 09/05/2015
 *      Author: utnso
 */

#ifndef MARTA_H_
#define MARTA_H_

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

#include <commons/temporal.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/process.h>
#include <commons/collections/list.h>
#include <mensajeria/sockets.h>

#define LOG_PATH "marta.log" //HACK necesitaba un path para no poner config
#define CONF_PATH "marta.conf" //HACK idem

pthread_mutex_t mutex_mapa_nodos;
pthread_mutex_t mutex_lista_jobs;

int socketFS;

t_log* LOGGER;
t_config* CONF;
t_dictionary* mapa_nodos;
t_list * lista_jobs;

void init();
void finish();
bool validarConfig();
void escucha(int puerto);

#endif /* MARTA_H_ */
