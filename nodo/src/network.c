/*
 * network.c
 *
 *  Created on: 10/6/2015
 *      Author: Alejandro Zalazar
 */
#include "network.h"

int conectarAFileSystem(t_estado* estado) {

	int socketDestino = conectarAServidor(estado->conf->IP_FS, estado->conf->PUERTO_FS);
//	t_contenido* contenido = malloc(sizeof(t_contenido));
//	t_header* header = malloc(sizeof(t_header));
//	header->
//	enviarMensaje(socketDestino, *header, *contenido, estado->logger);
//	enviar(socketDestino, "hola", 5);
	return -1;
}
