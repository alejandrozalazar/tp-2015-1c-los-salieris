/*
 * fs_nodos.c
 *
 *  Created on: 07/06/2015
 *      Author: utnso
 */

#include "filesystem.h"

void tratarMensaje(int numSocket, t_mensaje* mensaje);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int fs_nodos(){

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
	socketInfo.sin_port = htons(7000);
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
						log_error(logger, string_from_format( "Hubo un error en el accept para el fd: %i", i));
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}

						//Shows the new connection administrated
						log_info(logger,
								string_from_format(
										"selectserver: new connection from %s on socket %d\n",
										inet_ntop(remoteaddr.ss_family,
												get_in_addr( (struct sockaddr*) &remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd));
					}

				} else {

					t_mensaje* mensaje_recibido = NULL;

					recibirDeserializado(logger, false, i, mensaje_recibido);

					tratarMensaje(i, mensaje_recibido);

					freeMensaje(mensaje_recibido);
				}
			}
		}
	}

	return EXIT_SUCCESS;
}

// TODO: por cada switch del mensaje, deberia haber una funcion que la trate
void tratarMensaje(int numSocket, t_mensaje* mensaje){
	log_info(logger, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);

	switch(mensaje->tipo){

		case JOB_TO_MARTA_FILES:
//		crearEstructuraJob(numSocket, mensaje->contenido);
//		mandarAMappear();
		break;

		case JOB_TO_MARTA_MAP_OK:
//		pingback(numSocket);
		break;

		case JOB_TO_MARTA_MAP_ERROR:
//		pingback(numSocket);
		break;

		case JOB_TO_MARTA_REDUCE_OK:
//		pingback(numSocket);
		break;

		case JOB_TO_MARTA_REDUCE_ERROR:
//		pingback(numSocket);
		break;

		default: log_error(logger, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  mensaje->tipo);

	}
}
