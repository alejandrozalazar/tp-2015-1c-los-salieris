/*
 * files.h
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_FILES_H_
#define SRC_FILES_H_

#include "../commons/log.h"
#include "../commons/string.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

char *extraerNombreArchivo(char *text);

int abrirArchivoSoloLectura(char* pathArchivo, t_log* logger);

int abrirArchivoLecturaEscritura(char* pathArchivo, t_log* logger);

int abrirOCrearArchivoLecturaEscritura(char* pathArchivo, t_log* logger);

struct stat describirArchivo(char* archivo, t_log* logger);
#endif /* SRC_FILES_H_ */

