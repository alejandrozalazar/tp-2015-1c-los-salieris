/*
 * files.c
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */
#include <commons/string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

int abrirArchivoLecturaEscritura(char* pathArchivo) {
	int fd = open(pathArchivo, O_RDWR, mode);
	if (fd == -1) {
		perror("open rw");
		exit(1);
	}
	return fd;
}


int abrirOCrearArchivoLecturaEscritura(char* pathArchivo) {
	int fd = open(pathArchivo, O_RDWR | O_CREAT, mode);
	if (fd == -1) {
		perror("open or create rw");
		exit(1);
	}
	return fd;
}

int abrirArchivoSoloLectura(char* pathArchivo) {
	int fd = open(pathArchivo, O_RDONLY, mode);
	if (fd == -1) {
		perror("open read only");
		exit(1);
	}
	return fd;
}

char *extraerNombreArchivo(char *text) {

	if (text != NULL) {
		char **elementoPath = string_split(text, "/");

		char *ultimoElementoEncontrado = NULL;

		while (*elementoPath != NULL) {
			if(ultimoElementoEncontrado != NULL) {
				free(ultimoElementoEncontrado);
			}

			ultimoElementoEncontrado = string_duplicate(*elementoPath);
			free(*elementoPath);
			elementoPath++;
		}

		return ultimoElementoEncontrado;
	}

	return NULL;
}
