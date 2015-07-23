/*
 * mapper.c
 *
 *  Created on: 10/06/2015
 *      Author: utnso
 */
#include "job.h"

void notificarError(t_map_request* map_request);

void mapper(t_map_request* map_request)
{
	log_info(LOGGER, "mapper->tid: %d. Comenzamos a ejecutar mapper()", pthread_self());

	int socketNodo, ret;
	log_info(LOGGER, "Intentamos conectarnos al nodo. ip: %s. puerto: %d", map_request->bloque_nodo.nodo.ip,
			map_request->bloque_nodo.nodo.puerto);
	char* ip = string_duplicate(map_request->bloque_nodo.nodo.ip);
	int puerto = map_request->bloque_nodo.nodo.puerto;
	socketNodo = conectarAServidor(ip, puerto);

	if(socketNodo == -1){
		notificarError(map_request);
		free(ip);
		pthread_exit(NULL);
	}

	log_info(LOGGER, "Pudimos conectarnos al Nodo!.......socket del nodo: [%d]", socketNodo);
	log_info(LOGGER, "Enviamos el script del mapper.......[%s]", config_get_string_value(CONFIG, "MAPPER"));
	header_t header;
	initHeader(&header);
	header.tipo = JOB_TO_NODO_MAP_SCRIPT;
	header.cantidad_paquetes = 1;
	header.largo_mensaje = sizeMapperScript;
	enviar_header(socketNodo, &header);
	enviar(socketNodo, bytesMapperScript, header.largo_mensaje);

	initHeader(&header);
	recibir_header_simple(socketNodo, &header);

	if(header.tipo != NODO_TO_JOB_MAP_SCRIPT_OK){
		notificarError(map_request);
		free(ip);
		pthread_exit(NULL);
	}

	char* parametros_a_enviar = string_from_format("[%d,%s]", map_request->bloque_archivo, map_request->archivo_resultado);
	log_info(LOGGER, "Se envía al Nodo [%s]", parametros_a_enviar);

	initHeader(&header);
	header.tipo = JOB_TO_NODO_MAP_REQUEST;
	header.cantidad_paquetes = 1;
	header.largo_mensaje = strlen(parametros_a_enviar) + 1;
	enviar_header(socketNodo, &header);
	enviar(socketNodo, parametros_a_enviar, header.largo_mensaje);

	initHeader(&header);
	recibir_header_simple(socketNodo, &header);

	if(header.tipo != NODO_TO_JOB_MAP_OK){
		notificarError(map_request);
		free(parametros_a_enviar);
		free(ip);
		pthread_exit(NULL);
	}

	log_info(LOGGER, "Mapper ejecutado correctamente. Finalizo.....", parametros_a_enviar);
	pthread_exit(NULL);
}

void notificarError(t_map_request* map_request){
	log_error(LOGGER, "No pudimos conectarnos al nodo. Se notifica a MaRTA");

	header_t header;
	memset(&header, 0, sizeof(header_t));
	header.tipo = JOB_TO_MARTA_MAP_ERROR;

	enviar_header(socketMarta, &header);
	enviar(socketMarta, (char*)map_request, sizeof(t_map_request));
}
