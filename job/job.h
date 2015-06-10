/*
 * job.h
 *
 *  Created on: 18/04/2015
 *      Author: utnso
 */

#ifndef JOB_H_
#define JOB_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/process.h>
#include <mensajeria/sockets.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CONFIG_PATH "job.conf"
#define LOGGER_PATH "job.log"
#define CANT_CONFIG_KEYS 7

typedef struct tipo_hilo_mapper {
	pthread_t tid;
	t_map_request map_request;
} t_hilo_mapper;

typedef struct tipo_hilo_reduce {
	pthread_t tid;
	char* archivo_a_reducir;
	int socketNodo;
	char* archivo_resultado;
	char* archivos_nodos;
} t_hilo_reduce;

t_config* CONFIG;
t_log* LOGGER;
int socketMarta;
char* bytesMapperScript;
size_t sizeMapperScript;
char* bytesReduceScript;
size_t sizeReduceScript;
int cantHilosMapper;
int cantHilosReduce;
pthread_mutex_t mutexHilosMapper;
pthread_mutex_t mutexHilosReduce;

void init();
int enviarArchivosMarta();
void notificar(header_t header, t_header tipo_mensaje);
char* getBytesFromScript(char *path, size_t* tam_archivo);
void finish();
void gestionar_map_request(header_t header);
void atiendeMapper(char* mensaje);
void atiendeReduce(char* mensaje);
t_hilo_mapper* crearEstructuraHiloMapper(char* mensaje);
t_hilo_reduce* crearEstructuraHiloReduce(char* mensaje);
void mapper();
void reduce();
bool validarConfig();

#endif /* JOB_H_ */
