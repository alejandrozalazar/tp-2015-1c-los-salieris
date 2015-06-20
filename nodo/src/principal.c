/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

t_estado* estado;

// TODO: por cada switch del mensaje, deberia haber una funcion que la trate
void tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* LOGGER){

	switch(mensaje->tipo){

//		case JOB_TO_MARTA_HANDSHAKE: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
//		enviarSerializado(LOGGER, numSocket, false, MARTA_TO_JOB, 0, NULL);
//		break; //HACK no encontraba el case, no se si era problema mio o falto commitear el commons

		case JOB_TO_MARTA_FILES: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
//		procesarArchivos(numSocket, mensaje->contenido);
		break;

		case JOB_TO_NODO_REDUCE_REQUEST: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
//		pingback(numSocket);
		break;

		case JOB_TO_NODO_HANDSHAKE: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			enviarNodoToJobHandshakeOk(numSocket, LOGGER);
		break;

//		case NODO_TO_FS_HANDSHAKE: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
//		enviarSerializado(LOGGER, numSocket, false, FS_TO_NODO_HANDSHAKE_OK, 0, NULL);
//		break;

//		case FS_TO_NODO_HANDSHAKE_OK: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			//ALE agregar a alguna lista
//		break;

		case JOB_TO_NODO_MAP_SCRIPT:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			break;
		case JOB_TO_NODO_MAP_REQUEST:
			log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
//		pingback(numSocket);

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

int ejecutarProgramaPrincipal(t_estado* estado) {

	t_log* logger = estado->logger;
	log_info(logger, "Inicializado");

	//todonodo conectar a filesystem
//	if(conectarAFileSystem(estado) > 0) {
//		//todonodo escuchar respuesta filesystem
//		//si la respuesta es Ok comenzar a escuchar filesystem (otra conexion?), nodos, hilos mapper y reducer
//
//	} else {
//		//si la respuesta es NO OK o hubo algun problema, loggear y salir
//		log_info(logger, "No se pudo conectar al filesystem %s:%d", estado->conf->IP_FS, estado->conf->PUERTO_FS);
//		return -1;
//	}

	if(escuchar(estado->conf->PUERTO_NODO, 0, (void*)tratarMensaje, NULL, logger) < 0)
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

