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
#include <mensajeria/mensajes.h>
#include <mensajeria/sockets.h>
#include "struct.h"
#include "conf.h"

int ejecutarProgramaPrincipal();
t_estado* inicializarEstado();

#endif /* SRC_PRINCIPAL_H_ */
