/*
 * espacio_datos.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include "espacio_datos.h"

char* crearEspacioDeDatos(int fd, int tamanioEspacioDatos, t_log* logger) {
	char* data = (char *) mmap((caddr_t) 0, tamanioEspacioDatos, PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == (caddr_t) (-1)) {
		log_info(logger, "mmap: %s", strerror(errno));
		exit(1);
	}
	return data;
}

void eliminarEspacioDeDatos(char* data, int tamanioEspacioDatos, t_log* logger) {
	int unmapResult = munmap(data, tamanioEspacioDatos);
	if (unmapResult == -1) {
		log_info(logger, "munmap: %s", strerror(errno));
		exit(1);
	}
}

int abrirArchivoEspacioDatos(char* archivoMmap, t_log* logger) {
	return abrirArchivoLecturaEscritura(archivoMmap);
}

struct stat describirArchivoEspacioDatos(char* archivoMmap, t_log* logger) {
	struct stat sbuf;
	if (stat(archivoMmap, &sbuf) == -1) {
		log_info(logger, "stat: %s", strerror(errno));
		exit(1);
	}
	return sbuf;
}

void cerrarArchivoEspacioDeDatos(int fd, t_log* logger) {
	int closeResult = close(fd);
	if (closeResult == -1) {
		log_info(logger, "close: %s", strerror(errno));
		exit(1);
	}
}


void escribirEnEspacioDatos(char* espacioDatos, char* contenido, int offset) {
	int posicionAEscribir = offset;
	int posicionALeer;
	int limit = string_length(contenido);
	for (posicionALeer = 0; posicionALeer < limit; posicionALeer++) {
		espacioDatos[posicionAEscribir++] = contenido[posicionALeer];
	}
}

char * leerEspacioDatos(char *espacioDatos, int offset, int cantidadALeer) {
	char *resultado = malloc(cantidadALeer + 1);

	int posicionALeer = offset;

	int posicionAEscribir;
	for (posicionAEscribir = 0; posicionAEscribir < cantidadALeer; posicionAEscribir++) {
		resultado[posicionAEscribir] = espacioDatos[posicionALeer++];
	}

	return resultado;
}
