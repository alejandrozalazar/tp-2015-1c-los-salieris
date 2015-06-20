/*
 * network.c
 *
 *  Created on: 10/6/2015
 *      Author: Alejandro Zalazar
 */
#include "network.h"

//int conectarAFileSystem(t_estado* estado) {
//
//	int socketDestino = conectarAServidor(estado->conf->IP_FS, estado->conf->PUERTO_FS);
//	enviar(socketDestino, "hola", 5);
//	return socketDestino;
//}

int conectarAFileSystem(t_estado* estado) {
	header_t header;

	int socketDestino = conectarAServidor(estado->conf->IP_FS, estado->conf->PUERTO_FS);
	if (socketDestino  < 0)
	{
		log_error(estado->logger,"%s conectarAFileSystem: Error al conectar a file system\n\n", getDescription(header.tipo));
		return -1;
	}

	initHeader(&header);
	header.tipo = NODO_TO_FS_HANDSHAKE;
	header.largo_mensaje = 0;
	header.cantidad_paquetes = 1;

	log_info(estado->logger, "conectarAFileSystem: sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	if (enviar_header(socketDestino, &header) != EXITO)
	{
		log_error(estado->logger,"%s conectarAFileSystem: Error al enviar header\n\n", getDescription(header.tipo));
		return -1;
	}

	header_t mensaje;
	recibir_header_simple(socketDestino, &mensaje);
	header_t* pMensaje = &mensaje;

	if(pMensaje->tipo == FS_TO_NODO_HANDSHAKE_OK) {
		log_info(estado->logger, "Mensaje recibido: [%s] del socket [%d]", getDescription(pMensaje->tipo), socketDestino);
	} else {
		log_error(estado->logger, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  pMensaje->tipo);
		return -1;
	}

	return EXITO;
}
