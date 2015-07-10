/*
 * espacio_datos.h
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_ESPACIO_DATOS_H_
#define SRC_ESPACIO_DATOS_H_

#include <commons/log.h>
#include <commons/string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <files/files.h>

char* nuevoEspacioDeDatos(char* path, char* tamanioEspacioDatos, t_log* logger);

char* crearEspacioDeDatos(int fd, int tamanioEspacioDatos, t_log* logger);

void eliminarEspacioDeDatos(char* data, int tamanioEspacioDatos, t_log* logger);

int abrirArchivoEspacioDatos(char* archivoMmap, t_log* logger);

struct stat describirArchivoEspacioDatos(char* archivoMmap, t_log* logger);

void cerrarArchivoEspacioDeDatos(int fd, t_log* logger);

void escribirEnEspacioDatos(char* espacioDatos, char* contenido, int offset);

char * leerEspacioDatos(char *espacioDatos, int offset, int cantidadALeer) ;

void crearArchivoMmap(char *pathArchivo, char* tamanioArchivoEnBytes);

void borrarArchivoMmapParaTest(char *pathArchivo) ;


#endif /* SRC_ESPACIO_DATOS_H_ */

