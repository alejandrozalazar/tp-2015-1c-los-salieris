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
	uint nro_bloque;
	int socketNodo;
	char* archivo_resultado;
} t_hilo_mapper;

typedef struct tipo_hilo_reduce {
	pthread_t tid;
	int socketNodo;
	char* archivo_resultado;
	char* archivos_nodos;
} t_hilo_reduce;

t_config* CONFIG;
t_log* LOGGER;
int32_t socketMarta;
char* bytesMapperScript;
size_t sizeMapperScript;
char* bytesReduceScript;
size_t sizeReduceScript;

t_config* loadConfig(char *path, t_log* logger);
void finish();
void atiendeMapper(char* mensaje);
void atiendeReduce(char* mensaje);
t_hilo_mapper* crearEstructuraHiloMapper(char* mensaje);
t_hilo_reduce* crearEstructuraHiloReduce(char* mensaje);
void mapper();
void reduce();
bool validarConfig();
t_header indicarArchivosJob();
char* getBytesFromScript(char *path, size_t* tam_archivo);

#endif /* JOB_H_ */
