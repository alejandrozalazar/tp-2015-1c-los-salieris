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

	int socketDestino = conectarAServidorYLoguear(estado->conf->IP_FS, estado->conf->PUERTO_FS, estado->logger);
	if (socketDestino  < 0)
	{
		log_error(estado->logger,"%s conectarAFileSystem: Error al conectar a file system\n\n", getDescription(header.tipo));
		return -1;
	}

	initHeader(&header);
	header.tipo = NODO_TO_FS_HANDSHAKE;
	header.largo_mensaje = 0;
	header.cantidad_paquetes = 1;

	log_info(estado->logger, "conectarAFileSystem: enviando handshake sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	if (enviar_header(socketDestino, &header) != EXITO)
	{
		log_error(estado->logger,"%s conectarAFileSystem: Error al enviar handshake\n\n", getDescription(header.tipo));
		return -1;
	}

	return socketDestino;
}
