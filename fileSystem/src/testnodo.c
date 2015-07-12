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

int tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* LOGGER) {

	switch (mensaje->tipo) {
	case NODO_TO_FS_HANDSHAKE:
		log_info(logger, "Mensaje recibido: [%s] del socket [%d]",
				getDescription(mensaje->tipo), numSocket);
		enviarFSToNodoHandshakeOk(numSocket, logger);

		sleep(5);
		log_info(logger, "YA ESPERE");


		bool getSetBloqueNodo = false;
		if(getSetBloqueNodo == true)
		{
	//		puts("================================ INI get bloque 2 =========================");
	//
	//		int nroBloque = 2;
	//		int resultadoSetBloque1 = enviarFSToNodoGetBloque(numSocket, logger, nroBloque);
	//		if(resultadoSetBloque1 != EXITO) {
	//			return ERROR;
	//		}
	//
	//		puts("================================ FIN get bloque 2 =========================");

			puts("================================ INI set bloque 5 =========================");
			int nroSetGetBloque = 5;

			char* setBloqueContent = "linea3=hola\nlinea2=hola2\nlinea5=hola tarolas";
			int resultadoSetBloque2 = enviarFsToNodoSetBloque(numSocket, logger, nroSetGetBloque, setBloqueContent);

			if(resultadoSetBloque2 != EXITO) {
				return ERROR;
			}
			puts("================================ FIN set bloque 5 =========================");

			puts("================================ INI get bloque 5 =========================");
			int resultadoGetBloque2 = enviarFSToNodoGetBloque(numSocket, logger, nroSetGetBloque);
			if(resultadoGetBloque2 != EXITO) {
				return ERROR;
			}
			puts("================================ FIN get bloque 5 =========================");

			return resultadoGetBloque2;
		}

		bool interfazNodo = true;
		if(interfazNodo == true)
		{
			int resultado1= enviarJobToNodoHandshake(numSocket, logger);
			if(resultado1 != EXITO) {
				return resultado1;
			}

			int resultado2= recibirNodoToJobHandshakeOk(numSocket, logger);
			if(resultado2 != EXITO) {
				return resultado2;
			}

			int resultado3= enviar_JOB_TO_NODO_MAP_SCRIPT(numSocket, logger);
			if(resultado3 != EXITO) {
				return resultado3;
			}
		}

		return EXITO;
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

	int ret;
	if ((ret = enviar_header(socketNodo, &header)) != EXITO) {
		log_error(logger,
				"Error al enviar el handshake OK al Nodo por el socket [%d]\n",
				socketNodo);
		return ret;
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

	header_t headerRecibir;
	recibir_header_simple(socketNodo, &headerRecibir);

	return recibirNodoToFSGetBloque(socketNodo, &headerRecibir, logger);
}

int recibirNodoToFSGetBloque(int socketNodo, header_t* header, t_log* logger) {

	t_nro_bloque getBloque;

	log_info(logger, "recibirNodoToFSGetBloque: sizeof(t_nro_bloque): %d %s por el socket [%d]\n",
			sizeof(t_nro_bloque), getDescription(header->tipo), socketNodo);

	if (recibir_struct(socketNodo, &getBloque, sizeof(t_nro_bloque)) != EXITO) {
		log_error(logger,
				"recibirNodoToFSGetBloque: Error al recibir struct getBloque \n\n");
		return ERROR;
	}

	log_info(logger, "recibirNodoToFSGetBloque: bloque solicitado nro: %d por el socket [%d]\n",
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
			"recibirNodoToFSGetBloque: INICIO contenido recibido de bloque solicitado nro: %d por el socket [%d]\n",
			getBloque.nro_bloque, socketNodo);
	log_info(logger,
				"%s \n",
				contenidoBloque);
	log_info(logger,
				"recibirNodoToFSGetBloque: FIN contenido recibido de bloque solicitado nro: %d \n",
				getBloque.nro_bloque);

	return EXITO;
}

int enviarFsToNodoSetBloque(int socketNodo, t_log* logger, int nroBloque, char* contenidoBloque) {
	size_t tamanioBloque = strlen(contenidoBloque); //no envio el \0

	header_t header;

	initHeader(&header);
	header.tipo = FS_TO_NODO_SET_BLOQUE;
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

int enviar_JOB_TO_NODO_MAP_SCRIPT(int socketNodo, t_log* logger) {

	char* contenidoArchivo = "Archivo con script!";
	int tamanio = string_length(contenidoArchivo);


	if(enviarHeader(socketNodo, logger, tamanio, JOB_TO_NODO_MAP_SCRIPT) != EXITO) {
		log_error(logger,
				"enviar_JOB_TO_NODO_MAP_SCRIPT: Error al recibir struct  \n\n");
		return ERROR;
	}

	log_info(logger, "Enviando contenido archivo[tamanio: %d] al nodo por el socket [%d]\n", tamanio, socketNodo);

	if (enviar(socketNodo, contenidoArchivo, tamanio) != EXITO)
	{
		log_error(logger, "Error enviando contenido archivo[nro_bloque: %d] al nodo por el socket [%d]\n", tamanio, socketNodo);
		return ERROR;
	}

	log_info(logger, "Se envio contenido archivo[tamanio: %d] al nodo por el socket [%d]\n", tamanio, socketNodo);

	return EXITO;
}



