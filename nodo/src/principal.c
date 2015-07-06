/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

//por cada switch del mensaje, deberia haber una funcion que la trate
int tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* logger){

	t_estado* estadoTratandoMensaje = (t_estado*) extra;

	log_info(logger, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);

	switch(mensaje->tipo){

		case JOB_TO_NODO_HANDSHAKE:
			return enviarNodoToJobHandshakeOk(numSocket, logger);
		break;

		case FS_TO_NODO_GET_BLOQUE:
			return enviarNodoToFsGetBloque(numSocket, estadoTratandoMensaje, logger);
		break;

		case JOB_TO_MARTA_FILES:

		break;

		case JOB_TO_NODO_REDUCE_REQUEST:

		break;

		case JOB_TO_NODO_MAP_SCRIPT:

		break;

		case JOB_TO_NODO_MAP_REQUEST:

		break;

		case FS_TO_NODO_HANDSHAKE_OK:

		break;

		default: log_error(logger, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  mensaje->tipo);
	}

	return WARNING;
}

header_t nuevoHeader(t_header tipo, int largo_mensaje, int cantidad_paquetes) {
	header_t header;
	initHeader(&header);
	header.tipo = tipo;
	header.largo_mensaje = largo_mensaje;
	header.cantidad_paquetes = cantidad_paquetes;
	return header;
}

int enviarNodoToJobHandshakeOk(int socketNodo, t_log* logger){

	header_t header = nuevoHeader(NODO_TO_JOB_HANDSHAKE_OK, 0, 1);

	log_info(logger, "enviarNodoToJobHandshakeOk: sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	int ret;
	if ((ret = enviar_header(socketNodo, &header)) != EXITO)
	{
		log_error(logger,"%s enviarNodoToJobHandshakeOk: Error al enviar header NUEVO_NIVEL\n\n", getDescription(header.tipo));
		return ret;
	}

	return EXITO;
}

void log_debug_header(t_log* logger, char* mensaje, header_t* header) {
	log_debug(logger, "%s: header.tipo = %s; header.largo_mensaje = %d; header.cantidad_paquetes ¡ %d;", mensaje,
			getDescription(header->tipo), header->largo_mensaje, header->cantidad_paquetes);
}

int enviarHeader_NODO_TO_FS_GET_BLOQUE_OK(int socketNodo, t_estado* estado, t_log* logger) {
	header_t header = nuevoHeader(NODO_TO_FS_GET_BLOQUE_OK, estado->conf->BLOCK_SIZE_IN_BYTES, 1);

	log_debug_header(logger, "enviarHeader_NODO_TO_FS_GET_BLOQUE_OK", &header);

	return enviar_header(socketNodo, &header);
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


	int ret;
	if ((ret = enviarHeader_NODO_TO_FS_GET_BLOQUE_OK(socketNodo, estado, logger)) != EXITO) {
		log_error(logger, "enviarNodoToFsGetBloque: Error al enviarHeader_NODO_TO_FS_GET_BLOQUE_OK");
		return ret;
	}

	int nroBloque = getBloque.nro_bloque;
	t_nro_bloque getBloqueResp;
	getBloqueResp.nro_bloque = nroBloque;


	log_info(logger, "Enviando respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);

	if (enviar_struct(socketNodo, &getBloqueResp, sizeof(t_nro_bloque)) != EXITO)
	{
		log_error(logger, "Error enviando respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	char* mensajeMockLargo = malloc(estado->conf->BLOCK_SIZE_IN_BYTES);
	char* mensajeMockOriginal = "hola tarolas";
	memcpy(mensajeMockLargo, mensajeMockOriginal, strlen(mensajeMockOriginal + 1));

	if (enviar_string(socketNodo, mensajeMockLargo) != EXITO)
	{
		log_error(logger, "Error enviando MOCK GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	return EXITO;
}

int ejecutarProgramaPrincipal(t_estado* estado) {

	t_log* logger = estado->logger;
	log_info(logger, "Inicializado");
	log_info(logger, "Usando configuracion: %s", estado->conf->TIPO_CONFIGURACION);

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

	if(escuchar(estado->conf->PUERTO_NODO, socketServer, (void*)tratarMensaje, estado, logger) < 0)
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

