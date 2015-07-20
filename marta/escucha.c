/*
 * planificador.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "marta.h"

void job_agregar(int fdJob, header_t header);
t_job* job_buscar_por_fd(int fd);
void job_eliminar(int fdJob, header_t header);
void job_liberar(t_job* job);
void job_archivo_agregar(int fd, t_archivo_job* archivo_job, char* nombre_archivo);
void job_archivo_liberar(t_archivo_job* archivo_job);
t_bloque_archivo *job_archivo_obtener_bloques(char* nombre_archivo, header_t *header);

void notificarMapOk(int fdJob, header_t header);
void notificarMapError(int fdJob, header_t header);
void notificarReduceOk(int fdJob, header_t header);
void notificarReduceError(int fdJob, header_t header);
void pingback(int numSocket);
void procesarArchivos(int socketJob, header_t header);
void loggear_bloque(t_bloque_archivo bloque);
void loggear_nodo(t_nodo nodo);
void planificarMapRequests(t_bloque_archivo* bloques, int tamanio, char* nombre_archivo, int socketJob);
int buscarNodoMinimo(t_bloque_nodo bloques_nodo[3]);
void registrarNodos(t_bloque_nodo bloques_nodo[3]);
t_bloque_archivo buscar_bloque_archivo(int fdJob, t_nombre archivo_origen, int nro_bloque);

char* get_filename(char* nombre);

void escucha(int puerto) {

	int myPID = process_get_thread_id();
	log_info(LOGGER, "************** Comienza a escuchar MaRTA a los Jobs (PID: %d) ***************",myPID);

	//Logica principal para administrar conexiones
	fd_set master; //file descriptor list
	fd_set read_fds; //file descriptor list temporal para el select()
	int fdmax; //maximo numero de file descriptor para hacer las busquedas en comunicaciones

	int listener;//socket escucha
	int newfd;//file descriptor del cliente aceptado
	struct sockaddr_storage remoteaddr; //dirección del cliente
	socklen_t addrlen;

	char remoteIP[INET6_ADDRSTRLEN];

	int i; // socket entrante

	struct addrinfo hints;

	FD_ZERO(&master);	//clear the master
	FD_ZERO(&read_fds); //clear the temp set

	 //Lleno la estructura de tipo addrinfo
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	t_socket_info socketInfo;
	socketInfo.sin_addr.s_addr = INADDR_ANY;
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_port = htons(puerto);
	memset(&(socketInfo.sin_zero), '\0', 8);

	listener = crearSocket();

	bindearSocket(listener, socketInfo);

	// listen turns on server mode for a socket.
	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	// add the listener to the master set
	FD_SET(listener, &master);

	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	for(;;){

		read_fds = master; // copy it

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) { // we got one!!

				if (i == listener) {

					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *) &remoteaddr,
							&addrlen);

					if (newfd == -1) {
						log_error(LOGGER, string_from_format( "Hubo un error en el accept para el fd: %i", i));
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}

						//Shows the new connection administrated
						log_info(LOGGER,
								string_from_format(
										"selectserver: new connection from %s on socket %d\n",
										inet_ntop(remoteaddr.ss_family,
												get_in_addr( (struct sockaddr*) &remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd));
					}

				} else {

					header_t header;
					initHeader(&header);

					if(recibir_header_simple(i, &header) != EXITO){
						log_error(LOGGER, "Error al recibir header. Considero que viene de un cliente que se cayó");
						header.tipo = ERR_CONEXION_CERRADA;
					}

					switch(header.tipo){

						case ERR_CONEXION_CERRADA:
							close(i);
							FD_CLR(i, &master);
							job_eliminar(i, header);
							break;

						case JOB_TO_MARTA_FILES:
							// agrego el job a la lista sincronizada, gestiono las estructuras y blah
							job_agregar(i, header);
							// le pido al fs los bloques de los archivos del job y actualizo el job de la lista
							procesarArchivos(i, header);
							break;

						case JOB_TO_MARTA_MAP_OK:
							notificarMapOk(i, header);
							break;

						case JOB_TO_MARTA_MAP_ERROR:
							notificarMapError(i, header);
							break;

						case JOB_TO_MARTA_REDUCE_OK:
							notificarReduceOk(i, header);
							break;

						case JOB_TO_MARTA_REDUCE_ERROR:
							notificarReduceError(i, header);
							break;

						default: log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  header);
					}
				}
			}
		}
	}
}

void job_liberar(t_job* job){
	dictionary_destroy_and_destroy_elements(job->archivos, (void*)job_archivo_liberar);
	free(job);
}

void job_archivo_liberar(t_archivo_job* archivo_job){
	free(archivo_job);
}

// ver de notificar que elimino un job, blah
void job_eliminar(int fdJob, header_t header){
	bool buscarJobPorFd(void* element){
		return ((t_job*)element)->fd == fdJob;
	}
	pthread_mutex_lock(&mutex_lista_jobs);
	list_remove_and_destroy_by_condition(lista_jobs, (void*)job_buscar_por_fd, (void*)job_liberar);
	pthread_mutex_unlock(&mutex_lista_jobs);

}

// ver de notificar que agregar un job, blah
void job_agregar(int fdJob, header_t header){
	t_job *job = calloc(1, sizeof(t_job));
	job->fd = fdJob;
	job->archivos = dictionary_create();
	pthread_mutex_lock(&mutex_lista_jobs);
	list_add(lista_jobs, job);
	pthread_mutex_unlock(&mutex_lista_jobs);
}

void pingback(int numSocket){

}

/**
 * verificar retorno de cada uno de los send()/receive()
 */
