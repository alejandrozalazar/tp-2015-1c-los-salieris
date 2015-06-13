/*
 * principal.c
 *
 *  Created on: 6/6/2015
 *      Author: Alejandro Zalazar
 */
#include "principal.h"

t_estado* estado;

int ejecutarProgramaPrincipal(t_estado* estado) {

	t_log* logger = estado->logger;
	log_info(logger, "Inicializado");

	//todonodo conectar a filesystem
//	if(conectarAFileSystem(estado) > 0) {
//		//todonodo escuchar respuesta filesystem
//		//si la respuesta es Ok comenzar a escuchar filesystem (otra conexion?), nodos, hilos mapper y reducer
//
//	} else {
//		//si la respuesta es NO OK o hubo algun problema, loggear y salir
//		log_info(logger, "No se pudo conectar al filesystem %s:%d", estado->conf->IP_FS, estado->conf->PUERTO_FS);
//	}

	if(escuchar(estado) < 0)
	{
		log_info(logger, "No se pudo escuchar el puerto configurado");
	}

	log_info(logger, "Finalizando");

	log_info(logger, "Exit");

	return 0;
}

int escuchar(t_estado* estado, t_log* logger){
	bool termina = false;
	struct sockaddr_in my_addr, their_addr;
	int socketFD, socketCliente;
	socketFD = crearSocket();

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(estado->conf->PUERTO_NODO);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', sizeof(struct sockaddr_in));

	bindearSocket(socketFD, my_addr);

	escucharEn(socketFD);
	socklen_t sin_size;
	sin_size = sizeof(struct sockaddr_in);

	if((socketCliente = accept(socketFD,(struct sockaddr *)&their_addr, &sin_size)) == -1)
	{
		perror("accept");
		return -1;
	}

	log_trace(logger, "Recibí conexion de %s", inet_ntoa(their_addr.sin_addr));

	while(!termina){
		header_t mensaje;
		recibir_header_simple(socketFD, &mensaje);
		header_t* pMensaje = &mensaje;

		log_debug(logger, "Recibi un header tipo: %d, tamanio: %d", pMensaje->tipo, pMensaje->largo_mensaje);

		if(pMensaje->tipo == JOB_TO_NODO_MAP_REQUEST) {
			t_map_request_nodo mapRequestNodo;
			bool isDesconecto;
			recibir_map_request_nodo(socketFD, &mapRequestNodo, &isDesconecto);
		} else {
			log_debug(logger, "Mensaje no soportado");
		}

//		if(header == JOB_TO_MARTA_FILES){
//			puts("llegamos! y respondemos!");
//			size_t tamanio = 4 * sizeof(size_t) + 9 * sizeof(t_bloque_nodo);
//			char* payload = serializarContenido(tamanio);
//			enviarSerializado(logger, socketCliente, false, MARTA_TO_JOB_FILE_FOUND, tamanio, payload);
//
//		}


		termina = false;

	}
}
