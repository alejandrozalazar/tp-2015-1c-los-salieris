/*
 * files.h
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_FILES_H_
#define SRC_FILES_H_


char *extraerNombreArchivo(char *text);

int abrirArchivoSoloLectura(char* pathArchivo);

int abrirArchivoLecturaEscritura(char* pathArchivo);

int abrirOCrearArchivoLecturaEscritura(char* pathArchivo);

#endif /* SRC_FILES_H_ */

