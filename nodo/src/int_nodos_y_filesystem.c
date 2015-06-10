/*
 * int_nodos_y_filesystem.c
 *
 *  Created on: 18/5/2015
 *      Author: Alejandro Zalazar
 */

#include "int_nodos_y_filesystem.h"

void getBloque(int numeroBloque, t_estado* estado) {

	leerEspacioDatos(estado->espacioDatos, numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES, estado->conf->BLOCK_SIZE_IN_BYTES);
}
void setBloque(int numeroBloque, int dataFileDescriptor) {

}
void getFileContent(char* nombreArchivoTemporal) {

}
