/*
 * principal.h
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_PRINCIPAL_H_
#define SRC_PRINCIPAL_H_

#include <commons/log.h>
#include <commons/string.h>
#include <commons/process.h>
#include <mensajeria/mensajes.h>
//#include <mensajeria/sockets.h>
#include "conf.h"
#include "int_nodos_y_filesystem.h"
#include "network.h"
#include "struct.h"

int ejecutarProgramaPrincipal();
t_estado* inicializarEstado();
int enviarNodoToJobHandshakeOk(int socketNodo, t_log* logger);

#endif /* SRC_PRINCIPAL_H_ */
