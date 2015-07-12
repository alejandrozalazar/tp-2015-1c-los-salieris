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
void escribirBloque(int numeroBloque, t_estado* estado, char* contenido) {

	log_debug(estado->logger, "INICIO escribir en espacio de datos bloque nro: %d\n", numeroBloque);
	log_debug(estado->logger, "%s \n", contenido);

	escribirEnEspacioDatos(estado->espacioDatos, contenido, numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES);

	log_debug(estado->logger, "FIN escribir en espacio de datos bloque nro: %d \n", numeroBloque);
}
void getFileContent(char* nombreArchivoTemporal) {

}
