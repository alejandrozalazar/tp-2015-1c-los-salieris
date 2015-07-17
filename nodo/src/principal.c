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
	case NODO_TO_NODO_GET_BLOQUE:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = enviar_FS_o_NODO_TO_NODO_GET_BLOQUE(numSocket, estadoTratandoMensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	case FS_TO_NODO_SET_BLOQUE:
	case NODO_TO_NODO_SET_BLOQUE:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_FS_o_NODO_TO_NODO_SET_BLOQUE(numSocket, estadoTratandoMensaje, mensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	case JOB_TO_MARTA_FILES:

		break;

	case JOB_TO_NODO_REDUCE_REQUEST:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_JOB_TO_NODO_REDUCE_REQUEST(numSocket, mensaje, estadoTratandoMensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
		break;

	case JOB_TO_NODO_MAP_SCRIPT:
	case JOB_TO_NODO_REDUCE_SCRIPT:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT(numSocket, estadoTratandoMensaje, mensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;

		break;

	case JOB_TO_NODO_MAP_REQUEST:
		printf("==================== INICIO %s ==================\n", getDescription(mensaje->tipo));
		resultado = recibir_JOB_TO_NODO_MAP_REQUEST(numSocket, mensaje, estadoTratandoMensaje, logger);
		printf("==================== FIN %s ==================\n", getDescription(mensaje->tipo));
		return resultado;
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

int enviar_FS_o_NODO_TO_NODO_GET_BLOQUE(int socketNodo, t_estado* estado, t_log* logger) {

	t_nro_bloque headerGetBloque;

	int ret;
	if ((ret = recibirNroBloque(socketNodo, logger, &headerGetBloque)) != EXITO) {
		return ret;
	}

	log_info(logger, "enviar_FS_o_NODO_TO_NODO_GET_BLOQUE: bloque solicitado nro: %d \n", headerGetBloque.nro_bloque);

	char* contenidoBloque = getBloque(headerGetBloque.nro_bloque, estado); // aca meter el mensaje posta

	if(string_length(contenidoBloque) == 0) {
		t_header respuestaProblema = FS_TO_NODO_GET_BLOQUE? NODO_TO_FS_GET_BLOQUE_KO:NODO_TO_NODO_GET_BLOQUE_KO;
		if ((ret = enviarHeader(socketNodo, logger, 0, respuestaProblema)) != EXITO) {
			log_error(logger, "enviar_FS_o_NODO_TO_NODO_GET_BLOQUE: Error al enviar respuesta de error %s", getDescription(respuestaProblema));
			return ret;
		}
		return EXITO;
	}

	size_t tamanioBloque = strlen(contenidoBloque); //no envio el \0

	t_header respuesta = FS_TO_NODO_GET_BLOQUE? NODO_TO_FS_GET_BLOQUE_OK:NODO_TO_NODO_GET_BLOQUE_OK;
	if ((ret = enviarHeader(socketNodo, logger, tamanioBloque, respuesta)) != EXITO) {
		log_error(logger, "enviar_FS_o_NODO_TO_NODO_GET_BLOQUE: Error al enviarHeader %s", getDescription(respuesta));
		return ret;
	}

	int nroBloque = headerGetBloque.nro_bloque;

	if ((ret = enviarNroBloque(socketNodo, nroBloque, logger)) != EXITO) {
		return ret;
	}

	if (enviar(socketNodo, contenidoBloque, tamanioBloque) != EXITO) {
		log_error(logger, "Error enviando respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio respuesta GET_BLOQUE[nro_bloque: %d] al filesystem por el socket [%d]\n", nroBloque, socketNodo);
	return EXITO;
}

int recibir_FS_o_NODO_TO_NODO_SET_BLOQUE(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {

	int ret;
	t_nro_bloque headerSetBloque;

	if ((ret = recibirNroBloque(socketNodo, logger, &headerSetBloque)) != EXITO) {
		return ret;
	}

	log_info(logger, "recibir_FS_o_NODO_TO_NODO_SET_BLOQUE: bloque solicitado nro: %d por el socket [%d]\n", headerSetBloque.nro_bloque, socketNodo);

	int nroBloque = headerSetBloque.nro_bloque;

//	char contenidoBloque;
//
//	if ((ret = recibirContenidoBloque(socketNodo, &contenidoBloque, header, logger)) != EXITO) {
//		return ret;
//	}

	char* contenidoBloque = malloc(header->largo_mensaje + 1);
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);

	if((ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje)) != EXITO) {
		log_error(logger, "recibir_FS_o_NODO_TO_NODO_SET_BLOQUE: se produjo un error al recibir el contenido del bloque %d por el socket [%d]\n", headerSetBloque.nro_bloque, socketNodo);
		return ret;
	}

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

int recibir_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT(int socketNodo, t_estado* estado, header_t* header, t_log* logger) {
	char* contenidoBloque = malloc(header->largo_mensaje + 1);
	//agrego espacio para el \0
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);
	int ret;

	if((ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje)) != EXITO) {
		log_error(logger, "recibir_JOB_TO_NODO_MAP_SCRIPT no se pudo recibir el contenido del bloque");
		return ret;
	}

	log_info(logger, "recibir_JOB_TO_NODO_MAP_SCRIPT: INICIO contenido archivo tamanio: %d por el socket [%d]\n", header->largo_mensaje,
			socketNodo);
	log_info(logger, "%s \n", contenidoBloque);
	log_info(logger, "recibir_JOB_TO_NODO_MAP_SCRIPT: FIN contenido archivo tamanio: %d \n", header->largo_mensaje);

	char* mascara;
	if(header->tipo == JOB_TO_NODO_MAP_SCRIPT) {
		mascara = "temp_%d_map_script";
	} else if(header->tipo == JOB_TO_NODO_REDUCE_SCRIPT) {
		mascara = "temp_%d_reduce_script";
	} else {
		log_error(logger, "recibir_JOB_TO_NODO_MAP_SCRIPT no se pudo reconocer el mensaje inicial");
		return ERROR;
	}

	char* nombreArchivo = setFileContent(contenidoBloque, estado, mascara);
	int tamanioNombreArchivo = strlen(nombreArchivo);


	t_header tipoRespuesta;
	if(header->tipo == JOB_TO_NODO_MAP_SCRIPT) {

		tipoRespuesta = NODO_TO_JOB_MAP_SCRIPT_OK;
	} else if(header->tipo == JOB_TO_NODO_REDUCE_SCRIPT) {
		tipoRespuesta = NODO_TO_JOB_REDUCE_SCRIPT_OK;
	} else {
		log_error(logger, "recibir_JOB_TO_NODO_MAP_SCRIPT no se pudo reconocer el mensaje inicial");
		return ERROR;
	}

	if((ret = enviarHeader(socketNodo, logger, tamanioNombreArchivo, tipoRespuesta)) != EXITO) {
		log_error(logger, "recibir_JOB_TO_NODO_MAP_SCRIPT No se pudo enviar NODO_TO_JOB_MAP_SCRIPT_OK");
		return ret;
	}

	if((ret = enviar(socketNodo, nombreArchivo, tamanioNombreArchivo)) != EXITO) {
		log_error(logger, "No se pudo enviar el nombre de archivo despues del header");
		return ret;
	}
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
	log_debug(logger, "%s: header.tipo = %s; header.largo_mensaje = %d; header.cantidad_paquetes = %d;", mensaje,
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


int recibir_JOB_TO_NODO_MAP_REQUEST(int socketNodo, header_t* header, t_estado* estado, t_log* logger) {

	char* contenidoBloque = malloc(header->largo_mensaje + 1);
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);

	int ret;
	if((ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje)) != EXITO) {
		log_error(logger, "recibir_JOB_TO_NODO_MAP_REQUEST: se produjo un error al recibir el contenido del request por el socket [%d]\n", socketNodo);
		return ret;
	}

	char nombreArchivoArray[256];
	int nroBloque;
	sscanf (contenidoBloque,"[%d,%s]", &nroBloque, nombreArchivoArray);
	log_debug(logger, "Bloque: %d\n", nroBloque);
	char* nombreArchivo = string_substring_until(&nombreArchivoArray, strlen(nombreArchivoArray) -1); //todonodo
	log_debug(logger, "Archivo: %s\n", nombreArchivo);

	char* nombreArchivoResultado;
	if(header->tipo == JOB_TO_NODO_MAP_REQUEST) {
		nombreArchivoResultado = ejecutarMapeo(nroBloque, nombreArchivo, estado);
	} else if(header->tipo == JOB_TO_NODO_REDUCE_REQUEST) {
		nombreArchivoResultado = ejecutarReduce(nroBloque, nombreArchivo, estado);
	} else {
		log_error(logger, "Tipo de request no reconocido %s\n", getDescription(header->tipo));
		return ERROR;
	}

	log_debug(logger, "Archivo: %s\n", nombreArchivoResultado);

	t_header headerResultado;
	if(header->tipo == JOB_TO_NODO_MAP_REQUEST) {
		headerResultado = NODO_TO_JOB_MAP_OK;
	} else if(header->tipo == JOB_TO_NODO_REDUCE_REQUEST) {
		headerResultado = NODO_TO_JOB_REDUCE_OK;
	}
	int tamanioNombreArchivoResultado = strlen(nombreArchivoResultado);
	if((ret = enviarHeader(socketNodo, logger, tamanioNombreArchivoResultado, headerResultado)) != EXITO) {
		log_error(logger, "No se pudo enviar la respuesta NODO_TO_JOB_MAP_OK por el socket %d\n", socketNodo);
		return ret;
	}

	if((ret = enviar(socketNodo, nombreArchivoResultado, tamanioNombreArchivoResultado)) != EXITO) {
		log_error(logger, "No se pudo enviar el nombre de archivo despues del header");
		return ret;
	}

	return EXITO;
}


int recibir_JOB_TO_NODO_REDUCE_REQUEST(int socketNodo, header_t* header, t_estado* estado, t_log* logger) {

//	char* contenidoBloque = malloc(header->largo_mensaje + 1);
//	memset(contenidoBloque, '\0', header->largo_mensaje + 1);
//
	int ret;
//	if((ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje)) != EXITO) {
//		log_error(logger, "recibir_JOB_TO_NODO_REDUCE_REQUEST: se produjo un error al recibir el contenido del request por el socket [%d]\n", socketNodo);
//		return ret;
//	}
	log_debug(logger, "Comienza recepcion reduce request");

	t_reduce_request reduceRequest;
	if((ret = recibir_struct(socketNodo, &reduceRequest, sizeof(t_reduce_request))) != EXITO) {
		log_error(logger, "Error recibiendo t_reduce_request por el socket %d", socketNodo);
		return ret;
	}

	log_debug(logger, "Se recibio header");

	char* nombreArchivoScript = reduceRequest.archivoScript;
	int cantTipoNodoALeer = reduceRequest.cantTipoNodo;
	int var;
	t_list* listaArchivoNodo = list_create();
	for (var = 0; var < cantTipoNodoALeer; var++) {

		t_archivo_nodo archivoNodo;
		if((ret = recibir_struct(socketNodo, &archivoNodo, sizeof(t_archivo_nodo))) != EXITO) {
			log_error(logger, "Error recibiendo archivo nodo por el socket %d", socketNodo);
			return ret;
		}

		log_debug(logger, "Se recibio nodo %d", var);

		//agregar a una lista
		list_add(listaArchivoNodo, &archivoNodo);
	}

	log_debug(logger, "Pre ejecutar");
	char* nombreArchivoResultado;
	if(list_size(listaArchivoNodo) == 1) {
		t_archivo_nodo* currentArchivoNodo = (t_archivo_nodo*)list_get(listaArchivoNodo, 0);
		log_debug(logger, "Si el size es 1 ejecutamos");
		nombreArchivoResultado = ejecutarReduce(0, nombreArchivoScript, estado, currentArchivoNodo);
	}
	log_debug(logger, "Post ejecutar");

	int tamanioNombreArchivoResultado = strlen(nombreArchivoResultado);
	if((ret = enviarHeader(socketNodo, logger, tamanioNombreArchivoResultado, NODO_TO_JOB_REDUCE_OK)) != EXITO) {
		log_error(logger, "No se pudo enviar la respuesta NODO_TO_JOB_REDUCE_OK por el socket %d\n", socketNodo);
		return ret;
	}

	if((ret = enviar(socketNodo, nombreArchivoResultado, tamanioNombreArchivoResultado)) != EXITO) {
		log_error(logger, "No se pudo enviar el nombre de archivo despues del header");
		return ret;
	}
	return EXITO;
}
