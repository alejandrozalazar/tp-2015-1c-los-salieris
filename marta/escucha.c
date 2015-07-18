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

t_bloque_archivo generar_bloque_archivo(int i);
char* serializarContenido(size_t cantidad, size_t tamanio);
t_nodo generar_nodo(int  i);
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
	free(archivo_job->vec_bloques);
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
		archivo_job->vec_bloques = job_archivo_obtener_bloques(archivo, &header);
		archivo_job->size_vec_bloques = header.cantidad_paquetes;

		if(archivo_job->vec_bloques == NULL){
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
		planificarMapRequests(archivo_job->vec_bloques, archivo_job->size_vec_bloques, archivo, socketJob);
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
}

void notificarMapError(int fdJob, header_t header){
	// teoricamente, hay que elegir otro nodo y mandar a ejecutar el hilo
}

void notificarReduceOk(int fdJob, header_t header){
	notificarMapOk(fdJob, header);
}

void notificarReduceError(int fdJob, header_t header){
	// teoricamente, hay que elegir otro nodo y mandar a ejecutar el hilo
}

char* serializarContenido(size_t cantidad, size_t tamanio){
	char *serializedPackage = malloc(tamanio);
	t_bloque_archivo* bloques = calloc(1, sizeof(t_bloque_archivo));


	int offset = 0;
	int size_to_send;

	size_to_send =  sizeof(size_t);
	memcpy(serializedPackage + offset, &cantidad, size_to_send);
	offset += size_to_send;

	int i;
	for(i = 0; i < cantidad; i++){
		bloques[i] = generar_bloque_archivo(i);
	}

	size_to_send =  sizeof(size_t);
	memcpy(serializedPackage + offset, &cantidad, size_to_send);
	offset += size_to_send;

	size_to_send =  cantidad * sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, bloques, size_to_send);
	offset += size_to_send;

	return serializedPackage;
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

t_bloque_archivo generar_bloque_archivo(int i){
	t_bloque_archivo bloque;
	strcpy(bloque.archivo_tmp, get_filename("job"));
	bloque.mapeado = false;
	bloque.nro_bloque = i;

	switch(i){

	case 0:
		bloque.bloques_nodo[0].nodo = generar_nodo(1);
		bloque.bloques_nodo[0].nro_bloque = 6;
		bloque.bloques_nodo[1].nodo = generar_nodo(2);
		bloque.bloques_nodo[1].nro_bloque = 9;
		bloque.bloques_nodo[2].nodo = generar_nodo(3);
		bloque.bloques_nodo[2].nro_bloque = 11;
	break;
	case 1:
		bloque.bloques_nodo[0].nodo = generar_nodo(4);
		bloque.bloques_nodo[0].nro_bloque = 6;
		bloque.bloques_nodo[1].nodo = generar_nodo(5);
		bloque.bloques_nodo[1].nro_bloque = 9;
		bloque.bloques_nodo[2].nodo = generar_nodo(1);
		bloque.bloques_nodo[2].nro_bloque = 11;
	break;
	case 2:
		bloque.bloques_nodo[0].nodo = generar_nodo(4);
		bloque.bloques_nodo[0].nro_bloque = 7;
		bloque.bloques_nodo[1].nodo = generar_nodo(3);
		bloque.bloques_nodo[1].nro_bloque = 4;
		bloque.bloques_nodo[2].nodo = generar_nodo(4);
		bloque.bloques_nodo[2].nro_bloque = 33;
	break;
	}
	return bloque;
}

t_nodo generar_nodo(int  i){
	t_nodo nodo;
	switch(i){
		case 1:
			strcpy(nodo.nombre, "Nodo1");
			strcpy(nodo.ip, "127.0.0.1");
			nodo.puerto = 5001;
			nodo.disponible = true;
			break;
		case 2:
			strcpy(nodo.nombre, "Nodo2");
			strcpy(nodo.ip, "127.0.0.1");
			nodo.puerto = 5002;
			nodo.disponible = true;
			break;
		case 3:
			strcpy(nodo.nombre, "Nodo3");
			strcpy(nodo.ip, "127.0.0.1");
			nodo.puerto = 5003;
			nodo.disponible = true;
			break;
		case 4:
			strcpy(nodo.nombre, "Nodo4");
			strcpy(nodo.ip, "127.0.0.1");
			nodo.puerto = 5004;
			nodo.disponible = true;
			break;
		case 5:
			strcpy(nodo.nombre, "Nodo5");
			strcpy(nodo.ip, "127.0.0.1");
			nodo.puerto = 5005;
			nodo.disponible = true;
			break;
	}
	return nodo;
}
