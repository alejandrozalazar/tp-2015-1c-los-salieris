/*
 * int_nodos_y_filesystem.h
 *
 *  Created on: 18/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_INT_NODOS_Y_FILESYSTEM_H_
#define SRC_INT_NODOS_Y_FILESYSTEM_H_

#include "espacio_datos.h"
#include "struct.h"

char* getBloque(int numeroBloque, t_estado* estado); //Devolverá el contenido del bloque solicitado almacenado en el Espacio de Datos.
void escribirBloque(int numeroBloque, t_estado* estado, char* contenido); //​Grabará los datos enviados en el bloque solicitado del Espacio de Datos
void getFileContent(char* nombreArchivoTemporal); //Devolverá el contenido del archivo de Espacio Temporal solicitado.
char* setFileContent(char* contenidoArchivoTemporal, t_estado* estado, char* mascara);
void getBloqueYEscribir(int numeroBloque, t_estado* estado, int fileDescriptorToWrite);

char* generarNombreArchivoTemporal(char* tempDirectory, char* mascara);

#endif /* SRC_INT_NODOS_Y_FILESYSTEM_H_ */

