/*
 * job.c
 *
 *  Created on: 10/06/2015
 *      Author: utnso
 */

#include "job.h"

int main(){

	init();

	int32_t id_proceso = process_getpid();

	system("clear");
	log_info(LOGGER, "************** Proceso Job (PID: %d) ***************\n"
		, id_proceso);

	if(conectar(config_get_string_value(CONFIG, "IP_MARTA"), config_get_int_value(CONFIG, "PUERTO_MARTA"), &socketMarta) != EXITO){
		log_error(LOGGER, "No Pude conectarme a MaRTA. Finalizo");
		return EXIT_FAILURE;
	}

	log_info(LOGGER, "Pude conectarme a MaRTA");

	enviarArchivosMarta();

	bool continuo = true;
	header_t header;

	while(continuo){

		if(recibir_header_simple(socketMarta, &header) != EXITO){
			log_error(LOGGER, "Se recibe un header vacio. MaRTA se cayó. Finaliza job......");
			continuo = false;
		}

		switch(header.tipo){

		case MARTA_TO_JOB_FILE_FOUND:
			notificar(header, MARTA_TO_JOB_FILE_FOUND);
			break;

		case MARTA_TO_JOB_FILE_NOT_FOUND:
			notificar(header, MARTA_TO_JOB_FILE_NOT_FOUND);
			break;

		case MARTA_TO_JOB_MAP_REQUEST:
			gestionar_map_request(header);
			break;

		case MARTA_TO_JOB_REDUCE_REQUEST:
			break;

		case MARTA_TO_JOB_ERROR: // puede ser que haya terminado bien o mal
			continuo = false;
			break;

		case MARTA_TO_JOB_JOB_FINISHED: // puede ser que haya terminado bien o mal
			continuo = false;
			break;

		default: log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  header);
		}


	}

	finish();
	return EXIT_SUCCESS;
}

void gestionar_map_request(header_t header)
{
	t_map_request map_request;

	if(recibir_map_request(socketMarta, &map_request) != EXITO){
		log_error(LOGGER, "Se desconectó el socket %d. Finalizamos Job", socketMarta);
		finish();
		exit(EXIT_FAILURE);
	}

	t_hilo_mapper hilo;
	hilo.map_request = map_request;
	pthread_create (&hilo.tid, NULL, (void*)mapper, (t_hilo_mapper*)&hilo);
}

void notificar(header_t header, t_header tipo_mensaje)
{
	char* mensaje_recibido = calloc(header.cantidad_paquetes, header.largo_mensaje);

	if(recibir_string(socketMarta, mensaje_recibido, header.largo_mensaje) != EXITO){
		log_error(LOGGER, "Se desconectó el socket %d. Finalizamos Job", socketMarta);
		finish();
		exit(EXIT_FAILURE);
	}

	log_info(LOGGER, "Mensaje recibido de MaRTA: %s-%s", getDescription(tipo_mensaje), mensaje_recibido);
	free(mensaje_recibido);
}

int enviarArchivosMarta(){

	header_t header;
	char* mensaje = config_get_string_value(CONFIG, "ARCHIVOS");
	int tamanio = strlen(mensaje)+1;

	initHeader(&header);
	header.tipo = JOB_TO_MARTA_FILES;
	header.largo_mensaje = tamanio;
	header.cantidad_paquetes = 1;

	log_info(LOGGER, "enviarArchivosMarta: mensaje %s sizeof(header): %d, largo mensaje: %d \n", getDescription(header.tipo), sizeof(header), header.largo_mensaje);

	if (enviar_header(socketMarta, &header) != EXITO)
	{
		log_error(LOGGER,"%s enviarArchivosMarta: Error al enviar header NUEVO_NIVEL\n\n", getDescription(header.tipo));
		return WARNING;
	}

//	recibir_header_simple(socketMarta, &header);
//	log_info(LOGGER, "recibo ack de Marta %s", getDescription(header.tipo));

	log_info(LOGGER, "vamos a enviar el string ahora %s", mensaje);
	if(enviar(socketMarta, mensaje, strlen(mensaje) + 1) != EXITO)
	{
		log_error(LOGGER,"enviarArchivosMarta: Error al enviar archivos\n\n");
		return WARNING;
	}

	return EXITO;
}

void init(){
	LOGGER = log_create(LOGGER_PATH, "job", true, LOG_LEVEL_DEBUG);

	if(LOGGER == NULL){
		puts("ERROR!!! No pudo levantar ni el archivo de log");
		exit(EXIT_FAILURE);
	}

	CONFIG = config_create(CONFIG_PATH);
	log_info(LOGGER, "Logger %s cargado correctamente.......", LOGGER_PATH);

	if (CONFIG == NULL || CONFIG->properties->elements_amount == 0) {
		log_error(LOGGER, "\nERROR AL LEVANTAR ARCHIVO DE CONFIGURACION\n( Don't PANIC! Si estas por consola ejecuta: ln -s ../%s %s )\n\n"
				, CONFIG_PATH
				, CONFIG_PATH);
		config_destroy(CONFIG);
		exit(EXIT_FAILURE);
	}
	log_info(LOGGER, "Configuración %s cargada correctamente.......", CONFIG_PATH);

	char keys[7][15] = {"IP_MARTA", "PUERTO_MARTA", "MAPPER", "REDUCE", "COMBINER", "ARCHIVOS", "RESULTADO"};

	int i;
	for(i = 0; i < 7; i++){
		if(!config_has_property(CONFIG, keys[i]) ){
			log_error(LOGGER, "La configuración no tiene la clave %s", keys[i]);
			exit(EXIT_FAILURE);
		}
	}

	if( (bytesMapperScript = getBytesFromScript(config_get_string_value(CONFIG, "MAPPER"), &sizeMapperScript) )== NULL){
		log_error(LOGGER, "No pudo abrirse el archivo %s para enviar al nodo", config_get_string_value(CONFIG, "MAPPER"));
		exit(EXIT_FAILURE);
	}
	log_info(LOGGER, "Scipt de mapper %s cargado correctamente.......", config_get_string_value(CONFIG, "MAPPER"));

	if( (bytesReduceScript = getBytesFromScript(config_get_string_value(CONFIG, "REDUCE"), &sizeReduceScript) )== NULL){
		log_error(LOGGER, "No pudo abrirse el archivo %s para enviar al nodo", config_get_string_value(CONFIG, "REDUCE"));
		exit(EXIT_FAILURE);
	}
	log_info(LOGGER, "Script de reduce %s cargado correctamente.......", config_get_string_value(CONFIG, "REDUCE"));

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

void finish(){
	free(bytesMapperScript);
	free(bytesReduceScript);
	log_destroy(LOGGER);
	config_destroy(CONFIG);
	log_info(LOGGER, "Estructuras liberadas correctamente!");
}
