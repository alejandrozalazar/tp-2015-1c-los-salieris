/*
 * network.h
 *
 *  Created on: 10/6/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include <mensajeria/sockets.h>
#include <mensajeria/comunicacion.h>
#include "struct.h"


int conectarAFileSystem(t_estado* estado);
t_nodo nuevoNodo(char* nombre, char* ip, int puerto);
#endif /* SRC_NETWORK_H_ */
