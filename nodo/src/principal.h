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


int recibirHeader(int socketNodo, t_log* logger, header_t* headerRecibir);

int enviarHeader(int socketNodo, t_log* logger, int tamanio, t_header tipo);

int recibirNroBloque(int socketNodo, t_log* logger, t_nro_bloque* headerGetBloque);

int enviarNroBloque(int socketNodo, int nroBloque, t_log* logger);

//header_t mensaje;
//recibir_header_simple(socketCliente, &mensaje);
//header_t* pMensaje = &mensaje;
//
//log_debug(logger, "Recibi un header tipo: %d, tamanio: %d", pMensaje->tipo, pMensaje->largo_mensaje);
//
//if(pMensaje->tipo == JOB_TO_NODO_MAP_REQUEST) {
//	t_map_request_nodo mapRequestNodo;
//	bool isDesconecto;
//	recibir_map_request_nodo(socketFD, &mapRequestNodo, &isDesconecto);
//} else {
//	log_debug(logger, "Mensaje no soportado");
//}

//int enviarNodoToJobHandshakeOk(int socketNodo, t_log* logger);

#endif /* SRC_PRINCIPAL_H_ */
