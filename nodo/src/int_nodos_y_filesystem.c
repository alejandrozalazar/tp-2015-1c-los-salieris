/*
 * int_nodos_y_filesystem.c
 *
 *  Created on: 18/5/2015
 *      Author: Alejandro Zalazar
 */

#include "int_nodos_y_filesystem.h"

char* getBloque(int numeroBloque, t_estado* estado) {

	return leerEspacioDatos(estado->espacioDatos, numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES, estado->conf->BLOCK_SIZE_IN_BYTES);
}
void getBloqueYEscribir(int numeroBloque, t_estado* estado, int fileDescriptorToWrite) {
	int offset = numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES;
	int cantidadALeerRestante = estado->conf->BLOCK_SIZE_IN_BYTES;
	int bufferSize = 100;
	void* buffer = malloc(bufferSize);
	while(cantidadALeerRestante > 0) {

		int cantidadALeerPorVez = cantidadALeerRestante > bufferSize? bufferSize: cantidadALeerRestante;

		char* lectura = leerEspacioDatos(estado->espacioDatos, offset, cantidadALeerPorVez);
		cantidadALeerRestante = cantidadALeerRestante - cantidadALeerPorVez;
	}
}
void setBloque(int numeroBloque, t_estado* estado, char* contenido) {

	escribirEnEspacioDatos(estado->espacioDatos, contenido, numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES);
}
void getFileContent(char* nombreArchivoTemporal) {

}
