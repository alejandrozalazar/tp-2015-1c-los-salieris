/*
 * planificador.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "marta.h"

void tratarMensaje(int numSocket, t_mensaje* mensaje);

void pingback(int numSocket);
void procesarArchivos(int socket, char* mensaje);

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

					t_mensaje* mensaje_recibido = NULL;

					recibirDeserializado(LOGGER, false, i, mensaje_recibido);

					tratarMensaje(i, mensaje_recibido);

					freeMensaje(mensaje_recibido);
				}
			}
		}
	}

}

// TODO: por cada switch del mensaje, deberia haber una funcion que la trate
void tratarMensaje(int numSocket, t_mensaje* mensaje){

	switch(mensaje->tipo){

		case JOB_TO_MARTA_HANDSHAKE: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		enviarSerializado(LOGGER, numSocket, false, MARTA_TO_JOB, 0, NULL);
		break;

		case JOB_TO_MARTA_FILES: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		procesarArchivos(numSocket, mensaje->contenido);
		break;

		case JOB_TO_NODO_REDUCE_REQUEST: log_info(LOGGER, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
		pingback(numSocket);
		break;

		default: log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  mensaje->tipo);

	}

}

void pingback(int numSocket){

}

void procesarArchivos(int socket, char* mensaje){

	char* split = string_split(mensaje, ",");
	t_list* lista_bloques = list_create();

	void obtenerBloquesArchivo(char* archivo){

		char* mensaje = string_duplicate(archivo);
		enviarSerializado(LOGGER, socketFS, true, MARTA_TO_FS_BUSCAR_ARCHIVO, strlen(mensaje)+1, mensaje);



	}

	string_iterate_lines(string_split(mensaje, ","), (void*)obtenerBloquesArchivo);

}
