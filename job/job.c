/*
 * job.c
 *
 *  Created on: 18/04/2015
 *      Author: utnso
 */


#include "job.h"

bool validarConfig();

int32_t  main(){
	bool loTermino = false;
	LOGGER = log_create(LOGGER_PATH, "Proceso Job", true, LOG_LEVEL_INFO);

	int32_t id_proceso = process_getpid();
	int32_t id_hilo = process_get_thread_id();

	system("clear");
	log_info(LOGGER, "************** Proceso Job (PID: %d) (TID: %d)***************\n"
		, id_proceso, id_hilo);

	CONFIG = config_create(CONFIG_PATH);

	if(!validarConfig()){
		log_error(LOGGER, "faltan parámetros necesarios en %s", CONFIG_PATH);
		return EXIT_FAILURE;
	}

	if( (bytesMapperScript = getBytesFromScript(config_get_string_value(CONFIG, "MAPPER"), &sizeMapperScript) )== NULL){
		log_error(LOGGER, "No pudo abrirse el archivo %s para enviar al nodo", config_get_string_value(CONFIG, "MAPPER"));
		return EXIT_FAILURE;
	}

	if( (bytesReduceScript = getBytesFromScript(config_get_string_value(CONFIG, "REDUCE"), &sizeReduceScript) )== NULL){
		log_error(LOGGER, "No pudo abrirse el archivo %s para enviar al nodo", config_get_string_value(CONFIG, "REDUCE"));
		return EXIT_FAILURE;
	}

	if ((socketMarta = conectarAServidor(config_get_string_value(CONFIG, "IP_MARTA"), config_get_int_value(CONFIG, "PUERTO_MARTA"))) == -1) {
		log_error(LOGGER, "Imposible conectar");
		return EXIT_FAILURE;
	}

	if(enviarSerializado(LOGGER, socketMarta, false, JOB_TO_MARTA_HANDSHAKE, 0, NULL)){
		log_error(LOGGER, "No pudo hacer handshake");
		return EXIT_FAILURE;
	}

	if(indicarArchivosJob() == ERR_ERROR_AL_RECIBIR_MSG){
		log_error(LOGGER, "No pudo hacer handshake");
		return EXIT_FAILURE;
	}

	t_mensaje* mensaje = NULL;
	while(!loTermino){

		// probarlo y anda así. teóricamente sí, por el char*
		t_header header = recibirDeserializado(LOGGER, true, socketMarta, mensaje);
		log_info(LOGGER, "Mensaje recibido: [%s]", getDescription(mensaje->tipo));

		switch(header){

		case ERR_CONEXION_CERRADA:
			loTermino = true;
			log_error(LOGGER, "Marta no responde. Se termina el Job",  header);
			break;

		case MARTA_TO_JOB_FILE_NOT_FOUND:
			loTermino = true;
			log_error(LOGGER, "Marta no encontró el archivo solicitado. Se termina el Job",  header);
			break;

		case MARTA_TO_JOB_JOB_FINISHED:
			loTermino = true;
			log_info(LOGGER, "El job ejecutó correctamente! Finalizo",  header);
			break;

		case MARTA_TO_JOB_MAP_REQUEST:
			atiendeMapper(mensaje->contenido);
			break;

		case MARTA_TO_JOB_REDUCE_REQUEST:
			atiendeReduce(mensaje->contenido);
			break;

		default: log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  header);

		}

		freeMensaje(mensaje);

	}

	log_info(LOGGER, "El job Finaliza su ejecución");
	finish();

	return EXIT_SUCCESS;
}

t_header indicarArchivosJob(){
	t_mensaje* mensaje = NULL;
	t_header header;
	char* contenido = config_get_string_value(CONFIG, "ARCHIVOS");
	int tamanio = strlen(contenido) + 1;

	enviarSerializado(LOGGER, socketMarta, true, JOB_TO_MARTA_FILES, tamanio, contenido);
	header = recibirDeserializado(LOGGER, socketMarta, false, mensaje);
	free(mensaje);

	return header;
}

bool validarConfig(){
	char keys[7][15] = {"IP_MARTA", "PUERTO_MARTA", "MAPPER", "REDUCE", "COMBINER", "ARCHIVOS", "RESULTADO"};
	int i;
	for(i=0;i<7;i++){
		if(string_is_empty(config_get_string_value(CONFIG, keys[i]))){
			return false;
		}
	}
	return true;
}

void atiendeMapper(char* mensaje){
	t_hilo_mapper* hilo_mapper = crearEstructuraHiloMapper(mensaje);
	pthread_create (&hilo_mapper->tid, NULL, (void*)mapper, (t_hilo_mapper*)hilo_mapper);

}

void atiendeReduce(char* mensaje){
	t_hilo_reduce* hilo_reduce = crearEstructuraHiloReduce(mensaje);
	pthread_create (&hilo_reduce->tid, NULL, (void*)reduce, (t_hilo_reduce*)hilo_reduce);
}

t_hilo_mapper* crearEstructuraHiloMapper(char* mensaje){
	char** split = string_get_string_as_array(mensaje);
	t_hilo_mapper* hilo = calloc(1, sizeof(t_hilo_mapper));

	hilo->nro_bloque = atoi(split[0]);

	hilo->archivo_resultado = calloc(1, strlen(split[4]) + 1);
	strcpy(hilo->archivo_resultado, split[4]);
	hilo->socketNodo = conectarAServidor(split[2], atoi(split[3]));

	return hilo;
}

t_hilo_reduce* crearEstructuraHiloReduce(char* mensaje){
	char** split = string_get_string_as_array(mensaje);
	t_hilo_reduce* hilo = calloc(1, sizeof(t_hilo_mapper));
	hilo->socketNodo = conectarAServidor(split[1], atoi(split[2]));

	hilo->archivo_resultado = string_duplicate(split[3]);

	hilo->archivos_nodos = string_duplicate(split[4]);
	string_append(&hilo->archivos_nodos, ",");

	int i, hasta = atoi(split[4]) * 2;
	for(i = 0; i < hasta; i += 2){
		string_append(&hilo->archivos_nodos, split[i]);
		string_append(&hilo->archivos_nodos, ",");
		string_append(&hilo->archivos_nodos, split[i+i]);
		string_append(&hilo->archivos_nodos, ",");
	}

	return hilo;
}

void finish(){
	free(bytesMapperScript);
	free(bytesReduceScript);
	log_destroy(LOGGER);
	config_destroy(CONFIG);
}

char* getBytesFromScript(char *path, size_t* tam_archivo) {
	FILE* entrada;

	if((entrada = fopen(path, "r")) == NULL ){
		perror(path);
		return NULL;
	}

	fseek(entrada, 0L, SEEK_END);
	// es para ponerle el '\0' al final
	*tam_archivo = ftell(entrada)+1;
	char * literal = (char*) calloc(1, *tam_archivo);
	fseek(entrada, 0L, 0L);

	fread(literal, *tam_archivo, 1, entrada);
	return literal;
}
