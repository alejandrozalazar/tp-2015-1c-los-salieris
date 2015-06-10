/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

t_estado* estado;

int ejecutarProgramaPrincipal(t_estado* estado) {

	t_log* logger = estado->logger;
	log_info(logger, "Inicializado");

	//todonodo conectar a filesystem
	if(conectarAFileSystem(estado) > 0) {
		//todonodo escuchar respuesta filesystem
		//si la respuesta es Ok comenzar a escuchar filesystem (otra conexion?), nodos, hilos mapper y reducer

	} else {
		log_info(logger, "No se pudo conectar al filesystem %s:%d", estado->conf->IP_FS, estado->conf->PUERTO_FS);
	}

	//si la respuesta es NO OK o hubo algun problema, loggear y salir




	log_info(logger, "Finalizando");

	log_info(logger, "Exit");

	return 0;
}
