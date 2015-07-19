/*
 * network.c
 *
 *  Created on: 10/6/2015
 *      Author: Alejandro Zalazar
 */
#include "network.h"

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

	log_debug(estado->logger, "conectarAFileSystem: enviando handshake %s:  largo mensaje: %d \n", getDescription(header.tipo), header.largo_mensaje);

	if (enviar_header(socketDestino, &header) != EXITO)
	{
		log_error(estado->logger,"%s conectarAFileSystem: Error al enviar handshake\n\n", getDescription(header.tipo));
		return -1;
	}

	t_nodo new_nodo = nuevoNodo(estado->conf->NOMBRE_NODO, estado->conf->IP_NODO, estado->conf->PUERTO_NODO);

	log_debug(estado->logger, "conectarAFileSystem: enviando new_nodo por el socket %d \n", socketDestino);

	if (enviar(socketDestino, (char*)&new_nodo, sizeof(t_nodo)) != EXITO)
	{
		log_error(estado->logger,"%s conectarAFileSystem: Error al enviar t_nodo por el socket %d\n", socketDestino);
		return -1;
	}

	return socketDestino;
}


t_nodo nuevoNodo(char* nombre, char* ip, int puerto) {
	t_nodo new_nodo;
	new_nodo.puerto = puerto;
	memcpy(new_nodo.ip, ip, sizeof(t_ip));
	memcpy(new_nodo.nombre, nombre, sizeof(t_nombre));
	new_nodo.disponible = true;
	return new_nodo;
}