void procesarArchivos(int socketJob, header_t header){

	char* mensaje = calloc(1, header.largo_mensaje);
//	enviar_ack(socketJob);
	recibir_string(socketJob, mensaje, header.largo_mensaje);

	void procesarArchivo(char* archivo){
		header_t header;

		t_archivo_job *archivo_job = calloc(1, sizeof(t_archivo_job));
		archivo_job->reducido = false;
		t_bloque_archivo* bloques = job_archivo_obtener_bloques(archivo, &header);
		archivo_job->size_vec_bloques = header.cantidad_paquetes;

		if(bloques == NULL){
			log_error(LOGGER, "Error al recibir los bloques del archivo %s", archivo);
			free(archivo_job);
			header.tipo = MARTA_TO_JOB_FILE_ERROR;
			enviar_header(socketJob, &header);
			return;
		}

		if(archivo_job->size_vec_bloques == 0){
			log_error(LOGGER, "No se encontró el archivo %s en el FileSystem", archivo);
			free(archivo_job);
			header.tipo = MARTA_TO_JOB_FILE_NOT_FOUND;
			enviar_header(socketJob, &header);
			return;
		}

		job_archivo_agregar(socketJob, archivo_job, archivo);

		// buscar job por fd
		planificarMapRequests(bloques, archivo_job->size_vec_bloques, archivo, socketJob);
	}

	string_iterate_lines(string_get_string_as_array(mensaje), (void*)procesarArchivo);
	free(mensaje);
}

// si hubo algun error, devolver NULL. validar!
t_bloque_archivo *job_archivo_obtener_bloques(char* nombre_archivo, header_t *header){
	log_info(LOGGER, "pido bloques del archivo al FS");
	enviar_t_mensaje(socketFS, MARTA_TO_FS_BUSCAR_ARCHIVO, strlen(nombre_archivo) + 1, nombre_archivo);

	if(recibir_header_simple(socketFS, header) != EXITO){
		log_error(LOGGER, "Error al recibir header del FS %s", nombre_archivo);
		return NULL;
	}
	if(header->cantidad_paquetes == 0){
		log_error(LOGGER, "Parece que no se encontró el archivo %s", nombre_archivo);
		return NULL;
	}
	t_bloque_archivo *bloques = calloc(header->cantidad_paquetes, sizeof(t_bloque_archivo));

	if(recibir(socketFS, (char*)bloques, sizeof(t_bloque_archivo) * header->cantidad_paquetes) != EXITO){
		log_error(LOGGER, "Error al recibir los bloques del archivo %s", nombre_archivo);
		free(bloques);
		return NULL;
	}
	return bloques;
}

void loggear_bloque(t_bloque_archivo bloque){
	log_info(LOGGER, "\nt_bloque_archivo recibido\nnro_bloque: %d\narchivo:tmp: %s\nmapeado?: %d", bloque.nro_bloque, bloque.archivo_tmp, bloque.mapeado);
	int i;
	for(i = 0; i < 3; i++){
		loggear_nodo(bloque.bloques_nodo[i].nodo);
		log_info(LOGGER, "nro de bloque en nodo %d", bloque.bloques_nodo[i].nro_bloque);
	}
}

void loggear_nodo(t_nodo nodo){
	log_info(LOGGER, "nodo: %s ip: %s puerto: %d", nodo.nombre, nodo.ip, nodo.puerto);
}

void planificarMapRequests(t_bloque_archivo* bloques, int tamanio, char* nombre_archivo, int socketJob){

	int i;
	for(i = 0; i < tamanio; i++){
		header_t header;
		t_bloque_archivo bloque = bloques[i];

		loggear_bloque(bloques[i]);

		pthread_mutex_lock(&mutex_mapa_nodos);
		// registros todos los nodos que recibo del FS
		registrarNodos(bloque.bloques_nodo);

		// busco el nodo con menor carga de los 3 que tengo
		int n = buscarNodoMinimo(bloque.bloques_nodo);

		// incremento la carga del nodo
		t_nodo *nodo = dictionary_get(mapa_nodos, bloque.bloques_nodo[n].nodo.nombre);
		(nodo->carga)++;
		pthread_mutex_unlock(&mutex_mapa_nodos);

		if(n == -1){
			log_error(LOGGER, "El bloque %d del archivo %s que se busca no está disponible", i, nombre_archivo);
			header.tipo = MARTA_TO_JOB_ERROR;
			enviar_header(socketJob, &header);
			continue;
		} else {
			header.tipo = MARTA_TO_JOB_MAP_REQUEST;
		}

		t_map_request request;
		strcpy(request.archivo_origen, nombre_archivo);
		strcpy(request.archivo_resultado, get_filename(nombre_archivo));
		request.bloque_nodo = bloque.bloques_nodo[n];

		enviar_header(socketJob, &header);
		enviar_map_request(socketJob, &request);
	}

}

