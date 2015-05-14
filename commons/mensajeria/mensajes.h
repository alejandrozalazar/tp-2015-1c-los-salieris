/*
 * mensajes.h
 *
 *  Created on: 25/04/2015
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

typedef enum {

	/*Errores de conexion */
	ERR_CONEXION_CERRADA,
	ERR_ERROR_AL_RECIBIR_MSG,
	ERR_ERROR_AL_ENVIAR_MSG,

	/*enviados desde el job*/
	JOB_TO_MARTA_HANDSHAKE,
	JOB_TO_NODO_MAP_REQUEST,
	JOB_TO_NODO_REDUCE_REQUEST,

	/*Enviados desde el marta*/
	MARTA_TO_JOB_MAP_REQUEST,
	MARTA_TO_JOB_REDUCE_REQUEST,
	MARTA_TO_SERVER,

	/*Enviados desde el nodo*/
	NODO_TO_JOB_MAP_OK,
	NODO_TO_JOB_MAP_KO,
	NODO_TO_JOB_REDUCE_OK,
	NODO_TO_JOB_REDUCE_KO,
	NODO_TO_NODO_GET_BLOQUE,
	NODO_TO_NODO_SET_BLOQUE,
	NODO_TO_NODO_GET_FILE_CONTENT,

	/*Enviados desde el filesystem*/
	FILESYSTEM_TO_NODO_GET_BLOQUE,
	FILESYSTEM_TO_NODO_SET_BLOQUE,
	FILESYSTEM_TO_NODO_GET_FILE_CONTENT,

	FIN
} t_header;

typedef struct tipo_mensaje {
	t_header tipo;
	size_t tamanio;
	char* contenido;
} t_mensaje;

#endif /* MENSAJES_H_ */
