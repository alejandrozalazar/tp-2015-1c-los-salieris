/*
 * mapper.c
 *
 *  Created on: 10/06/2015
 *      Author: utnso
 */
#include "job.h"

void mapper(t_hilo_mapper* hilo)
{
	log_info(LOGGER, "mapper->tid: %d. Comenzamos a ejecutar mapper()", hilo->tid);
	log_info(LOGGER, "mapper->tid: %d Intentamos conectarnos al nodo", hilo->tid);

	int socketNodo, ret;
	t_map_request map_request = hilo->map_request;
	ret = conectar(map_request.bloque_nodo.nodo.ip, map_request.bloque_nodo.nodo.puerto, &socketNodo);

	if(ret != EXITO){
		log_error(LOGGER, "mapper->tid: %d No pudimos conectarnos al nodo. Se notifica a MaRTA", hilo->tid);

		header_t header;
		memset(&header, 0, sizeof(header_t));
		header.tipo = JOB_TO_MARTA_MAP_ERROR;

		enviar_header(socketMarta, &header);
		enviar(socketMarta, (char*)&map_request, sizeof(t_map_request));

		// definir aca qué le mandamos a marta...
		pthread_exit(NULL);
	}
	log_info(LOGGER, "mapper->tid: %d Nos pudimos conectar al nodo con ip %s y puerto %d", hilo->tid
			, map_request.bloque_nodo.nodo.ip, map_request.bloque_nodo.nodo.puerto);

	header_t header;
	initHeader(&header);
	header.tipo = JOB_TO_NODO_MAP_REQUEST;
	header.cantidad_paquetes = 1;
	header.largo_mensaje = sizeof(t_map_request_nodo);

	log_info(LOGGER, "mapper->tid: %d Intentamos conectarnos al nodo", hilo->tid);
	enviar_header(socketNodo, &header);

	t_map_request_nodo request;
	request.nro_bloque = map_request.bloque_nodo.nro_bloque;
	request.tamanio_script = sizeMapperScript;
	strcpy(request.archivo_resultado, map_request.archivo_resultado);
	strcpy(request.nombre_script, config_get_string_value(CONFIG, "MAPPER"));

	enviar_map_request_nodo(socketNodo, &request);
	enviar(socketNodo, bytesMapperScript, sizeMapperScript);

	// recibimos respuesta del Nodo. pensar en la estructura....

	pthread_exit(NULL);


}