void registrarNodos(t_bloque_nodo bloques_nodo[3]){
	int i;
	for(i = 0; i < 3; i++){
		t_nodo nodo_bloque = bloques_nodo[i].nodo;
		t_nodo *nodo_mapa = dictionary_get(mapa_nodos, nodo_bloque.nombre);
		if(nodo_mapa == NULL){
			nodo_mapa = calloc(1, sizeof(t_nodo));
			nodo_mapa->carga = 0; // bloques_nodo[i].nodo.carga debería ser siempre 0, porque me lo mandan por 1era vez
			nodo_mapa->disponible = nodo_bloque.disponible;
			nodo_mapa->puerto = nodo_bloque.puerto;
			strcpy(nodo_mapa->ip, nodo_bloque.ip);
			strcpy(nodo_mapa->nombre, nodo_bloque.nombre);

			dictionary_put(mapa_nodos, nodo_mapa->nombre, nodo_mapa);
		}
	}
}

// TODO: la hace Eze
// devolver el indice del nodo que tenga la menor carga
int buscarNodoMinimo(t_bloque_nodo bloques_nodo[3]){



	return 0;
}

void job_archivo_agregar(int fd, t_archivo_job* archivo_job, char* nombre_archivo){
	pthread_mutex_lock(&mutex_lista_jobs);
	t_job* job = job_buscar_por_fd(fd);
	dictionary_put(job->archivos, nombre_archivo, archivo_job);
	pthread_mutex_unlock(&mutex_lista_jobs);
}

t_job* job_buscar_por_fd(int fd){
	bool buscarJobPorFd(void* element){
		return ((t_job*)element)->fd == fd;
	}
	return list_find(lista_jobs, (void*)buscarJobPorFd);
}


void notificarMapOk(int fdJob, header_t header){

	t_map_request map_request;
	recibir_map_request(fdJob, &map_request);
	log_info(LOGGER, "Se recibe request del nodo %s", map_request.bloque_nodo.nodo.nombre);

	pthread_mutex_lock(&mutex_mapa_nodos);
	int *carga = dictionary_get(mapa_nodos, map_request.bloque_nodo.nodo.nombre);
	(*carga)--;
	pthread_mutex_unlock(&mutex_mapa_nodos);

	t_job* job = job_buscar_por_fd(fdJob);
	t_archivo_job* archivo = dictionary_get(job->archivos, map_request.archivo_origen);
	(archivo->cant_mapeados)++;

	if(archivo->cant_mapeados == archivo->size_vec_bloques){
		memset(&header, 0, sizeof(header_t));
		header.tipo = MARTA_TO_JOB_REDUCE_REQUEST;

	}
}

void notificarMapError(int fdJob, header_t header){
	t_map_request map_request;
	recibir(fdJob, (char*)&map_request, sizeof(t_map_request));

	t_bloque_archivo bloque = buscar_bloque_archivo(fdJob, map_request.archivo_origen, map_request.bloque_archivo);

	int n = buscarNodoMinimo(bloque.bloques_nodo);
	t_bloque_nodo bloque_nodo = bloque.bloques_nodo[n];
	map_request.bloque_nodo.nodo = bloque_nodo.nodo;

	initHeader(&header);
	header.tipo = MARTA_TO_JOB_MAP_REQUEST;
	enviar_header(fdJob, &header);
	enviar_map_request(fdJob, &map_request);
}

t_bloque_archivo buscar_bloque_archivo(int fdJob, t_nombre archivo_origen, int nro_bloque){

	t_bloque_archivo bloque;
	header_t header;
	initHeader(&header);
	header.tipo = MARTA_TO_FS_BUSCAR_BLOQUE_ARCHIVO;

	enviar_header(socketFS, &header);
	char* mensaje = string_from_format("[%s,%d]", archivo_origen, nro_bloque);
	enviar(fdJob, mensaje, strlen(mensaje) + 1);

	recibir(socketFS, (char*)&bloque, sizeof(t_bloque_archivo));

	return bloque;
}

void notificarReduceOk(int fdJob, header_t header){
	notificarMapOk(fdJob, header);
}

// matamos el job
void notificarReduceError(int fdJob, header_t header){
	header_t header_a_job;
	initHeader(&header_a_job);
	header_a_job.tipo = MARTA_TO_JOB_ERROR;
	enviar_header(fdJob, &header);
}

char* get_filename(char* nombre){
	char* ret = string_duplicate(nombre);
	char* tiempo = temporal_get_string_time_temp();
	string_append(&ret, "-");
	string_append(&ret, tiempo);
	string_append(&ret, ".tmp");
	free(tiempo);
	return ret;
}
