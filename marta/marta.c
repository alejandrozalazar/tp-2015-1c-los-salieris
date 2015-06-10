/*
 * kernel.c
 *
 *  Created on: 11/10/2014
 *      Author: utnso
 */

#include "marta.h"

int main(){

//	system("clear"); //HACK me tiraba un error de TERM

	int kernel_pid = getpid();
	init(); //HACK invertir el orden, por que no estaba inicializado el header
	log_info(LOGGER, "************** MaRTA iniciado V1.0! (PID: %d) ***************\n", kernel_pid); //HACK idem

	escucha(config_get_int_value(CONF, "PUERTO"));

	finish();
	return EXIT_SUCCESS;
}


void finish(){
	log_destroy(LOGGER);
	config_destroy(CONF);
}

void init(){

	LOGGER = log_create(LOG_PATH, "MaRTA", true, LOG_LEVEL_DEBUG);
	CONF = config_create(CONF_PATH); //HACK estaba mal el nombre de la funcion o no la encontraba o no se

	if(LOGGER == NULL){
		perror("ERROR! no pudo levantar ni el log!");
		exit(EXIT_FAILURE);
	}

	if(CONF == NULL){
		log_error(LOGGER, "No pudo levantar la configuración de marta");
		exit(EXIT_FAILURE);
	}

//	if(!validarConfig()){ //HACK no lo necesitaba, no entendi el codigo del map y no me servia, anda :P
//		log_error(LOGGER, "No pudo obtenerse alguno de los parametros de configuracion");
//		exit(EXIT_FAILURE);
//	}

	mapa_archivos = dictionary_create();
	pthread_mutex_init(&mutex_mapa_archivos, NULL);

	socketFS = conectarAServidor(config_get_string_value(CONF, "IP_FS"), config_get_int_value(CONF, "PUERTO_FS"));



}

bool validarConfig(){
	char keys[4][15] = {"IP", "PUERTO", "IP_FS", "PUERTO_FS"};
	int i;
	for(i=0;i<7;i++){
		if(string_is_empty(config_get_string_value(CONF, keys[i]))){
			return false;
		}
	}
	return true;
}

