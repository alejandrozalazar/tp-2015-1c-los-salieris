#include "sockets.h"

int crearSocket() {

	int newSocket;
	int si = 1;
	if ((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return EXIT_FAILURE;
	} else {
		if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &si, sizeof(int)) == -1) {
			return EXIT_FAILURE;
		}
		return newSocket;
	}
}

int bindearSocket(int newSocket, tSocketInfo socketInfo) {
	if (bind(newSocket, (struct sockaddr*) &socketInfo, sizeof(socketInfo)) == -1) {
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

	if (connect(socketDestino, (struct sockaddr*) &infoSocketDestino, sizeof(infoSocketDestino)) == -1) {
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

int32_t enviarMensaje(int32_t numSocket, t_header header, t_contenido mensaje, t_log *logger) {

	puts("llego aca?");
	if (strlen(mensaje) > sizeof(t_contenido)) {
		log_error(logger, "Error en el largo del mensaje, tiene que ser menor o igual al máximo: %s", MSG_SIZE);
		return -1;
	}

	t_mensajes *s = malloc(sizeof(t_mensajes));
	s->id = header;
	strcpy(s->contenido, mensaje);
	log_info(logger, "Se ENVIA por SOCKET:%d - HEAD:%s MENSAJE:\"%s\" ", numSocket, getDescription(s->id), s->contenido);
	int n = send(numSocket, s, sizeof(*s), 0);
	if (n != STRUCT_SIZE) {
		log_error(logger, "#######################################################################");
		log_error(logger, "##    ERROR en el envío de mensajes: no se envió todo. socket: %d    ##", numSocket);
		log_error(logger, "#######################################################################");
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
		log_debug(logger, "Se RECIBE por SOCKET:%d - HEAD:%s%s%s MENSAJE:\"%s%s%s\" ", numSocket, getDescription(s->id), mensaje);
		return s->id;
	} else {
		if (n == 0) { // Conexión remota cerrada
			log_info(logger, "El socket %d cerró la conexion.", numSocket);
			strcpy(mensaje, "");
			return ERR_CONEXION_CERRADA;
		} else { // El mensaje tiene un tamaño distinto al esperado
			log_error(logger, "#######################################################");
			log_error(logger, "##    ERROR en el recibo de mensaje del socket %d    ##", numSocket);
			log_error(logger, "#######################################################");
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
		case ERR_CONEXION_CERRADA: return "ERR_CONEXION_CERRADA";
		case ERR_ERROR_AL_RECIBIR_MSG: return "ERR_ERROR_AL_RECIBIR_MSG";
		case ERR_ERROR_AL_ENVIAR_MSG: return "ERR_ERROR_AL_ENVIAR_MSG";

		case JOB_TO_MARTA_FILES: return "JOB_TO_MARTA_FILES";
		case JOB_TO_MARTA_MAP_OK: return "JOB_TO_MARTA_MAP_OK";
		case JOB_TO_MARTA_MAP_ERROR: return "JOB_TO_MARTA_MAP_ERROR";
		case JOB_TO_MARTA_REDUCE_OK: return "JOB_TO_MARTA_REDUCE_OK";
		case JOB_TO_MARTA_REDUCE_ERROR: return "JOB_TO_MARTA_REDUCE_ERROR";
		case JOB_TO_NODO_MAP_REQUEST: return "JOB_TO_NODO_MAP_REQUEST";
		case JOB_TO_NODO_REDUCE_REQUEST: return "JOB_TO_NODO_REDUCE_REQUEST";
		case JOB_TO_NODO_MAP_SCRIPT: return "JOB_TO_NODO_MAP_SCRIPT";
		case JOB_TO_NODO_REDUCE_SCRIPT: return "JOB_TO_NODO_REDUCE_SCRIPT";
		case JOB_TO_NODO_MAP_PARAMS: return "JOB_TO_NODO_MAP_PARAMS";
		case JOB_TO_NODO_REDUCE_PARAMS: return "JOB_TO_NODO_REDUCE_PARAMS";

		case MARTA_TO_JOB_ERROR: return "MARTA_TO_JOB_ERROR";
		case MARTA_TO_JOB_MAP_REQUEST: return "MARTA_TO_JOB_MAP_REQUEST";
		case MARTA_TO_JOB_REDUCE_REQUEST: return "MARTA_TO_JOB_REDUCE_REQUEST";
		case MARTA_TO_JOB_FILE_FOUND: return "MARTA_TO_JOB_FILE_FOUND";
		case MARTA_TO_JOB_FILE_NOT_FOUND: return "MARTA_TO_JOB_FILE_NOT_FOUND";
		case MARTA_TO_JOB_JOB_FINISHED: return "MARTA_TO_JOB_JOB_FINISHED";
		case MARTA_TO_FS_BUSCAR_ARCHIVO: return "MARTA_TO_FS_BUSCAR_ARCHIVO";
		case MARTA_TO_FS_BUSCAR_BLOQUE_ARCHIVO: return "MARTA_TO_FS_BUSCAR_BLOQUE_ARCHIVO";

		case NODO_TO_JOB_HANDSHAKE_OK: return "NODO_TO_JOB_HANDSHAKE_OK";
		case NODO_TO_JOB_MAP_OK: return "NODO_TO_JOB_MAP_OK";
		case NODO_TO_JOB_MAP_KO: return "NODO_TO_JOB_MAP_KO";
		case NODO_TO_JOB_REDUCE_OK: return "NODO_TO_JOB_REDUCE_OK";
		case NODO_TO_JOB_REDUCE_KO: return "NODO_TO_JOB_REDUCE_KO";
		case NODO_TO_JOB_REDUCE_SCRIPT_OK: return "NODO_TO_JOB_REDUCE_SCRIPT_OK";
		case NODO_TO_NODO_GET_BLOQUE: return "NODO_TO_NODO_GET_BLOQUE";
		case NODO_TO_NODO_SET_BLOQUE: return "NODO_TO_NODO_SET_BLOQUE";
		case NODO_TO_NODO_GET_FILE_CONTENT: return "NODO_TO_NODO_GET_FILE_CONTENT";
		case NODO_TO_FS_HANDSHAKE: return "NODO_TO_FS_HANDSHAKE";
		case FS_TO_MARTA_BLOQUES_ARCHIVO: return "FS_TO_MARTA_BLOQUES_ARCHIVO";
		case FS_TO_NODO_GET_BLOQUE: return "FS_TO_NODO_GET_BLOQUE";
		case FS_TO_NODO_SET_BLOQUE: return "FS_TO_NODO_SET_BLOQUE";
		case FS_TO_NODO_GET_FILE_CONTENT: return "FS_TO_NODO_GET_FILE_CONTENT";
		case FIN: return "FIN";

		default: return "---DEFAULT--- (mensaje sin definir)";

	}
	return "";
}



int escuchar(int puertoEscucha, int socketServer, void (*funcionParaProcesarMensaje)(int, header_t*, void*, t_log*), void* extra,  t_log* logger) {

	int miPID = process_get_thread_id();
	log_info(logger, "************** Comenzamos el proceso de escucha (PID: %d) ***************", miPID);

	//Logica principal para administrar conexiones
	fd_set masterFDList; //file descriptor list
	fd_set readFDList; //file descriptor list temporal para el select()
	int maxFDNumber; //maximo numero de file descriptor para hacer las busquedas en comunicaciones

	int socketEscucha;//socket escucha
	int nuevoFD;//file descriptor del cliente aceptado
	struct sockaddr_storage remoteaddr; //dirección del cliente
	socklen_t addrlen;

	char remoteIP[INET6_ADDRSTRLEN];

	int socketActual;

	struct addrinfo hints;

	FD_ZERO(&masterFDList);	//limpiamos la lista principal
	FD_ZERO(&readFDList); //limpiamos la lista de lectura

	 //Lleno la estructura de tipo addrinfo
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	t_socket_info socketInfo;
	socketInfo.sin_addr.s_addr = INADDR_ANY;
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_port = htons(puertoEscucha);
	memset(&(socketInfo.sin_zero), '\0', 8);

	socketEscucha = crearSocket();

	bindearSocket(socketEscucha, socketInfo);

	// el socket se pone en modo server
	if (listen(socketEscucha, 10) == -1) {
		perror("listen");
		exit(3);
	}

	// agregamos el socket a la lista principal
	FD_SET(socketEscucha, &masterFDList);


	// guardamos el maximo numero de descriptor
	maxFDNumber = socketEscucha;
	if (socketServer > -1) {
		FD_SET(socketServer, &masterFDList);
		if (socketServer > maxFDNumber) {    // grabamos el mayor FD
			maxFDNumber = socketServer;
		}
	}

//	log_info(logger, "socketEscucha %d", socketEscucha);
//	log_info(logger, "socketServer %d", socketServer);
//	log_info(logger, "maxFDNumber %d", maxFDNumber);

	for(;;){

		readFDList = masterFDList; // copy it

		if (select(maxFDNumber+1, &readFDList, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// recorremos las conexiones viendo si hay datos para leer
		for(socketActual = 0; socketActual <= maxFDNumber; socketActual++) {
//			log_debug(logger, "Reviso socket %d", socketActual);
			if (FD_ISSET(socketActual, &readFDList)) { // checkeamos si hay datos

				if (socketActual == socketEscucha) {

					// manejamos conexiones nuevas
					addrlen = sizeof remoteaddr;
					nuevoFD = accept(socketEscucha, (struct sockaddr *) &remoteaddr, &addrlen);

					if (nuevoFD == -1) {
						log_error(logger, string_from_format( "Hubo un error en el accept para el fd: %i", socketActual));
					} else {
						FD_SET(nuevoFD, &masterFDList); // agregamos a la lista principal
						if (nuevoFD > maxFDNumber) {    // grabamos el mayor FD
							maxFDNumber = nuevoFD;
						}
						void* inAddr = get_in_addr((struct sockaddr*) &remoteaddr);

						log_info(logger, string_from_format("Se recibe una nueva conexion desde %s en el socket %d\n",
								inet_ntop(remoteaddr.ss_family, inAddr, remoteIP, INET6_ADDRSTRLEN), nuevoFD));
					}

				} else {

					header_t mensaje;
					recibir_header_simple(socketActual, &mensaje);
					header_t* pMensaje = &mensaje;

					if (pMensaje->tipo == ERR_CONEXION_CERRADA) {
						//Removes from master set and say good bye! :)
						close(socketActual); // bye!

						FD_CLR(socketActual, &readFDList); // remove from master set

					} else {
						log_debug(logger, "Recibi un header tipo: %d, tamanio: %d", pMensaje->tipo, pMensaje->largo_mensaje);
						funcionParaProcesarMensaje(socketActual, &mensaje, extra, logger);
						FD_CLR(socketActual, &readFDList); //HACK faltaba limpiar, sino me traia los mensajes infinitamente
					}


				}
				FD_CLR(socketActual, &readFDList); //HACK faltaba limpiar, sino me traia los mensajes infinitamente
			}
		}
	}
	return 0;
}


void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

