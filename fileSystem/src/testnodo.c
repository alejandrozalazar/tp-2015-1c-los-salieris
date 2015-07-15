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


header_t nuevoHeader(t_header tipo, int largo_mensaje, int cantidad_paquetes) {
	header_t header;
	initHeader(&header);
	header.tipo = tipo;
	header.largo_mensaje = largo_mensaje;
	header.cantidad_paquetes = cantidad_paquetes;
	return header;
}

void log_debug_header(t_log* logger, char* mensaje, header_t* header) {
	log_debug(logger, "%s: header.tipo = %s; header.largo_mensaje = %d; header.cantidad_paquetes ¡ %d;", mensaje,
			getDescription(header->tipo), header->largo_mensaje, header->cantidad_paquetes);
}

int enviarHeader(int socketNodo, t_log* logger, int tamanio, t_header tipo) {
	header_t header = nuevoHeader(tipo, tamanio, 1);

	log_debug_header(logger, "enviarHeader_%s", &header);

	return enviar_header(socketNodo, &header);
}

int recibirHeader(int socketNodo, t_log* logger, header_t* headerRecibir) {

	int res = recibir_header_simple(socketNodo, headerRecibir);

	log_debug_header(logger, "recibirHeader", headerRecibir);

	return res;
}

int tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* LOGGER) {

	switch (mensaje->tipo) {
	case NODO_TO_FS_HANDSHAKE:
		log_info(logger, "Mensaje recibido: [%s] del socket [%d]",
				getDescription(mensaje->tipo), numSocket);
		enviarFSToNodoHandshakeOk(numSocket, logger);

		sleep(5);
		log_info(logger, "Espero el handshake para simular el nodo");

		bool getSetBloqueFsANodo = true;
		bool getSetBloqueNodoANodo = false;
		bool interfazNodo = true;

		if(getSetBloqueFsANodo == true)
		{
			t_header tipoGetBloque = FS_TO_NODO_GET_BLOQUE;
			t_header tipoSetBloque = FS_TO_NODO_SET_BLOQUE;

			int nroBloqueFalla = 2;
			int nroSetGetBloque = 3;

			printf("================================ INI get bloque %d =========================\n", nroBloqueFalla);

			int resultadoSetBloque1 = enviarFSoNodoToNodoGetBloque(numSocket, logger, nroBloqueFalla, tipoGetBloque);
			if(resultadoSetBloque1 != EXITO) {
				return ERROR;
			}

			printf("================================ FIN get bloque %d =========================\n", nroBloqueFalla);

			printf("================================ INI set bloque %d =========================\n", nroSetGetBloque);

			char* setBloqueContent = "linea=fs\nlinea3=hola\nlinea2=hola2\nlinea5=hola tarolas";
			int resultadoSetBloque2 = enviarFsToNodoSetBloque(numSocket, logger, nroSetGetBloque, setBloqueContent, tipoSetBloque);

			if(resultadoSetBloque2 != EXITO) {
				return ERROR;
			}
			printf("================================ FIN set bloque %d =========================\n", nroSetGetBloque);

			printf("================================ INI get bloque %d =========================\n", nroSetGetBloque);
			int resultadoGetBloque2 = enviarFSoNodoToNodoGetBloque(numSocket, logger, nroSetGetBloque, tipoGetBloque);
			if(resultadoGetBloque2 != EXITO) {
				return ERROR;
			}
			printf("================================ FIN get bloque %d =========================\n", nroSetGetBloque);

		}

		if(getSetBloqueNodoANodo == true)
		{
			t_header tipoGetBloque = NODO_TO_NODO_GET_BLOQUE;
			t_header tipoSetBloque = NODO_TO_NODO_SET_BLOQUE;

			int nroBloqueFalla = 4;
			int nroSetGetBloque = 5;

			printf("================================ INI get bloque %d =========================\n", nroBloqueFalla);

			int resultadoSetBloque1 = enviarFSoNodoToNodoGetBloque(numSocket, logger, nroBloqueFalla, tipoGetBloque);
			if(resultadoSetBloque1 != EXITO) {
				return ERROR;
			}

			printf("================================ FIN get bloque %d =========================\n", nroBloqueFalla);

			printf("================================ INI set bloque %d =========================\n", nroSetGetBloque);

			char* setBloqueContent = "linea=nodo\nlinea3=hola\nlinea2=hola2\nlinea5=hola tarolas";
			int resultadoSetBloque2 = enviarFsToNodoSetBloque(numSocket, logger, nroSetGetBloque, setBloqueContent, tipoSetBloque);

			if(resultadoSetBloque2 != EXITO) {
				return ERROR;
			}
			printf("================================ FIN set bloque %d =========================\n", nroSetGetBloque);

			printf("================================ INI get bloque %d =========================\n", nroSetGetBloque);
			int resultadoGetBloque2 = enviarFSoNodoToNodoGetBloque(numSocket, logger, nroSetGetBloque, tipoGetBloque);
			if(resultadoGetBloque2 != EXITO) {
				return ERROR;
			}
			printf("================================ FIN get bloque %d =========================\n", nroSetGetBloque);

		}

		if(interfazNodo == true)
		{
			int nroSetGetBloque = 5;

			int resultado1= enviarJobToNodoHandshake(numSocket, logger);
			if(resultado1 != EXITO) {
				return resultado1;
			}

			int resultado2= recibirNodoToJobHandshakeOk(numSocket, logger);
			if(resultado2 != EXITO) {
				return resultado2;
			}

			int resultado3= enviar_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT(numSocket, nroSetGetBloque, logger, JOB_TO_NODO_MAP_SCRIPT);
			if(resultado3 != EXITO) {
				return resultado3;
			}

			int resultado5= enviar_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT(numSocket, nroSetGetBloque, logger, JOB_TO_NODO_REDUCE_SCRIPT);
			if(resultado5 != EXITO) {
				return resultado5;
			}

		}

		return EXITO;
		break;

	case NODO_TO_FS_GET_BLOQUE_OK:
		log_info(logger, "Mensaje recibido: [%s] del socket [%d]",
				getDescription(mensaje->tipo), numSocket);
		return recibirNodoToNodoOFSGetBloque(numSocket, mensaje, logger);
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

	int ret;
	if ((ret = enviar_header(socketNodo, &header)) != EXITO) {
		log_error(logger,
				"Error al enviar el handshake OK al Nodo por el socket [%d]\n",
				socketNodo);
		return ret;
	}

	return EXITO;
}

int enviarFSoNodoToNodoGetBloque(int socketNodo, t_log* logger, int nroBloque, t_header tipo) {

	header_t header;

	initHeader(&header);
	header.tipo = tipo;
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

	header_t headerRecibir;
	recibir_header_simple(socketNodo, &headerRecibir);

	if(headerRecibir.tipo == NODO_TO_FS_GET_BLOQUE_OK || headerRecibir.tipo == NODO_TO_NODO_GET_BLOQUE_OK) {
		return recibirNodoToNodoOFSGetBloque(socketNodo, &headerRecibir, logger);
	} else {
		log_error(logger, "El nodo informa que se produjo un error\n");
		return EXITO;
	}
}

int recibirNodoToNodoOFSGetBloque(int socketNodo, header_t* header, t_log* logger) {

	t_nro_bloque getBloque;

	log_info(logger, "recibirNodoToNodoOFSGetBloque: sizeof(t_nro_bloque): %d %s por el socket [%d]\n",
			sizeof(t_nro_bloque), getDescription(header->tipo), socketNodo);

	if (recibir_struct(socketNodo, &getBloque, sizeof(t_nro_bloque)) != EXITO) {
		log_error(logger,
				"recibirNodoToNodoOFSGetBloque: Error al recibir struct getBloque \n\n");
		return ERROR;
	}

	log_info(logger, "recibirNodoToNodoOFSGetBloque: bloque solicitado nro: %d por el socket [%d]\n",
			getBloque.nro_bloque, socketNodo);

	int nroBloque = getBloque.nro_bloque;

	char* contenidoBloque = malloc(header->largo_mensaje + 1); //agrego espacio para el \0
	memset(contenidoBloque, '\0', header->largo_mensaje + 1);

	int ret = recibir(socketNodo, contenidoBloque, header->largo_mensaje);

	if (ret != EXITO) {

		log_error(logger,
				"Error recibiendo GET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d] [%s]\n",
				nroBloque, socketNodo, getDescription(header->tipo));

		return ret;
	}

	log_info(logger,
			"recibirNodoToNodoOFSGetBloque: INICIO contenido recibido de bloque solicitado nro: %d por el socket [%d]\n",
			getBloque.nro_bloque, socketNodo);
	log_info(logger,
				"%s \n",
				contenidoBloque);
	log_info(logger,
				"recibirNodoToNodoOFSGetBloque: FIN contenido recibido de bloque solicitado nro: %d \n",
				getBloque.nro_bloque);

	return EXITO;
}

int enviarFsToNodoSetBloque(int socketNodo, t_log* logger, int nroBloque, char* contenidoBloque, t_header tipo) {
	size_t tamanioBloque = strlen(contenidoBloque); //no envio el \0

	header_t header;

	initHeader(&header);
	header.tipo = tipo;
	header.cantidad_paquetes = 0;
	header.largo_mensaje = tamanioBloque;

	log_info(logger,
			"Enviando header SET_BLOQUE al nodo por el socket [%d] [%s]\n",
			socketNodo, getDescription(header.tipo));

	if (enviar_header(socketNodo, &header) != EXITO) {
		log_error(logger,
				"Error enviando SET_BLOQUE al nodo por el socket [%d] [%s]\n",
				socketNodo, getDescription(header.tipo));
		return ERROR;
	}

	t_nro_bloque setBloque;
	setBloque.nro_bloque = nroBloque;

	log_info(logger, "enviarNodoToFsSetBloque: sizeof(t_nro_bloque): %d \n", sizeof(t_nro_bloque));

	if (enviar_struct(socketNodo, &setBloque, sizeof(t_nro_bloque)) != EXITO)
	{
		log_error(logger, "Error enviando header SET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	//mockeo
//	char* contenidoBloque = "hola tarolas"; // aca meter el mensaje posta

	//fin mockeo

	log_info(logger, "Enviando contenido SET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d]\n", nroBloque, socketNodo);

	if (enviar(socketNodo, contenidoBloque, tamanioBloque) != EXITO)
	{
		log_error(logger, "Error enviando contenido SET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio contenido SET_BLOQUE[nro_bloque: %d] al nodo por el socket [%d]\n", nroBloque, socketNodo);
	return EXITO;
}

int enviarHeader_FS_TO_NODO_SET_BLOQUE(int socketNodo, t_log* logger, int tamanio) {
	header_t header = nuevoHeader(FS_TO_NODO_SET_BLOQUE, tamanio, 1);

	log_debug_header(logger, "enviarHeader_FS_TO_NODO_SET_BLOQUE", &header);

	return enviar_header(socketNodo, &header);
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

int enviarJobToNodoHandshake(int socketNodo, t_log* logger) {
	header_t header;

	initHeader(&header);
	header.tipo = JOB_TO_NODO_HANDSHAKE;
	header.cantidad_paquetes = 0;

	log_info(logger, "Se envia handshake JOB_TO_NODO_HANDSHAKE al Nodo por el socket [%d]\n",
			socketNodo);

	int ret;
	if ((ret = enviar_header(socketNodo, &header)) != EXITO) {
		log_error(logger,
				"Error al enviar el handshake JOB_TO_NODO_HANDSHAKE al Nodo por el socket [%d]\n",
				socketNodo);
		return ret;
	}

	return EXITO;

}


int recibirNodoToJobHandshakeOk(int socketNodo, t_log* logger) {

	header_t headerRecibir;

	log_info(logger, "recibirNodoToJobHandshakeOk: por el socket [%d]\n",
			 socketNodo);

	if (recibir_header_simple(socketNodo, &headerRecibir) != EXITO) {
		log_error(logger,
				"recibirNodoToJobHandshakeOk: Error al recibir struct  \n\n");
		return ERROR;
	}

	return EXITO;
}

int enviar_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT(int socketNodo, int nroBloque, t_log* logger, t_header tipo) {

	char* contenidoArchivo = string_new();
	string_append_with_format(&contenidoArchivo, "Archivo script! tipo: %s", getDescription(tipo));
	int tamanio = string_length(contenidoArchivo);


	if(enviarHeader(socketNodo, logger, tamanio, tipo) != EXITO) {
		log_error(logger,
				"enviar_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT: Error al recibir struct  \n\n");
		return ERROR;
	}

	log_info(logger, "Enviando contenido archivo[tamanio: %d] al nodo por el socket [%d]\n", tamanio, socketNodo);

	if (enviar(socketNodo, contenidoArchivo, tamanio) != EXITO)
	{
		log_error(logger, "Error enviando contenido archivo[nro_bloque: %d] al nodo por el socket [%d]\n", tamanio, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio contenido archivo[tamanio: %d] al nodo por el socket [%d]\n", tamanio, socketNodo);

	t_header recibirTipo = tipo == JOB_TO_NODO_MAP_SCRIPT ? NODO_TO_JOB_MAP_SCRIPT_OK:NODO_TO_JOB_REDUCE_SCRIPT_OK;

	int resultado4= recibir_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT_OK(socketNodo, nroBloque, logger, recibirTipo);
	if(resultado4 != EXITO) {
		return resultado4;
	}

	return EXITO;
}

int recibir_JOB_TO_NODO_MAP_o_REDUCE_SCRIPT_OK(int socketNodo, int nroBloque, t_log* logger, t_header tipo) {
	header_t header;

	int ret;
	if ((ret = recibirHeader(socketNodo, logger, &header)) != EXITO) {
		log_error(logger, "Error recibiendo respuesta %s", getDescription(tipo));
		return ret;
	}

	if(header.tipo == tipo) {
		log_info(logger, "Recibi respuesta ok %s\n", getDescription(header.tipo));
	} else {
		log_error(logger, "Error recibiendo respuesta, recibi %s\n", getDescription(header.tipo));
	}

	int largoMensaje = header.largo_mensaje;

	char* nombreArchivo = malloc(largoMensaje + 1); //agrego espacio para el \0
	memset(nombreArchivo, '\0', largoMensaje + 1);

	if((ret = recibir(socketNodo, nombreArchivo, largoMensaje)) != EXITO) {
		log_error(logger, "Error recibiendo nombre de archivo\n");
		return ret;
	}

	log_info(logger, "Se recibio nombre de archivo temporal: %s\n", nombreArchivo);

	if((ret = enviar_JOB_TO_NODO_MAP_o_REDUCE_REQUEST(socketNodo, nroBloque, nombreArchivo, logger, tipo)) != EXITO) {
		return ret;
	}

	return EXITO;
}


int enviar_JOB_TO_NODO_MAP_o_REDUCE_REQUEST(int socketNodo, int nroBloque, char* nombreArchivo,
		t_log* logger, t_header tipo) {
	char* mensaje = string_new();
	string_append_with_format(&mensaje, "[%d,%s]", nroBloque, nombreArchivo);
	int tamanioMensaje = string_length(mensaje);

	enviarHeader(socketNodo, logger, tamanioMensaje, JOB_TO_NODO_MAP_REQUEST);
	if (enviar(socketNodo, mensaje, tamanioMensaje) != EXITO)
	{
		log_error(logger, "Error enviando map request [nro_bloque: %d] al nodo por el socket [%d]\n", nroBloque, socketNodo);
		return ERROR;
	}


	header_t headerRecibir;
	recibirHeader(socketNodo, logger, &headerRecibir);

	//verificar el NODO_TO_JOB_MAP_OK y seguir con el reduce
	return EXITO;
}

