/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

//por cada switch del mensaje, deberia haber una funcion que la trate
int tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* logger) {

	t_estado* estadoTratandoMensaje = (t_estado*) extra;

	log_info(logger, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
	int resultado;
	switch (mensaje->tipo) {

	case JOB_TO_NODO_HANDSHAKE:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_JOB_TO_NODO_HANDSHAKE(numSocket, mensaje, estadoTratandoMensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	case FS_TO_NODO_GET_BLOQUE:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = enviar_FS_TO_NODO_GET_BLOQUE(numSocket, estadoTratandoMensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	case FS_TO_NODO_SET_BLOQUE:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_FS_TO_NODO_SET_BLOQUE(numSocket, estadoTratandoMensaje, mensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	case JOB_TO_MARTA_FILES:

		break;

	case JOB_TO_NODO_REDUCE_REQUEST:

		break;

	case JOB_TO_NODO_MAP_SCRIPT:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_JOB_TO_NODO_MAP_SCRIPT(numSocket, estadoTratandoMensaje, mensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;

		break;

	case JOB_TO_NODO_MAP_REQUEST:

		break;

	case FS_TO_NODO_HANDSHAKE_OK:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_NODO_TO_FS_GET_BLOQUE_OK(numSocket, mensaje, estadoTratandoMensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	default:
		log_error(logger, "ERROR mensaje NO RECONOCIDO (%d) !!\n", mensaje->tipo);
	}

	return WARNING;
}

int recibir_NODO_TO_FS_GET_BLOQUE_OK(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {

	log_info(logger, "Conexion al FS exitosa: por el socket [%d]\n", socketNodo);

	return EXITO;
}

int recibir_JOB_TO_NODO_HANDSHAKE(int socketNodo, header_t* header, t_estado* estado, t_log* logger) {

	int ret;
	if ((ret = enviarHeader(socketNodo, logger, 0, NODO_TO_JOB_HANDSHAKE_OK)) != EXITO) {
		log_error(logger, "enviarNodoToJobHandshakeOk: Error al enviar %s\n", getDescription(NODO_TO_JOB_HANDSHAKE_OK));
		return ret;
	}

	header_t header2;
	if ((ret = recibirHeader(socketNodo, logger, &header2)) != EXITO) {
		log_error(logger, "Error al recibir header despues del handshake\n");
		return ret;
	}

	if ((ret = tratarMensaje(socketNodo, &header2, estado, logger)) != EXITO) {
		return ret;
	}

	return EXITO;
}

int enviar_FS_TO_NODO_GET_BLOQUE(int socketNodo, t_estado* estado, t_log* logger) {

	t_nro_bloque headerGetBloque;

	int ret;
	if ((ret = recibirNroBloque(socketNodo, logger, &headerGetBloque)) != EXITO) {
		return ret;
	}

	log_info(logger, "enviar_FS_TO_NODO_GET_BLOQUE: bloque solicitado nro: %d \n", headerGetBloque.nro_bloque);

	char* contenidoBloque = getBloque(headerGetBloque.nro_bloque, estado); // aca meter el mensaje posta
	size_t tamanioBloque = strlen(contenidoBloque); //no envio el \0

	if ((ret = enviarHeader(socketNodo, logger, tamanioBloque, NODO_TO_FS_GET_BLOQUE_OK)) != EXITO) {
		log_error(logger, "enviar_FS_TO_NODO_GET_BLOQUE: Error al enviarHeader_NODO_TO_FS_GET_BLOQUE_OK");
		return ret;
	}

	int nroBloque = headerGetBloque.nro_bloque;

	if ((ret = enviarNroBloque(nroBloque, socketNodo, logger)) != EXITO) {
		return ret;
	}

	if (enviar(socketNodo, contenidoBloque, tamanioBloque) != EXITO) {
		log_error(logger, "Error enviando respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
	return EXITO;
}

int recibir_FS_TO_NODO_SET_BLOQUE(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {

	int ret;
	t_nro_bloque headerSetBloque;

	if ((ret = recibirNroBloque(socketNodo, logger, &headerSetBloque)) != EXITO) {
		return ret;
	}

	log_info(logger, "recibir_FS_TO_NODO_SET_BLOQUE: bloque solicitado nro: %d por el socket [%d]\n", headerSetBloque.nro_bloque, socketNodo);

	int nroBloque = headerSetBloque.nro_bloque;

//	char contenidoBloque;
//
//	if ((ret = recibirContenidoBloque(socketNodo, &contenidoBloque, header, logger)) != EXITO) {
//		return ret;
//	}

	char* contenidoBloque = malloc(header->largo_mensaje + 1);
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);
	ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje);

	log_info(logger, "%s \n", contenidoBloque);

	//escribimos el bloque
	escribirBloque(nroBloque, estado, contenidoBloque);

	return EXITO;
}

int recibirContenidoBloque(int socketNodo, char *contenidoBloque, header_t* header, t_log* logger) {

	contenidoBloque = malloc(header->largo_mensaje + 1); //agrego espacio para el \0
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);

	int ret;
	if ((ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje)) != EXITO) {
		log_error(logger, "Error recibiendo bloque por el socket [%d]\n", socketNodo);
	}
	return ret;

}


int ejecutarProgramaPrincipal(t_estado* estado) {

	t_log* logger = estado->logger;
	log_info(logger, "Inicializado");
	log_info(logger, "Usando configuracion: %s", estado->conf->TIPO_CONFIGURACION);

	//todonodo conectar a filesystem
	bool conFileSystem = true;
	int socketServer = -1;
	if (conFileSystem == true) {

		socketServer = conectarAFileSystem(estado);
		if (socketServer > 0) {
			//todonodo escuchar respuesta filesystem
			//si la respuesta es Ok comenzar a escuchar filesystem (otra conexion?), nodos, hilos mapper y reducer

		} else {
			//si la respuesta es NO OK o hubo algun problema, loggear y salir
			log_info(logger, "No se pudo conectar al filesystem %s:%d", estado->conf->IP_FS, estado->conf->PUERTO_FS);
			return -1;
		}
	}

	if (escuchar(estado->conf->PUERTO_NODO, socketServer, (void*) tratarMensaje, estado, logger) < 0) {
		log_info(logger, "No se pudo escuchar el puerto configurado");
	}

	log_info(logger, "Finalizando");

	log_info(logger, "Exit");

	return 0;
}

int recibir_JOB_TO_NODO_MAP_SCRIPT(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {
	char* contenidoBloque = malloc(header->largo_mensaje + 1);
	//agrego espacio para el \0
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);
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
	return ret;
}

void log_debug_header(t_log* logger, char* mensaje, header_t* header) {
	log_debug(logger, "%s: header.tipo = %s; header.largo_mensaje = %d; header.cantidad_paquetes ¡ %d;", mensaje,
			getDescription(header->tipo), header->largo_mensaje, header->cantidad_paquetes);
}


header_t nuevoHeader(t_header tipo, int largo_mensaje, int cantidad_paquetes) {
	header_t header;
	initHeader(&header);
	header.tipo = tipo;
	header.largo_mensaje = largo_mensaje;
	header.cantidad_paquetes = cantidad_paquetes;
	return header;
}


int recibirHeader(int socketNodo, t_log* logger, header_t* headerRecibir) {

	int res = recibir_header_simple(socketNodo, headerRecibir);

	log_debug_header(logger, "recibirHeader", headerRecibir);

	return res;
}

int enviarHeader(int socketNodo, t_log* logger, int tamanio, t_header tipo) {
	header_t header = nuevoHeader(tipo, tamanio, 1);

	log_debug_header(logger, "enviarHeader", &header);

	return enviar_header(socketNodo, &header);
}

int recibirNroBloque(int socketNodo, t_log* logger, t_nro_bloque* headerGetBloque) {

	int ret;
	log_debug(logger, "recibirNroBloque por el socket [%d]\n", socketNodo);
	if ((ret = recibir_struct(socketNodo, headerGetBloque, sizeof(t_nro_bloque))) != EXITO) {
		log_error(logger, "Error al recibir struct t_nro_bloque \n");
	}
	return ret;
}


int enviarNroBloque(int socketNodo, int nroBloque, t_log* logger) {
	t_nro_bloque getBloqueResp;
	getBloqueResp.nro_bloque = nroBloque;

	int ret;
	log_debug(logger, "enviarNroBloque por el socket [%d]\n", socketNodo);
	if ((ret = enviar_struct(socketNodo, &getBloqueResp, sizeof(t_nro_bloque))) != EXITO) {
		log_error(logger, "Error al enviar struct t_nro_bloque \n");
	}
	return ret;
}
