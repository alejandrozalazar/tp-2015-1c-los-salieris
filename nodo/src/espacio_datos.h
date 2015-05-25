/*
 * espacio_datos.h
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_ESPACIO_DATOS_H_
#define SRC_ESPACIO_DATOS_H_



char* crearEspacioDeDatos(int fd, int tamanioEspacioDatos);

void eliminarEspacioDeDatos(char* data, int tamanioEspacioDatos);

int abrirArchivoEspacioDatos(char* archivoMmap);

struct stat describirArchivoEspacioDatos(char* archivoMmap);

void cerrarArchivoEspacioDeDatos(int fd);

void escribirEnEspacioDatos(char* espacioDatos, char* contenido, int offset);

char * leerEspacioDatos(char *espacioDatos, int offset, int cantidadALeer) ;


#endif /* SRC_ESPACIO_DATOS_H_ */

