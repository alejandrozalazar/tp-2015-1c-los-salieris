/*
 * job.c
 *
 *  Created on: 18/04/2015
 *      Author: utnso
 */


#include "job.h"

int32_t  main(){
	int loTermino = 0;
	LOG = log_create(LOG_PATH, "Proceso Job", true, LOG_LEVEL_INFO);

	int32_t id_proceso = process_getpid();
	int32_t id_hilo = process_get_thread_id();

	system("clear");
	log_info(LOG, "************** Proceso Job (PID: %d) (TID: %d)***************\n"
		, id_proceso, id_hilo);

	CONFIG = config_load(CONF_PATH, LOG);
	validarConfig();

	if(!validarConfig()){
		log_error(LOG, "faltan parámetros necesarios en %s", CONF_PATH);
	}

	socketMarta = conectarAServidor(config_get_string_value(CONFIG, "IP_MARTA"), config_get_int_value(CONFIG, "PUERTO_MARTA"));

	if (socketMarta == -1) {
		log_error(LOG, "Imposible conectar\n\n");
		return EXIT_FAILURE;
	}

	t_contenido mensaje;
	memset(mensaje, 0, sizeof(t_contenido));

	if(doHandShake(socketMarta, JOB_TO_MARTA_HANDSHAKE, LOG) == -1){
		return EXIT_FAILURE;
	}

//	indicarArchivosJob(socketMarta, config_get_string_value(CONFIG, "ARCHIVOS"));

	while(!loTermino){

		t_header header = recibirMensaje(socketMarta, mensaje, LOG);

		if(header == ERR_CONEXION_CERRADA){
			loTermino = 1;
		} else if (header == MARTA_TO_JOB_MAP_REQUEST){
			log_info(LOG, "MARTA_TO_JOB_MAP_REQUEST");
//			levantoHiloMap();
		} else if( header == MARTA_TO_JOB_REDUCE_REQUEST){
			log_info(LOG, "MARTA_TO_JOB_REDUCE_REQUEST");
//			levantoHiloReduce();
		}

	}

	finish();

	puts("soy un job");
	return EXIT_SUCCESS;
}

bool validarConfig(){
	return config_get_string_value(CONFIG, "IP_MARTA") != NULL && config_get_string_value(CONFIG, "PUERTO_MARTA") != NULL;
}

void finish(){
	log_destroy(LOG);
	config_destroy(CONFIG);
}
