/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

t_estado* estado;

// TODO: por cada switch del mensaje, deberia haber una funcion que la trate
int tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* LOGGER){

	switch(mensaje->tipo){

		case JOB_TO_NODO_HANDSHAKE: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			enviarNodoToJobHandshakeOk(numSocket, LOGGER);
		break;

		case FS_TO_NODO_GET_BLOQUE: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			return enviarNodoToFsGetBloque(numSocket, estado, LOGGER);
		break;

		case JOB_TO_MARTA_FILES:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		break;

		case JOB_TO_NODO_REDUCE_REQUEST:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		break;
		case JOB_TO_NODO_MAP_SCRIPT:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			break;
		case JOB_TO_NODO_MAP_REQUEST:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		break;

		case FS_TO_NODO_HANDSHAKE_OK:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		break;

		default: log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  mensaje->tipo);

	}

}

int enviarNodoToJobHandshakeOk(int socketNodo, t_log* logger){
	header_t header;


	initHeader(&header);
	header.tipo = NODO_TO_JOB_HANDSHAKE_OK;
	header.largo_mensaje = 0;
	header.cantidad_paquetes = 1;

	log_info(logger, "enviarNodoToJobHandshakeOk: sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	if (enviar_header(socketNodo, &header) != EXITO)
	{
		log_error(logger,"%s enviarNodoToJobHandshakeOk: Error al enviar header NUEVO_NIVEL\n\n", getDescription(header.tipo));
		return WARNING;
	}

	return EXITO;
}

int enviarNodoToFsGetBloque(int socketNodo, t_estado* estado, t_log* logger){

	t_nro_bloque getBloque;

	log_info(logger, "enviarNodoToFsGetBloque: sizeof(t_nro_bloque): %d \n", sizeof(t_nro_bloque));

	if (recibir_struct(socketNodo, &getBloque, sizeof(t_nro_bloque)) != EXITO)
	{
		log_error(logger,"enviarNodoToFsGetBloque: Error al recibir struct getBloque \n\n");
		return ERROR;
	}

	log_info(logger, "enviarNodoToFsGetBloque: bloque solicitado nro: %d \n", getBloque.nro_bloque);


	header_t header;

	initHeader(&header);
	header.tipo = NODO_TO_FS_GET_BLOQUE_OK;
	header.largo_mensaje = estado->conf->BLOCK_SIZE_IN_BYTES;
	header.cantidad_paquetes = 1;

	log_info(logger, "enviarNodoToFsGetBloque: Respondiendo sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	if (enviar_header(socketNodo, &header) != EXITO)
	{
		log_error(logger,"%s enviarNodoToFsGetBloque: Error al enviar header \n\n", getDescription(header.tipo));
		return ERROR;
	}

	int nroBloque = getBloque.nro_bloque;
	t_nro_bloque getBloqueResp;
	getBloqueResp.nro_bloque = nroBloque;


	log_info(logger, "Enviando GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d] [%s]\n", nroBloque, socketNodo, getDescription(header.tipo));

	if (enviar_struct(socketNodo, &getBloqueResp, sizeof(t_nro_bloque)) != EXITO)
	{
		log_error(logger, "Error enviando GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d] [%s]\n", nroBloque, socketNodo, getDescription(header.tipo));
		return ERROR;
	}

	char* mensajeMockLargo = malloc(estado->conf->BLOCK_SIZE_IN_BYTES);
	char* mensajeMockOriginal = "hola tarolas";
	memcpy(mensajeMockLargo, mensajeMockOriginal, strlen(mensajeMockOriginal + 1));

	enviar_string(socketNodo, mensajeMockLargo);

	return EXITO;
}

int ejecutarProgramaPrincipal(t_estado* estado) {

	t_log* logger = estado->logger;
	log_info(logger, "Inicializado");

	//todonodo conectar a filesystem
	bool conFileSystem = true;
	int socketServer = -1;
	if(conFileSystem == true) {

		socketServer = conectarAFileSystem(estado);
		if(socketServer > 0) {
			//todonodo escuchar respuesta filesystem
			//si la respuesta es Ok comenzar a escuchar filesystem (otra conexion?), nodos, hilos mapper y reducer

		} else {
			//si la respuesta es NO OK o hubo algun problema, loggear y salir
			log_info(logger, "No se pudo conectar al filesystem %s:%d", estado->conf->IP_FS, estado->conf->PUERTO_FS);
			return -1;
		}
	}

	if(escuchar(estado->conf->PUERTO_NODO, socketServer, (void*)tratarMensaje, NULL, logger) < 0)
	{
		log_info(logger, "No se pudo escuchar el puerto configurado");
	}


	log_info(logger, "Finalizando");

	log_info(logger, "Exit");

	return 0;
}

//header_t mensaje;
//recibir_header_simple(socketCliente, &mensaje);
//header_t* pMensaje = &mensaje;
//
//log_debug(logger, "Recibi un header tipo: %d, tamanio: %d", pMensaje->tipo, pMensaje->largo_mensaje);
//
//if(pMensaje->tipo == JOB_TO_NODO_MAP_REQUEST) {
//	t_map_request_nodo mapRequestNodo;
//	bool isDesconecto;
//	recibir_map_request_nodo(socketFD, &mapRequestNodo, &isDesconecto);
//} else {
//	log_debug(logger, "Mensaje no soportado");
//}

