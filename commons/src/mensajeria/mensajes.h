/*
 * mensajes.h
 *
 *  Created on: 25/04/2015
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#define NAME_SIZE 32

#include "../commons/collections/dictionary.h"
#include "../commons/collections/list.h"

typedef char t_nombre[NAME_SIZE+1];

typedef enum {

	/*Errores de conexion */
	ERR_CONEXION_CERRADA,
	ERR_ERROR_AL_RECIBIR_MSG,
	ERR_ERROR_AL_ENVIAR_MSG,

	/*enviados desde el job*/
	JOB_TO_MARTA_HANDSHAKE,
	JOB_TO_MARTA_FILES,
	JOB_TO_MARTA_MAP_OK,
	JOB_TO_MARTA_REDUCE_OK,
	JOB_TO_MARTA_MAP_ERROR,
	JOB_TO_MARTA_REDUCE_ERROR,
	JOB_TO_MARTA_MAP_FINISH_OK,
	JOB_TO_MARTA_REDUCE_FINISH_OK,
	JOB_TO_MARTA_MAP_FINISH_ERROR,
	JOB_TO_MARTA_REDUCE_FINISH_ERROR,
	JOB_TO_NODO_HANDSHAKE,
	JOB_TO_NODO_MAP_REQUEST,
	JOB_TO_NODO_REDUCE_REQUEST,
	JOB_TO_NODO_MAP_SCRIPT,
	JOB_TO_NODO_REDUCE_SCRIPT,
	JOB_TO_NODO_MAP_PARAMS,
	JOB_TO_NODO_REDUCE_PARAMS,

	/*Enviados desde el marta*/
	MARTA_TO_JOB,
	MARTA_TO_JOB_MAP_REQUEST,
	MARTA_TO_JOB_REDUCE_REQUEST,
	MARTA_TO_JOB_FILE_NOT_FOUND,
	MARTA_TO_JOB_JOB_FINISHED,
	MARTA_TO_FS_BUSCAR_ARCHIVO,

	/*Enviados desde el nodo*/
	NODO_TO_JOB_MAP_OK,
	NODO_TO_JOB_MAP_KO,
	NODO_TO_JOB_REDUCE_OK,
	NODO_TO_JOB_REDUCE_KO,
	NODO_TO_JOB_MAP_SCRIPT_OK,
	NODO_TO_JOB_MAP_SCRIPT_KO,
	NODO_TO_JOB_REDUCE_SCRIPT_OK,
	NODO_TO_JOB_REDUC_SCRIPTE_KO,
	NODO_TO_NODO_GET_BLOQUE,
	NODO_TO_NODO_SET_BLOQUE,
	NODO_TO_NODO_GET_FILE_CONTENT,

	/*Enviados desde el filesystem*/
	FS_TO_MARTA_BLOQUES_ARCHIVO,
	FS_TO_NODO_GET_BLOQUE,
	FS_TO_NODO_SET_BLOQUE,
	FS_TO_NODO_GET_FILE_CONTENT,

	FIN
} t_header;

typedef struct tipo_mensaje {
	t_header tipo;
	size_t tamanio;
	char* contenido;
} t_mensaje;

/**
 * Estructuras generales que circularan entre FS, Job y MaRTA
 */
typedef struct tipo_nodo {
	t_nombre nombre_nodo;
	t_nombre ip_nodo;
	int puerto_nodo;
} t_nodo;

typedef struct tipo_bloque_nodo {
	t_nodo nodo;
	int nro_bloque;
} t_bloque_nodo;

/**
 * se envia desde el FS al Job
 */
typedef struct tipo_map_request {
	t_nombre archivo_job;
	t_nombre archivo_temporal;
	t_bloque_nodo bloque_nodo;
} t_map_request;

typedef struct tipo_map_response {
	bool respuesta; // OK?
	t_nombre mensaje_respuesta;
	t_nombre archivo_job;
	t_bloque_nodo bloque_nodo;
} t_map_response;

/**
 * se envia desde el FS al Job
 */
typedef struct tipo_reduce_request {
	t_nombre archivo_job;
	t_nombre archivo_temporal;
	t_list* nodos_archivos;
} t_reduce_request;

typedef struct tipo_reduce_response {
	bool respuesta; // OK?
	t_nombre mensaje_respuesta;
	t_nombre archivo_job;
	t_nombre archivo_temporal;
} t_reduce_response;

typedef struct tipo_nodo_archivo {
	t_nodo nodo;
	t_nombre archivo;
} t_nodo_archivo;

#endif /* MENSAJES_H_ */
