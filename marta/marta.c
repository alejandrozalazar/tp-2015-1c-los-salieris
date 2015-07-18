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
	log_debug(LOGGER, "************** MaRTA iniciado V1.0! (PID: %d) ***************\n", kernel_pid); //HACK idem

	escucha(config_get_int_value(CONF, "PUERTO"));

	finish();
	return EXIT_SUCCESS;
}


void finish(){
	// crear una funcion que libere BIEN el job de la lista, que incluya diccionario..
	list_destroy_and_destroy_elements(lista_jobs, (void*)free);
	dictionary_clean_and_destroy_elements(mapa_nodos, (void*)free);
	pthread_mutex_destroy(&mutex_mapa_nodos);
	pthread_mutex_destroy(&mutex_lista_jobs);
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

	mapa_nodos = dictionary_create();
	lista_jobs = list_create();
	pthread_mutex_init(&mutex_mapa_nodos, NULL);
	pthread_mutex_init(&mutex_lista_jobs, NULL);


	log_info(LOGGER, "intenta conectarse a la ip %s con puerto %d", config_get_string_value(CONF, "IP_FS"), config_get_int_value(CONF, "PUERTO_FS"));
	socketFS = conectarAServidor(config_get_string_value(CONF, "IP_FS"), config_get_int_value(CONF, "PUERTO_FS"));
	while (socketFS == -1) {
		 log_info(LOGGER, "Filesystem no levantado. Se reintenta conexion en unos segundos");
		 sleep(5);
		 socketFS = conectarAServidor(config_get_string_value(CONF, "IP_FS"), config_get_int_value(CONF, "PUERTO_FS"));
	}

	t_mensaje mensaje;
	memset(&mensaje, 0, sizeof(t_mensaje));

	log_info(LOGGER, "envio header %s FS", getDescription(mensaje.tipo));
	enviar_t_mensaje(socketFS, MARTA_TO_FS_HANDSHAKE, 0, "");
	recibir_t_mensaje(socketFS, &mensaje);
	log_info(LOGGER, "recibo respuesta %s de FS", getDescription(mensaje.tipo));

	while(mensaje.tipo != ACK){
		log_info(LOGGER, "Todavia el FS no esta disponible. Header recibido: %s. Reintento en 5 segundos", getDescription(mensaje.tipo));
		sleep(5);
		enviar_t_mensaje(socketFS, MARTA_TO_FS_HANDSHAKE, 0, "");
		recibir_t_mensaje(socketFS, &mensaje);
	}

	log_info(LOGGER, "%s: FS está operativo. Continúo", getDescription(mensaje.tipo));
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

