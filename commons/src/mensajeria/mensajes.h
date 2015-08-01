/*
 * mensajes.h
 *
 *  Created on: 25/04/2015
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#define TAMCONTENIDO 256
#define NAME_SIZE 64
#define FILENAME_SIZE 128
#define IP_SIZE 15

#include <stddef.h>
#include "../commons/collections/dictionary.h"
#include "../commons/collections/list.h"

typedef char t_nombre[NAME_SIZE+1];
typedef char t_ip[IP_SIZE+1];

typedef enum {

	/*Errores de conexion */
	ERR_CONEXION_CERRADA,
	ERR_ERROR_AL_RECIBIR_MSG,
	ERR_ERROR_AL_ENVIAR_MSG,

	/* recibido OK. recibido no OK*/
	ACK,
	NAK,

	/*enviados desde el job*/
	JOB_TO_MARTA_FILES,
	JOB_TO_MARTA_MAP_OK,
	JOB_TO_MARTA_MAP_ERROR,
	JOB_TO_MARTA_REDUCE_OK,
	JOB_TO_MARTA_REDUCE_ERROR,
	JOB_TO_NODO_HANDSHAKE,
	JOB_TO_NODO_MAP_REQUEST,
	JOB_TO_NODO_REDUCE_REQUEST,
	JOB_TO_NODO_MAP_SCRIPT,
	JOB_TO_NODO_REDUCE_SCRIPT,
	JOB_TO_NODO_MAP_PARAMS,
	JOB_TO_NODO_REDUCE_PARAMS,

	/*Enviados desde el marta*/
	MARTA_TO_JOB_ERROR,
	MARTA_TO_JOB_MAP_REQUEST,
	MARTA_TO_JOB_REDUCE_REQUEST,
	MARTA_TO_JOB_FILE_FOUND,
	MARTA_TO_JOB_FILE_NOT_FOUND,
	MARTA_TO_JOB_FILE_ERROR,
	MARTA_TO_JOB_JOB_FINISHED,
	MARTA_TO_FS_HANDSHAKE,
	MARTA_TO_FS_BUSCAR_ARCHIVO,
	MARTA_TO_FS_BUSCAR_BLOQUE_ARCHIVO,
	MARTA_TO_FS_NODO_REQUEST,

	/*Enviados desde el nodo*/
	NODO_TO_FS_HANDSHAKE,
	NODO_TO_FS_GET_BLOQUE_OK,
	NODO_TO_FS_GET_BLOQUE_KO,
	NODO_TO_JOB_HANDSHAKE_OK,
	NODO_TO_JOB_MAP_OK,
	NODO_TO_JOB_MAP_KO,
	NODO_TO_JOB_MAP_SCRIPT_OK,
	NODO_TO_JOB_REDUCE_OK,
	NODO_TO_JOB_REDUCE_KO,
	NODO_TO_JOB_REDUCE_SCRIPT_OK,
	NODO_TO_NODO_GET_BLOQUE,
	NODO_TO_NODO_GET_BLOQUE_OK,
	NODO_TO_NODO_GET_BLOQUE_KO,
	NODO_TO_NODO_SET_BLOQUE,
	NODO_TO_NODO_GET_FILE_CONTENT,

	/*Enviados desde el filesystem*/
	FS_TO_MARTA_BLOQUES_ARCHIVO,
	FS_TO_MARTA_ESPERAR_ACTIVACION,
	FS_TO_NODO_HANDSHAKE_OK,
	FS_TO_NODO_GET_BLOQUE,
	FS_TO_NODO_SET_BLOQUE,
	FS_TO_NODO_GET_FILE_CONTENT,

	FIN
} t_header;

typedef struct tipo_mensaje {
	t_header tipo;
	size_t tamanio;
	char contenido[256];
} t_mensaje;


/**
 * Estructuras generales que circularan entre FS, Job y MaRTA
 */
typedef struct tipo_nodo {
	t_nombre nombre;
	t_ip ip;
	int puerto;
	bool disponible;
	int carga; // del FS siempre va a estar en 0
	int fd;
} t_nodo;

typedef struct tipo_archivo_nodo {
	t_nombre archivo;
	int tamanioArchivo;
	t_nodo nodo;
} t_archivo_nodo;

typedef struct tipo_bloque_nodo {
	t_nodo nodo;
	int nro_bloque;
} t_bloque_nodo;

typedef struct tipo_numero_bloque {
	int nro_bloque;
} t_nro_bloque;

typedef struct tipo_bloque_archivo {
	int nro_bloque;
	bool mapeado;
	t_nombre archivo_tmp;
	t_bloque_nodo bloques_nodo[3];
} t_bloque_archivo;

typedef struct tipo_archivo_job {
	bool reducido;
	int size_vec_bloques;
	int cant_mapeados;
} t_archivo_job;

typedef struct tipo_job {
	int fd;
	t_dictionary* archivos; // la key es el path del archivo, la clave es un t_archivo_job
} t_job;

// estructuras que se reciben en el proceso job
typedef struct tipo_map_request {
	t_bloque_nodo bloque_nodo;
	t_nombre archivo_resultado;
	t_nombre archivo_origen;
	int bloque_archivo;
} t_map_request;

typedef struct tipo_reduce_request {
	t_nombre archivoScript;
	int cantTipoNodo;
} t_reduce_request;

typedef struct tipo_resultado_ejecucion {
	t_nombre nombreArchivo;
	int tamanioArchivo;
} t_result_exec;

// estructuras que se reciben en el proceso nodo
#pragma pack(1)
typedef struct tipo_map_request_nodo {
	t_nombre nombre_script;
	int tamanio_script;
	int nro_bloque;
	t_nombre archivo_resultado;
} t_map_request_nodo;
#pragma pack(0)

#endif /* MENSAJES_H_ */
