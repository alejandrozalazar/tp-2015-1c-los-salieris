/*
 * planificador.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "marta.h"

void eliminarJob(int fdJob, header_t header);
void agregarJob(int fdJob, header_t header);
void notificarMapOk(int fdJob, header_t header);
void notificarMapError(int fdJob, header_t header);
void notificarReduceOk(int fdJob, header_t header);
void notificarReduceError(int fdJob, header_t header);
void pingback(int numSocket);
void procesarArchivos(int socket, header_t header);

t_bloque_archivo generar_bloque_archivo(int i);
char* serializarContenido(size_t cantidad, size_t tamanio);
t_nodo generar_nodo(int  i);
char* get_filename(char* nombre);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

void escucha(int puerto) {

	int myPID = process_get_thread_id();
	log_info(LOGGER, "************** Comienza el planificador!(PID: %d) ***************",myPID);

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
					if(recibir_header_simple(i, &header) != EXITO){
						log_error(LOGGER, "Se recibe un header vacio. MaRTA se cayó. Finaliza job......");
					}

					switch(header.tipo){

					case ERR_CONEXION_CERRADA:
						close(i);
						FD_CLR(i, &master);
						eliminarJob(i, header);
						break;

					case JOB_TO_MARTA_FILES:
						// agrego el job a la lista sincronizada, gestiono las estructuras y blah
						agregarJob(i, header);
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

void eliminarJob(int fdJob, header_t header){

}

void agregarJob(int fdJob, header_t header){

}

void pingback(int numSocket){

}

void procesarArchivos(int socket, header_t header){

}

void notificarMapOk(int fdJob, header_t header){

}

void notificarMapError(int fdJob, header_t header){

}

void notificarReduceOk(int fdJob, header_t header){

}

void notificarReduceError(int fdJob, header_t header){

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
