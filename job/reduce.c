/*
 * reducer.c
 *
 *  Created on: 13/05/2015
 *      Author: utnso
 */

#include "job.h"

void reduce(t_hilo_reduce* hilo_reduce){
	pthread_mutex_lock(&mutexHilosReduce);
	cantHilosReduce++;
	log_info(LOGGER, "comienza a ejecutar el hilo reduce. tid: %d", hilo_reduce->tid);
	pthread_mutex_unlock(&mutexHilosReduce);

	t_mensaje* mensaje = NULL;

	if(hilo_reduce->socketNodo == -1){
		char* contenido = "No se pudo establecer conexión con el Nodo";
		log_error(LOGGER, "Se notifica a Marta: [%s]", contenido);
		enviarSerializado(LOGGER, socketMarta, true, JOB_TO_MARTA_REDUCE_ERROR, strlen(contenido)+1, contenido);
		pthread_exit(NULL);
	}

	// 1. hago handshake con Nodo
	enviarSerializado(LOGGER, hilo_reduce->socketNodo, false, JOB_TO_NODO_HANDSHAKE, 0, NULL);

	// aca tiene que recibir un header nomas
	if(recibirDeserializado(LOGGER, hilo_reduce->socketNodo, false, mensaje) == ERR_ERROR_AL_RECIBIR_MSG){
		char* contenido = "No pudo establecer conexión con el Nodo";
		log_error(LOGGER, "%s. Se notifica a Marta", contenido);
		enviarSerializado(LOGGER, socketMarta, true, JOB_TO_MARTA_REDUCE_ERROR, strlen(contenido)+1, contenido);
		pthread_exit(NULL);
	}

	// 2.envio mapper.sh
	enviarSerializado(LOGGER, hilo_reduce->socketNodo, false, JOB_TO_NODO_MAP_SCRIPT, sizeMapperScript, bytesMapperScript);
	recibirDeserializado(LOGGER, hilo_reduce->socketNodo, true, mensaje);

	char* reduce_args = string_from_format("[%s,%s]", hilo_reduce->archivo_resultado, hilo_reduce->archivos_nodos);
	enviarSerializado(LOGGER, hilo_reduce->socketNodo, true, JOB_TO_NODO_MAP_REQUEST, strlen(reduce_args) + 1, reduce_args);

	pthread_mutex_lock(&mutexHilosReduce);
	log_info(LOGGER, "terminó de ejecutar hilo mapper.tid: %d", hilo_reduce->tid);
	pthread_mutex_unlock(&mutexHilosReduce);

	pthread_exit(NULL);
}
