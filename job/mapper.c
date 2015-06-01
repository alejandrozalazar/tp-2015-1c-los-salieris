/*
 * mapper.c
 *
 *  Created on: 13/05/2015
 *      Author: utnso
 */

#include "job.h"

void mapper(t_hilo_mapper* hilo_mapper){

	pthread_mutex_lock(&mutexHilosMapper);
	cantHilosMapper++;
	log_info(LOGGER, "comienza a ejecutar el hilo mapper. tid: %d. Cantidad de hilos mapper corriende %d", hilo_mapper->tid, cantHilosMapper);
	pthread_mutex_unlock(&mutexHilosMapper);

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
	if(recibirDeserializado(LOGGER, hilo_mapper->socketNodo, false, mensaje) == ERR_ERROR_AL_RECIBIR_MSG){
		char* contenido = "No pudo establecer conexión con el Nodo";
		log_error(LOGGER, "%s. Se notifica a Marta", contenido);
		enviarSerializado(LOGGER, socketMarta, true, JOB_TO_MARTA_MAP_ERROR, strlen(contenido)+1, contenido);
		pthread_exit(NULL);
	}

	// 2.envio mapper.sh
	enviarSerializado(LOGGER, hilo_mapper->socketNodo, false, JOB_TO_NODO_MAP_SCRIPT, sizeMapperScript, bytesMapperScript);
	recibirDeserializado(LOGGER, hilo_mapper->socketNodo, true, mensaje);

	// 3. envio de los demas parametros
	char* contenido = string_from_format("[%d,%s]", hilo_mapper->nro_bloque, hilo_mapper->archivo_resultado);
	enviarSerializado(LOGGER, hilo_mapper->socketNodo, true, JOB_TO_NODO_MAP_REQUEST, strlen(contenido)+1, contenido);

	pthread_mutex_lock(&mutexHilosMapper);
	cantHilosMapper--;
	log_info(LOGGER, "terminó de ejecutar hilo mapper.tid: %d. Cantidad de hilos corriendo %d", hilo_mapper->tid, cantHilosMapper);
	if(cantHilosMapper == 0){
		log_info(LOGGER, "terminaron de ejecutar todos los hilo mappers. Notifico a MaRTA", hilo_mapper->tid, cantHilosMapper);
	}
	pthread_mutex_unlock(&mutexHilosMapper);

	pthread_exit(NULL);
}
