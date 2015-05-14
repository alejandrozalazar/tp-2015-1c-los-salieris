/*
 * kernel.c
 *
 *  Created on: 11/10/2014
 *      Author: utnso
 */

#include "marta.h"

int main(){

	// TODO: agregar logs para el planificador y para el loader
	system("clear");

	// Hello Kernel!
	int kernel_pid = getpid();
	log_info(logger, "************** WELCOME TO KERNEL V1.0! (PID: %d) ***************\n", kernel_pid);
	init();

	escucha(config_get_int_value(CONF, "PUERTO"));

	finish();
	return EXIT_SUCCESS;
}


void finish(){
	log_destroy(logger);
	config_destroy(CONF);
}

void init(){
	logger = log_create(LOG_PATH, "Kernel", 1, LOG_LEVEL_DEBUG);
	CONF = config_load(CONF_PATH, logger);

	if(logger == NULL){
		perror("ERROR! no pudo levantar ni el log!");
		exit(1);
	}

	if(CONF == NULL){
		log_error(logger, "No pudo levantar la configuración de marta");
		exit(1);
	}

	if(config_get_string_value(CONF, "PUERTO") == NULL){
		log_error(logger, "No pudo obtenerse el puerto de escucha");
		exit(1);
	}
}
