/*
 * files.c
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */
#include <stdlib.h>
#include <commons/string.h>

char *extraerNombreArchivo(char *text) {

	if (text != NULL) {
		char **elementoPath = string_split(text, "/");

		char *ultimoElementoEncontrado = NULL;
		while (*elementoPath != NULL) {
			ultimoElementoEncontrado = *elementoPath;
			elementoPath++;
		}

		return ultimoElementoEncontrado;
	}

	return NULL;
}
