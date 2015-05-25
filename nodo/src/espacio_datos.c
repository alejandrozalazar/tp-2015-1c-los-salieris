/*
 * espacio_datos.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include <commons/string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "files.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

char* crearEspacioDeDatos(int fd, int tamanioEspacioDatos) {
	char* data = (char *) mmap((caddr_t) 0, tamanioEspacioDatos, PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == (caddr_t) (-1)) {
		perror("mmap");
		exit(1);
	}
	return data;
}

void eliminarEspacioDeDatos(char* data, int tamanioEspacioDatos) {
	int unmapResult = munmap(data, tamanioEspacioDatos);
	if (unmapResult == -1) {
		perror("munmap");
		exit(1);
	}
}

int abrirArchivoEspacioDatos(char* archivoMmap) {
	return abrirArchivoLecturaEscritura(archivoMmap);
}

struct stat describirArchivoEspacioDatos(char* archivoMmap) {
	struct stat sbuf;
	if (stat(archivoMmap, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}
	return sbuf;
}

void cerrarArchivoEspacioDeDatos(int fd) {
	int closeResult = close(fd);
	if (closeResult == -1) {
		perror("close");
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
