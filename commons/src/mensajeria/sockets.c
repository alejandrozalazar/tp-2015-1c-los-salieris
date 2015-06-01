#include "sockets.h"

static char* serializarMensaje(t_header header, size_t tamanio, char* contenido);

int crearSocket() {

	int newSocket;
	int si = 1;
	if ((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return EXIT_FAILURE;
	} else {
		if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &si, sizeof(int))
				== -1) {
			return EXIT_FAILURE;
		}
		return newSocket;
	}
}

int bindearSocket(int newSocket, tSocketInfo socketInfo) {
	if (bind(newSocket, (struct sockaddr*) &socketInfo, sizeof(socketInfo))
			== -1) {
		perror("Error al bindear socket escucha");
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}

int escucharEn(int newSocket) {
	if (listen(newSocket, BACKLOG) == -1) {
		perror("Error al poner a escuchar socket");
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}

int conectarAServidor(char *ipDestino, unsigned short puertoDestino) {
	int socketDestino;
	tSocketInfo infoSocketDestino;
	infoSocketDestino.sin_family = AF_INET;
	infoSocketDestino.sin_port = htons(puertoDestino);
	inet_aton(ipDestino, &infoSocketDestino.sin_addr);
	memset(&(infoSocketDestino.sin_zero), '\0', 8);

	if ((socketDestino = crearSocket()) == EXIT_FAILURE) {
		perror("Error al crear socket");
		return -1;
	}

	if (connect(socketDestino, (struct sockaddr*) &infoSocketDestino,
			sizeof(infoSocketDestino)) == -1) {
		perror("Error al conectar socket");
		close(socketDestino);
		return -1;
	}

	return socketDestino;
}

int desconectarseDe(int socket) {
	if (close(socket)) {
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}

int32_t enviarMensaje(int32_t numSocket, t_header header, t_contenido mensaje,
		t_log *logger) {

	puts("llego aca?");
	if (strlen(mensaje) > sizeof(t_contenido)) {
		log_error(logger,
				"Error en el largo del mensaje, tiene que ser menor o igual al máximo: %s",
				MSG_SIZE);
		return -1;
	}

	t_mensajes *s = malloc(sizeof(t_mensajes));
	s->id = header;
	strcpy(s->contenido, mensaje);
	log_info(logger, "Se ENVIA por SOCKET:%d - HEAD:%s MENSAJE:\"%s\" ",
			numSocket, getDescription(s->id), s->contenido);
	int n = send(numSocket, s, sizeof(*s), 0);
	if (n != STRUCT_SIZE) {
		log_error(logger,
				"#######################################################################");
		log_error(logger,
				"##    ERROR en el envío de mensajes: no se envió todo. socket: %d    ##",
				numSocket);
		log_error(logger,
				"#######################################################################");
	}
	free(s);
	return n;
}

t_header recibirMensaje(int numSocket, t_contenido mensaje, t_log *logger) {

	char buffer[STRUCT_SIZE];

	log_debug(logger, "Estoy en recv");
	int n = recv(numSocket, buffer, STRUCT_SIZE, 0);

	if (n == STRUCT_SIZE) {
		t_mensajes* s = (t_mensajes*) buffer;
		strcpy(mensaje, s->contenido);
		log_debug(logger,
				"Se RECIBE por SOCKET:%d - HEAD:%s%s%s MENSAJE:\"%s%s%s\" ",
				numSocket, getDescription(s->id), mensaje);
		return s->id;
	} else {
		if (n == 0) { // Conexión remota cerrada
			log_info(logger, "El socket %d cerró la conexion.", numSocket);
			strcpy(mensaje, "");
			return ERR_CONEXION_CERRADA;
		} else { // El mensaje tiene un tamaño distinto al esperado
			log_error(logger,
					"#######################################################");
			log_error(logger,
					"##    ERROR en el recibo de mensaje del socket %d    ##",
					numSocket);
			log_error(logger,
					"#######################################################");
			strcpy(mensaje, "");
			//usleep(500000);
			return ERR_ERROR_AL_RECIBIR_MSG;
		}
	}
}

int cerrarSocket(int numSocket, fd_set *fd) {

	close(numSocket);

	if (fd != NULL ) {
		FD_CLR(numSocket, fd);
	}
	return EXIT_SUCCESS;
}

void freeMensaje(t_mensaje* mensaje){
	free(mensaje->contenido);
	free(mensaje);
}

char* getDescription(int item) {

	switch (item) {
	case ERR_CONEXION_CERRADA:
		return "ERR_CONEXION_CERRADA  ";
	case ERR_ERROR_AL_RECIBIR_MSG:
		return "ERR_ERROR_AL_RECIBIR_MSG ";
	case ERR_ERROR_AL_ENVIAR_MSG:
		return "ERR_ERROR_AL_ENVIAR_MSG ";

	case JOB_TO_MARTA_HANDSHAKE:
		return "JOB_TO_MARTA_HANDSHAKE ";
	case JOB_TO_NODO_MAP_REQUEST:
		return "JOB_TO_NODO_MAP_REQUEST ";
	case JOB_TO_NODO_REDUCE_REQUEST:
		return "JOB_TO_NODO_REDUCE_REQUEST ";

	case MARTA_TO_JOB_MAP_REQUEST:
		return "MARTA_TO_JOB_MAP_REQUEST ";
	case MARTA_TO_JOB_REDUCE_REQUEST:
		return "MARTA_TO_JOB_REDUCE_REQUEST ";
	case MARTA_TO_JOB:
		return "MARTA_TO_JOB ";

	case NODO_TO_JOB_MAP_OK:
		return "NODO_TO_JOB_MAP_OK ";
	case NODO_TO_JOB_MAP_KO:
		return "NODO_TO_JOB_MAP_KO ";
	case NODO_TO_JOB_REDUCE_OK:
		return "NODO_TO_JOB_REDUCE_OK ";
	case NODO_TO_JOB_REDUCE_KO:
		return "NODO_TO_JOB_REDUCE_KO ";
	case NODO_TO_NODO_GET_BLOQUE:
		return "NODO_TO_NODO_GET_BLOQUE ";
	case NODO_TO_NODO_SET_BLOQUE:
		return "NODO_TO_NODO_SET_BLOQUE ";
	case NODO_TO_NODO_GET_FILE_CONTENT:
		return "NODO_TO_NODO_GET_FILE_CONTENT ";
	case FS_TO_NODO_GET_BLOQUE:
		return "FS_TO_NODO_GET_BLOQUE ";
	case FS_TO_NODO_SET_BLOQUE:
		return "FS_TO_NODO_SET_BLOQUE ";
	case FS_TO_NODO_GET_FILE_CONTENT:
		return "FS_TO_NODO_GET_FILE_CONTENT ";
	case FIN:
		return "FIN ";
	default:
		return "---DEFAULT--- (mensaje sin definir)";
	}
	return "";

}

int enviar(int sock, char *buffer, int tamano) {
	int escritos;

	if ((escritos = send(sock, buffer, tamano, 0)) <= 0) {
		printf("Error en el send\n\n");
		return WARNING;
	} else if (escritos != tamano) {
		printf(
				"La cantidad de bytes enviados es distinta de la que se quiere enviar\n\n");
		return WARNING;
	}

	return EXIT_SUCCESS;
}

int recibir(int sock, void *buffer, int tamano) {
	int val;
	int leidos = 0;

	memset(buffer, '\0', tamano);

	while (leidos < tamano) {

		val = recv(sock, buffer + leidos, tamano - leidos, 0);
		leidos += val;
		if (val < 0) {
			printf("Error al recibir datos: %d - %s\n", val, strerror(val)); //ENOTCONN ENOTSOCK
			switch (val) {
			// The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.  POSIX.1-2001 allows either error to be returned for this
			// case, and does not require these constants to have the same value, so a portable application should check for both possibilities.
			//case EAGAIN: printf(" - EAGAIN \n The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.\n\n"); break;
			//case EWOULDBLOCK: printf("EWOULDBLOCK \n The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.\n\n"); break;
			// The argument sockfd is an invalid descriptor.
			case EBADF:
				printf(
						"EBADF \n The argument sockfd is an invalid descriptor.\n\n");
				break;
				// A remote host refused to allow the network connection (typically because it is not running the requested service).
			case ECONNREFUSED:
				printf(
						"ECONNREFUSED \n A remote host refused to allow the network connection (typically because it is not running the requested service).\n\n");
				break;
				// The receive buffer pointer(s) point outside the process's address space.
			case EFAULT:
				printf(
						"EFAULT \n The receive buffer pointer(s) point outside the process's address space.\n\n");
				break;
				// The receive was interrupted by delivery of a signal before any data were available; see signal(7).
			case EINTR:
				printf(
						"EINTR \n The receive was interrupted by delivery of a signal before any data were available; see signal(7).\n\n");
				break;
				// Invalid argument passed.
			case EINVAL:
				printf("EINVAL \n Invalid argument passed.\n\n");
				break;
				// Could not allocate memory for recvmsg().
			case ENOMEM:
				printf(
						"ENOMEM \n Could not allocate memory for recvmsg().\n\n");
				break;
				// The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).
			case ENOTCONN:
				printf(
						"ENOTCONN \n The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).\n\n");
				break;
				// The argument sockfd does not refer to a socket.
			case ENOTSOCK:
				printf(
						"ENOTSOCK \n The argument sockfd does not refer to a socket.\n\n");
				break;

			}
			return EXIT_FAILURE;
		}
		// Cuando recv devuelve 0 es porque se desconecto el socket.
		if (val == 0) {
			/*printf("%d se desconecto\n", sock);*/
			return WARNING;
		}
	}
	return EXIT_FAILURE;
}

int enviarSerializado(t_log* logger, int socket, bool esTexto, t_header header, size_t tamanio, char* contenido){

	int ret;
	log_debug(logger, "Enviando al socket %d, header %s, tamanio %d", socket, getDescription(header), tamanio);
	if(esTexto){
		log_debug(logger, "Mensaje: [%s]", contenido);
	}

	char* payload = serializarMensaje(header, tamanio, contenido);
	ret = enviar(socket, payload, tamanio);
	free(payload);

	return ret;
}

static char* serializarMensaje(t_header header, size_t tamanio, char* contenido) {

	char *serializedPackage = calloc(1,
			sizeof(t_header) + sizeof(size_t)
					+ tamanio);
	int offset = 0, size_to_send;

	size_to_send = sizeof(t_header);
	memcpy(serializedPackage + offset, &header, size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(size_t);
	memcpy(serializedPackage + offset, &tamanio, size_to_send);
	offset += size_to_send;

	if(tamanio == 0){
		return serializedPackage;
	}

	size_to_send = tamanio;
	memcpy(serializedPackage + offset, contenido, size_to_send);

	return serializedPackage;
}

t_header recibirDeserializado(t_log *logger, int socketCliente, bool esTexto, t_mensaje* package) {
	package = calloc(1,sizeof(t_mensaje));
	int status;

	status = recibir(socketCliente, &(package->tipo), sizeof(t_header));
	if (!status)
		return ERR_ERROR_AL_RECIBIR_MSG;
	status = recibir(socketCliente, &(package->tamanio), sizeof(size_t));

	// en caso de que tamanio sea 0 es porque es un handshake
	if(package->tamanio == 0){
		log_debug(logger, "Mensaje recibido:[%s,%d]", getDescription(package->tipo), package->tamanio);
		return package->tipo;
	}
	if (!status)
		return ERR_ERROR_AL_RECIBIR_MSG;
	package->contenido = calloc(1,package->tamanio);
	status = recibir(socketCliente, package->contenido, package->tamanio);
	if (!status)
		return ERR_ERROR_AL_RECIBIR_MSG;
	log_debug(logger, "Mensaje recibido:[%s,%d,s%]", getDescription(package->tipo), package->tamanio, esTexto ? package->contenido : "{CONTENIDO BINARIO}");
	return package->tipo;
}
