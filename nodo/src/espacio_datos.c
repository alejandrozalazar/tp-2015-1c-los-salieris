/*
 * espacio_datos.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include "espacio_datos.h"

char* nuevoEspacioDeDatos(char* path, char* tamanioEspacioDatos, t_log* logger) {
	if( access( path, F_OK ) == -1 ) {
		crearArchivoMmap(path, tamanioEspacioDatos);
	}
	int fdEspacioDatos = abrirArchivoLecturaEscritura(path, logger);
	struct stat statArchivoEspacioDatos = describirArchivoEspacioDatos(path, logger);
    int tamanioArchivoEspacioDatos = statArchivoEspacioDatos.st_size;
    return crearEspacioDeDatos(fdEspacioDatos, tamanioArchivoEspacioDatos, logger);
}

char* crearEspacioDeDatos(int fd, int tamanioEspacioDatos, t_log* logger) {
	char* data = (char *) mmap((caddr_t) 0, tamanioEspacioDatos, PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == (caddr_t) (-1)) {
		log_error(logger, "mmap: %s", strerror(errno));
		exit(1);
	}
	return data;
}

void eliminarEspacioDeDatos(char* data, int tamanioEspacioDatos, t_log* logger) {
	int unmapResult = munmap(data, tamanioEspacioDatos);
	if (unmapResult == -1) {
		log_error(logger, "munmap: %s", strerror(errno));
		exit(1);
	}
}

int abrirArchivoEspacioDatos(char* archivoMmap, t_log* logger) {
	return abrirArchivoLecturaEscritura(archivoMmap, logger);
}

struct stat describirArchivoEspacioDatos(char* archivoMmap, t_log* logger) {
	struct stat sbuf;
	if (stat(archivoMmap, &sbuf) == -1) {
		log_error(logger, "stat: %s", strerror(errno));
		exit(1);
	}
	return sbuf;
}

void cerrarArchivoEspacioDeDatos(int fd, t_log* logger) {
	int closeResult = close(fd);
	if (closeResult == -1) {
		log_error(logger, "close: %s", strerror(errno));
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


void crearArchivoMmap(char *pathArchivo, char* tamanioArchivoEnBytes) {
	char *command = string_new();
	string_append_with_format(&command, "truncate -s %s %s", tamanioArchivoEnBytes, pathArchivo);
	int systemResult = system(command);
	if(systemResult < 0) {
		perror("truncate");
		exit(1);
	}
	free(command);
}

void borrarArchivoMmapParaTest(char *pathArchivo) {
	unlink(pathArchivo);
}
