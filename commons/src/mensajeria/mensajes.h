/*
 * mensajes.h
 *
 *  Created on: 25/04/2015
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#define NAME_SIZE 64
#define FILENAME_SIZE 128
#define IP_SIZE 64

#include "../commons/collections/dictionary.h"
#include "../commons/collections/list.h"

typedef char t_nombre[NAME_SIZE+1];
typedef char t_ip[IP_SIZE+1];

typedef enum {

	/*Errores de conexion */
	ERR_CONEXION_CERRADA,
	ERR_ERROR_AL_RECIBIR_MSG,
	ERR_ERROR_AL_ENVIAR_MSG,

	/*enviados desde el job*/
	JOB_TO_MARTA_FILES,
	JOB_TO_MARTA_MAP_OK,
	JOB_TO_MARTA_MAP_ERROR,
	JOB_TO_MARTA_REDUCE_OK,
	JOB_TO_MARTA_REDUCE_ERROR,
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
	MARTA_TO_JOB_FILE_FOUND,
	MARTA_TO_JOB_FILE_NOT_FOUND,
	MARTA_TO_JOB_JOB_FINISHED,
	MARTA_TO_FS_BUSCAR_ARCHIVO,
	MARTA_TO_FS_BUSCAR_BLOQUE_ARCHIVO,

	/*Enviados desde el nodo*/
	NODO_TO_JOB_HANDSHAKE_OK,
	NODO_TO_JOB_MAP_OK,
	NODO_TO_JOB_MAP_KO,
	NODO_TO_JOB_REDUCE_OK,
	NODO_TO_JOB_REDUCE_KO,
	NODO_TO_JOB_MAP_SCRIPT_KO,
	NODO_TO_JOB_REDUCE_SCRIPT_OK,
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
	t_ip nombre_nodo;
	t_ip ip_nodo;
	int puerto_nodo;
} t_nodo;

typedef struct tipo_archivo_nodo {
	t_nombre archivo;
	t_nodo nodo;
} t_archivo_nodo;


typedef struct tipo_bloque_nodo {
	int nro_bloque;
	t_nodo nodo;
} t_bloque_nodo;

typedef struct tipo_bloque_archivo {
	int nro_bloque;
	bool mapeado;
	t_nombre archivo_tmp;
	t_list* bloques_nodo; // t_bloque_nodo
} t_bloque_archivo;

typedef struct tipo_archivo_job {
	int fd_job;
	bool reducido;
	t_list* bloques_archivo; // t_bloque_archivo
} t_archivo_job;

typedef struct tipo_job {
	int fd;
	t_dictionary* archivos; // la key es el path del archivo, la clave es un t_archivo_job
} t_job;

#endif /* MENSAJES_H_ */
