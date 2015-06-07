/*
 * filesystem.c
 *
 *  Created on: 07/06/2015
 *      Author: utnso
 */

#include "filesystem.h"

int main(){

	init();

	pthread_create(&tid_fs_marta, NULL, (void*) fs_marta, NULL);
	pthread_create(&tid_fs_nodos, NULL, (void*) fs_nodos, NULL);

	pthread_join(tid_fs_nodos, NULL);
	pthread_join(tid_fs_marta, NULL);

	finish();
	return EXIT_SUCCESS;
}

void init(){

	if( (logger = log_create(LOG_PATH, "filesystem", true, LOG_LEVEL_TRACE)) == NULL){
		exit(EXIT_FAILURE);
	}

	config_load();

	inicializar_estructuras();

}

void inicializar_estructuras(){

}

void finish(){
	log_destroy(logger);
	config_destroy(config);

}

void config_load(){

	config = config_create(CONFIG_PATH);
	if(config== NULL){
		log_error(logger, "No pudo cargar la configuracion del path %s", CONFIG_PATH);
		exit(EXIT_FAILURE);
	}

	char keys[2][15] = {"LISTA_NODOS", "PUERTO_LISTEN"};
	int i;
	for(i = 0; i < 2; i++){
		if(config_get_string_value(config, keys[i]) == NULL){
			log_error(logger, "El value de la key %s es NULL", keys[i]);
			exit(EXIT_FAILURE);
		}
	}
}

