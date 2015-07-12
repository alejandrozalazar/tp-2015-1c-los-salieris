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
	int resultado;
	switch(mensaje->tipo){

		case JOB_TO_NODO_HANDSHAKE:
			printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
			resultado = enviarNodoToJobHandshakeOk(numSocket, mensaje, estadoTratandoMensaje, logger);
			printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
			return resultado;
		break;

		case FS_TO_NODO_GET_BLOQUE:
			printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
			resultado = enviarNodoToFsGetBloque(numSocket, estadoTratandoMensaje, logger);
			printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
			return resultado;
		break;

		case FS_TO_NODO_SET_BLOQUE:
			printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
			resultado = recibirFSToNodoSetBloque(numSocket, estadoTratandoMensaje, mensaje, logger);
			printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
			return resultado;
		break;

		case JOB_TO_MARTA_FILES:

		break;

		case JOB_TO_NODO_REDUCE_REQUEST:

		break;

		case JOB_TO_NODO_MAP_SCRIPT:
			printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
			resultado = recibirJobToNodoMapScript(numSocket, estadoTratandoMensaje, mensaje, logger);
			printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
			return resultado;

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

int enviarNodoToJobHandshakeOk(int socketNodo, header_t* header, t_estado* estado, t_log* logger) {

	header_t headerOK = nuevoHeader(NODO_TO_JOB_HANDSHAKE_OK, 0, 1);

	log_info(logger, "enviarNodoToJobHandshakeOk: sizeof(headerOK): %d, largo mensaje: %d \n", sizeof(headerOK), headerOK.largo_mensaje);

	int ret;
	if ((ret = enviar_header(socketNodo, &headerOK)) != EXITO)
	{
		log_error(logger,"%s enviarNodoToJobHandshakeOk: Error al enviar headerOK NUEVO_NIVEL\n\n", getDescription(headerOK.tipo));
		return ret;
	}

	header_t header2;
	if((ret = recibirHeader(socketNodo, logger, &header2)) != EXITO) {
		return ret;
	}

	if((ret = tratarMensaje(socketNodo, &header2, estado, logger)) != EXITO) {
		return ret;
	}


	return EXITO;
}

void log_debug_header(t_log* logger, char* mensaje, header_t* header) {
	log_debug(logger, "%s: header.tipo = %s; header.largo_mensaje = %d; header.cantidad_paquetes ¡ %d;", mensaje,
			getDescription(header->tipo), header->largo_mensaje, header->cantidad_paquetes);
}

int enviarHeader_NODO_TO_FS_GET_BLOQUE_OK(int socketNodo, t_log* logger, int tamanio) {
	header_t header = nuevoHeader(NODO_TO_FS_GET_BLOQUE_OK, tamanio, 1);

	log_debug_header(logger, "enviarHeader_NODO_TO_FS_GET_BLOQUE_OK", &header);

	return enviar_header(socketNodo, &header);
}

int enviarNodoToFsGetBloque(int socketNodo, t_estado* estado, t_log* logger){

	t_nro_bloque headerGetBloque;

	log_info(logger, "enviarNodoToFsGetBloque: sizeof(t_nro_bloque): %d \n", sizeof(t_nro_bloque));

	if (recibir_struct(socketNodo, &headerGetBloque, sizeof(t_nro_bloque)) != EXITO)
	{
		log_error(logger,"enviarNodoToFsGetBloque: Error al recibir struct getBloque \n\n");
		return ERROR;
	}

	log_info(logger, "enviarNodoToFsGetBloque: bloque solicitado nro: %d \n", headerGetBloque.nro_bloque);


	//mockeo
	char* contenidoBloque = getBloque(headerGetBloque.nro_bloque, estado); // aca meter el mensaje posta
	size_t tamanioBloque = strlen(contenidoBloque); //no envio el \0

	//fin mockeo

	int ret;
	if ((ret = enviarHeader_NODO_TO_FS_GET_BLOQUE_OK(socketNodo, logger, tamanioBloque)) != EXITO) {
		log_error(logger, "enviarNodoToFsGetBloque: Error al enviarHeader_NODO_TO_FS_GET_BLOQUE_OK");
		return ret;
	}

	int nroBloque = headerGetBloque.nro_bloque;
	t_nro_bloque getBloqueResp;
	getBloqueResp.nro_bloque = nroBloque;


	log_info(logger, "Enviando HEADER respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);

	if (enviar_struct(socketNodo, &getBloqueResp, sizeof(t_nro_bloque)) != EXITO)
	{
		log_error(logger, "Error enviando respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio HEADER respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
	log_info(logger, "Enviando respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);

	if (enviar(socketNodo, contenidoBloque, tamanioBloque) != EXITO)
	{
		log_error(logger, "Error enviando MOCK GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
	return EXITO;
}

int recibirFSToNodoSetBloque(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {

	t_nro_bloque headerSetBloque;

	log_info(logger, "recibirNodoToFSSetBloque: sizeof(t_nro_bloque): %d %s por el socket [%d]\n",
			sizeof(t_nro_bloque), getDescription(header->tipo), socketNodo);

	if (recibir_struct(socketNodo, &headerSetBloque, sizeof(t_nro_bloque)) != EXITO) {
		log_error(logger,
				"recibirNodoToFSSetBloque: Error al recibir struct setBloque \n\n");
		return ERROR;
	}

	log_info(logger, "recibirNodoToFSSetBloque: bloque solicitado nro: %d por el socket [%d]\n",
			headerSetBloque.nro_bloque, socketNodo);

	int nroBloque = headerSetBloque.nro_bloque;

	char* contenidoBloque = malloc(header->largo_mensaje + 1); //agrego espacio para el \0
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);

	int ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje);

	if (ret != EXITO) {

		log_error(logger,
				"Error recibiendo SET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d] [%s]\n",
				nroBloque, socketNodo, getDescription(header->tipo));

		return ret;
	}

	log_info(logger,
			"recibirNodoToFSSetBloque: INICIO contenido recibido de bloque solicitado nro: %d por el socket [%d]\n",
			headerSetBloque.nro_bloque, socketNodo);
	log_info(logger,
				"%s \n",
				contenidoBloque);
	log_info(logger,
				"recibirNodoToFSSetBloque: FIN contenido recibido de bloque solicitado nro: %d \n",
				headerSetBloque.nro_bloque);

	log_info(logger,
			"recibirNodoToFSSetBloque: INICIO escribir en espacio de datos bloque nro: %d por el socket [%d]\n",
			headerSetBloque.nro_bloque, socketNodo);

	setBloque(nroBloque, estado, contenidoBloque);

	log_info(logger,
				"recibirNodoToFSSetBloque: FIN escribir en espacio de datos bloque nro: %d \n",
				headerSetBloque.nro_bloque);


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

int recibirHeader(int socketNodo, t_log* logger, header_t* headerRecibir) {


	log_info(logger, "recibirHeader: por el socket [%d]\n", socketNodo);

	if (recibir_header_simple(socketNodo, headerRecibir) != EXITO) {
		log_error(logger, "recibirHeader: Error al recibir struct  \n\n");
		return ERROR;
	}

	return EXITO;
}

int recibirJobToNodoMapScript(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {
	char* contenidoBloque = malloc(header->largo_mensaje + 1);
	//agrego espacio para el \0memset(contenidoBloque, '\0', header->largo_mensaje + 1);
	int ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje);

	log_info(logger, "recibirJobToNodoMapScript: INICIO contenido archivo tamanio: %d por el socket [%d]\n", header->largo_mensaje,
			socketNodo);
	log_info(logger, "%s \n", contenidoBloque);
	log_info(logger, "recibirJobToNodoMapScript: FIN contenido archivo tamanio: %d \n", header->largo_mensaje);
	//
	//	log_info(logger,
	//			"recibirJobToNodoMapScript: INICIO escribir en espacio de datos bloque nro: %d por el socket [%d]\n",
	//			header->largo_mensaje, socketNodo);
	//
	//	setBloque(nroBloque, estado, contenidoBloque);
	//
	//	log_info(logger,
	//				"recibirJobToNodoMapScript: FIN escribir en espacio de datos bloque nro: %d \n",
	//				header->largo_mensaje);

}
