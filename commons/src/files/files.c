/*
 * files.c
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */
#include "files.h"

mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

int abrirArchivoLecturaEscritura(char* pathArchivo, t_log* logger) {
	int fd = open(pathArchivo, O_RDWR, mode);
	if (fd == -1) {
		log_error(logger, "open rw: %s", strerror(errno));
		exit(1);
	}
	return fd;
}


int abrirOCrearArchivoLecturaEscritura(char* pathArchivo, t_log* logger) {
	int fd = open(pathArchivo, O_RDWR | O_CREAT, mode);
	if (fd == -1) {
		log_error(logger, "open or create rw", strerror(errno));
		exit(1);
	}
	return fd;
}

int abrirArchivoSoloLectura(char* pathArchivo, t_log* logger) {
	int fd = open(pathArchivo, O_RDONLY, mode);
	if (fd == -1) {
		log_error(logger, "open read only", strerror(errno));
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


struct stat describirArchivo(char* archivo, t_log* logger) {
	struct stat sbuf;
	if (stat(archivo, &sbuf) == -1) {
		log_error(logger, "stat: archivo: %s, error: %s", archivo, strerror(errno));
		exit(1);
	}
	return sbuf;
}
