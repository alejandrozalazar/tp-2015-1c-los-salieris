/*
 * files.c
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>

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
