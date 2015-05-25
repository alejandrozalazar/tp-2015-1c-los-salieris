/*
 * mapper.c
 *
 *  Created on: 13/05/2015
 *      Author: utnso
 */

#include "job.h"

void mapper(t_hilo_mapper* hilo_mapper){
	log_info(LOGGER, "comienza a ejecutar el hilo mapper. tid: %d", hilo_mapper->tid);
	t_mensaje* mensaje = NULL;

	if(hilo_mapper->socketNodo == -1){
		char* contenido = "No se pudo establecer conexión con el Nodo";
		log_error(LOGGER, "Se notifica a Marta: [%s]", contenido);
		enviarSerializado(LOGGER, socketMarta, true, JOB_TO_MARTA_MAP_ERROR, strlen(contenido)+1, contenido);
		pthread_exit(NULL);
	}

	// 1. hago handshake con Nodo
	enviarSerializado(LOGGER, hilo_mapper->socketNodo, false, JOB_TO_NODO_HANDSHAKE, 0, NULL);

	// aca tiene que recibir un header nomas
	if(recibirDeserializado(LOGGER, false, hilo_mapper->socketNodo, mensaje) == ERR_ERROR_AL_RECIBIR_MSG){
		char* contenido = "No pudo establecer conexión con el Nodo";
		log_error(LOGGER, "%s. Se notifica a Marta", contenido);
		enviarSerializado(LOGGER, socketMarta, true, JOB_TO_MARTA_MAP_ERROR, strlen(contenido)+1, contenido);
		pthread_exit(NULL);
	}

	// 2.envio mapper.sh
	enviarSerializado(LOGGER, hilo_mapper->socketNodo, false, JOB_TO_NODO_MAP_SCRIPT, sizeMapperScript, bytesMapperScript);
	recibirDeserializado(LOGGER, true, hilo_mapper->socketNodo, mensaje);

	// 3. envio de los demas parametros
	char* contenido = string_from_format("[%d,%s]", hilo_mapper->nro_bloque, hilo_mapper->archivo_resultado);
	enviarSerializado(LOGGER, hilo_mapper->socketNodo, true, JOB_TO_NODO_MAP_REQUEST, strlen(contenido)+1, contenido);

	log_info(LOGGER, "terminó de ejecutar hilo mapper.tid: %d", hilo_mapper->tid);
	pthread_exit(NULL);
}
