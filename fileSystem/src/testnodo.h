/*
 * testnodo.h
 *
 *  Created on: Jun 28, 2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_TESTNODO_H_
#define SRC_TESTNODO_H_

#include "fileSystem.h"

int mainAlternativo(int argc, char *argv[]);
void log_debug_header(t_log* logger, char* mensaje, header_t* header);
char* obtenerContenidoArchivo(char* filePath);
t_nodo crearNodo() ;
#endif /* SRC_TESTNODO_H_ */
