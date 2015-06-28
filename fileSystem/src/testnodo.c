/*
 * testnodo.c
 *
 *  Created on: Jun 28, 2015
 *      Author: Alejandro Zalazar
 */
#include "testnodo.h"

t_log* logger;
unsigned short iCantNodosMinima;
unsigned short puertoEscucha;

int tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* LOGGER) {

	switch (mensaje->tipo) {
	case NODO_TO_FS_HANDSHAKE:
		log_info(logger, "Mensaje recibido: [%s] del socket [%d]",
				getDescription(mensaje->tipo), numSocket);
		enviarFSToNodoHandshakeOk(numSocket, logger);
		sleep(10);
		log_info(logger, "YA ESPERE");

		int nroBloque = 2;
		return enviarFSToNodoGetBloque(numSocket, logger, nroBloque);
		break;

	case NODO_TO_FS_GET_BLOQUE_OK:
		log_info(logger, "Mensaje recibido: [%s] del socket [%d]",
				getDescription(mensaje->tipo), numSocket);
		return recibirNodoToFSGetBloque(numSocket, mensaje, logger);
		break;

	default:
		log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",
				mensaje->tipo);

	}
	return 0;
}

int enviarFSToNodoHandshakeOk(int socketNodo, t_log* logger) {
	header_t header;

	initHeader(&header);
	header.tipo = FS_TO_NODO_HANDSHAKE_OK;
	header.cantidad_paquetes = 0;

	log_info(logger, "Se envia handshake OK al Nodo por el socket [%d]\n",
			socketNodo);

	if (enviar_header(socketNodo, &header) != EXITO) {
		log_error(logger,
				"Error al enviar el handshake OK al Nodo por el socket [%d]\n",
				socketNodo);
		return WARNING;
	}

	return EXITO;
}

int enviarFSToNodoGetBloque(int socketNodo, t_log* logger, int nroBloque) {

	header_t header;

	initHeader(&header);
	header.tipo = FS_TO_NODO_GET_BLOQUE;
	header.cantidad_paquetes = 0;

	log_info(logger,
			"Enviando header GET_BLOQUE al nodo por el socket [%d] [%s]\n",
			socketNodo, getDescription(header.tipo));

	if (enviar_header(socketNodo, &header) != EXITO) {
		log_error(logger,
				"Error enviando GET_BLOQUE al nodo por el socket [%d] [%s]\n",
				socketNodo, getDescription(header.tipo));
		return ERROR;
	}

	t_nro_bloque getBloque;
	getBloque.nro_bloque = nroBloque;

	log_info(logger,
			"Enviando GET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d] [%s]\n",
			nroBloque, socketNodo, getDescription(header.tipo));

	if (enviar_struct(socketNodo, &getBloque, sizeof(t_nro_bloque)) != EXITO) {
		log_error(logger,
				"Error enviando GET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d] [%s]\n",
				nroBloque, socketNodo, getDescription(header.tipo));
		return ERROR;
	}

	return EXITO;
}

int recibirNodoToFSGetBloque(int socketNodo, header_t* header, t_log* logger) {

	t_nro_bloque getBloque;

	log_info(logger, "recibirNodoToFSGetBloque: sizeof(t_nro_bloque): %d \n",
			sizeof(t_nro_bloque));

	if (recibir_struct(socketNodo, &getBloque, sizeof(t_nro_bloque)) != EXITO) {
		log_error(logger,
				"recibirNodoToFSGetBloque: Error al recibir struct getBloque \n\n");
		return ERROR;
	}

	log_info(logger, "recibirNodoToFSGetBloque: bloque solicitado nro: %d \n",
			getBloque.nro_bloque);

	int nroBloque = getBloque.nro_bloque;

	char* contenidoBloque = malloc(header->largo_mensaje);

	int ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje);

	if (ret != EXITO) {

		log_error(logger,
				"Error recibiendo GET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d] [%s]\n",
				nroBloque, socketNodo, getDescription(header->tipo));

		return ret;
	}

	log_info(logger,
			"recibirNodoToFSGetBloque: contenido recibido de bloque solicitado nro: %d \n",
			getBloque.nro_bloque);

	return EXITO;
}

int mainAlternativo(int argc, char *argv[]) {

	logger = log_create("logFileSystem.log", "FileSystem", true,
			LOG_LEVEL_DEBUG);

	char* pathArchiConf =
			"/home/utnso/dev/tp-2015-1c-los-salieris/fileSystem/src/testnodoconf";

	t_config* archivoConfig = config_create(pathArchiConf);

	iCantNodosMinima = config_get_int_value(archivoConfig, "CANT_MINIMA_NODOS");

	puertoEscucha = config_get_int_value(archivoConfig, "PUERTO_LISTEN");

	if (escuchar(puertoEscucha, 0, (void*) tratarMensaje, NULL, logger) < 0) {
		log_info(logger, "No se pudo escuchar el puerto configurado");
	}

	return EXIT_SUCCESS;
}
