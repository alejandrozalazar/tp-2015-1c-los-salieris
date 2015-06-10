/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

conf_nodo* configuracion;
t_estado* estado;

t_estado* inicializarEstado() {

	t_estado* miestado = malloc(sizeof(t_estado));
	t_log* logger = log_create(getRutaLog(), "Nodo", true, LOG_LEVEL_DEBUG);
	miestado->logger = logger;
	return miestado;
}

int ejecutarProgramaPrincipal() {

	configuracion = cargarConfiguracion();
	estado= inicializarEstado();

	t_log* logger = estado->logger;
	log_debug(logger, "Nodo inicializado");

	return 0;
}
