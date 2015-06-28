#include "fileSystem.h"

t_log* loggerFS;

unsigned short iCantNodosMinima;
unsigned short iPuertoFS;

int seDesconectoUnNodo(int fdNodo, t_list* listaNodos, int* isFSOperativo){
	int i;
	for (i=0; i < list_size(listaNodos); ++i) {
		t_nodo* nodo = list_get(listaNodos, i);
		if (nodo->puerto == fdNodo){
			list_remove(listaNodos, i);
			if (list_size(listaNodos) < iCantNodosMinima){
				*isFSOperativo = false;
			}
			return true;
		}
	}
	return false;
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		perror("No se puede iniciar el File System, falta indicar archivo de configuracion.");
		exit(EXIT_FAILURE);
	}

	cargarConfiguracion(argv[1]);

	struct sockaddr_in myaddr; // dirección del servidor
	struct sockaddr_in remoteaddr; // dirección del cliente

	int fdmax; // número máximo de descriptores de fichero
	int listener; // descriptor de socket a la escucha
	int newfd; // descriptor de socket de nueva conexión aceptada

	int yes = 1; // para setsockopt() SO_REUSEADDR, más abajo
	int addrlen;

	fd_set master; // conjunto maestro de descriptores de fichero
	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
	//fd_set write_fds;
	FD_ZERO(&master);// borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);  // obtener socket a la escucha
	//FD_ZERO(&write_fds);

	//struct timeval tiempoEspera;
	//tiempoEspera.tv_sec = 30;
	//tiempoEspera.tv_usec = 0;

	int i, tamanioMensaje;
	char buffer[STRUCT_SIZE];

	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(loggerFS, "No se pudo crear el Socket");
		return (EXIT_FAILURE);
	}

	// Obviar el mensaje "address already in use" (la dirección ya se está usando)
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		log_error(loggerFS, "No se pudo setear los parametros al Socket");
		return (EXIT_FAILURE);
	}

	// Enlazar
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//myaddr.sin_addr.s_addr = htonl(INADDR_ANY); // usa mi dirección IP
	myaddr.sin_port = htons(iPuertoFS);
	memset(&(myaddr.sin_zero), '\0', 8);
	if (bind(listener, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1) {
		log_error(loggerFS, "No se pudo bindear al Socket");
		return (EXIT_FAILURE);
	}

	// Escuchar
	if (listen(listener, BACKLOG) == -1) {
		log_error(loggerFS, "No se pudo escuchar en el Socket");
		return (EXIT_FAILURE);
	}

	// Añado el teclado y el listener al conjunto maestro
	FD_SET(STDIN, &master);
	FD_SET(listener, &master);
	// Seguir la pista del descriptor de fichero mayor
	fdmax = listener; // por ahora es éste

	t_mensajes* elMensaje = (t_mensajes*) buffer;
	t_contenido contenido;
	t_list * listaNodos = list_create();
	int isFSOperativo = false;

	do { // bucle principal del Select
		read_fds = master; // cópialo
		//write_fds = master;

		printf("COMMAND->\n");
		if (select(fdmax + 1, &read_fds, NULL/*&write_fds*/, NULL, NULL/*&tiempoEspera*/) == -1) {
			log_error(loggerFS, "FATAL Error en 'select()'");
			exit(EXIT_FAILURE);
		}

		// Explorar conexiones existentes en busca de datos que leer
		for (i=0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) { //FD_ISSET: Return true if fd is in the set.
				if (i == listener) {
					log_debug(loggerFS, "Gestiono una nueva conexión al File System");

					addrlen = sizeof(remoteaddr);
					if ((newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen)) == -1) {
						log_error(loggerFS, "Error en 'accept()'");
					} else {
						FD_SET(newfd, &master); // Agregamos el FD al conjunto master
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						log_info(loggerFS, "Nueva conexión desde %s en el socket %d", inet_ntoa(remoteaddr.sin_addr), newfd);

						/*if (!fork()) { // this is the child process
						 close(listener); // child doesn't need the listener
						 if (send(newfd, "Hello, world!", 13, 0) == -1)
						 perror("send");
						 close(newfd);
						 exit(0);
						 }
						 //close(newfd);  // parent doesn't need this
						 */

					}
				} else { // Hay un socket que nos envia un mensaje

					if (i == STDIN) {
						char inputBuffer[MAX_LINE];
						char *args[MAX_LINE];
						obtenerComando(inputBuffer, args);

						if (strcmp(args[0], "exit") == 0) {
							log_info(loggerFS, "Exit");
							log_destroy(loggerFS);
							list_destroy(listaNodos);
							exit(EXIT_SUCCESS);
						} else if (strcmp(args[0], "status") == 0) {
							printf("Cantidad de Nodos necesarios: %d\n", iCantNodosMinima);
							printf("Cantidad de Nodos conectados: %d\n", list_size(listaNodos));
							break;
						} else {
							printf("No se reconoce el comando.\n");
							break;
						}
					} else {
						log_debug(loggerFS, "Un cliente envía datos.");

						if ((tamanioMensaje = recv(i, buffer, STRUCT_SIZE, 0)) <= 0) {
							// Got error or connection closed by client
							if (tamanioMensaje == 0) {
								log_info(loggerFS, "El cliente en el socket %d se fue.", i);
								// HAY QUE CHEQUEAR SI ES UN NODO, DESCONTARLO, VER SI SE DESACTIVA EL FS y BAJAR LOS ARCHIVOS QUE TENIA
								if (seDesconectoUnNodo(i, listaNodos, &isFSOperativo)){
									//BAJAR LOS ARCHIVOS QUE TENIA
								}
							} else {
								log_error(loggerFS, "ERROR al Recibir en el socket %d", i);
							}
							close(i); // bye!
							FD_CLR(i, &master); // remove from master set
						} else {
							strcpy(contenido, elMensaje->contenido);

							switch(elMensaje->id) {
							   case NODO_TO_FS_HANDSHAKE:
								   log_debug(loggerFS, "Se conecto un Nodo en SOCKET:%d - MENSAJE:\"%s\" ", i, contenido);
								   t_nodo* elNodo = malloc(sizeof(t_nodo));
								   char** vContenido = string_get_string_as_array(contenido);
								   strcpy(elNodo->nombre, vContenido[0]);
								   strcpy(elNodo->ip, vContenido[1]);
								   if (strcmp("SI", vContenido[2])){
									   elNodo->disponible = true;
								   }else {
									   elNodo->disponible = false;
								   }
								   elNodo->puerto = i;

								   list_add(listaNodos, elNodo);
								   int cantNodos = list_size(listaNodos);
								   log_debug(loggerFS, "CANTIDAD NODOS:%d ", cantNodos);
								   if (cantNodos >= iCantNodosMinima){
									   isFSOperativo = true;
								   }

								   if (send(newfd, "Hello!", 13, 0) == -1)
									   perror("send");

								   break;
							   default:
								   log_info(loggerFS, "El mensaje tiene un encabezado desconocido, en el socket %d.", i);
								   log_info(loggerFS, "Mensaje: %s.", contenido);
							}
						}
					}
				}
			}
		}
	} while (true);

	return EXIT_SUCCESS;
}

void cargarConfiguracion(char* pathArchiConf) {

	t_config* archivoConfig = config_create(pathArchiConf);
	char* valueProperty;

	if (config_has_property(archivoConfig, "PATH_LOG")) {
		valueProperty = (char*) config_get_string_value(archivoConfig, "PATH_LOG");
		loggerFS = log_create(valueProperty, "FileSystem", true, LOG_LEVEL_DEBUG);
	} else {
		loggerFS = log_create("logFileSystem.log", "FileSystem", true, LOG_LEVEL_DEBUG);
	}
	log_info(loggerFS, "Se inicializa el FS con el archivo %s", pathArchiConf);

	if (!config_has_property(archivoConfig, "CANT_MINIMA_NODOS")) {
		log_error(loggerFS, "No se encontro la key 'CANT_MINIMA_NODOS' en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (!config_has_property(archivoConfig, "PUERTO_LISTEN")) {
		log_error(loggerFS, "No se encontro la key 'PUERTO_LISTEN' en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

	iCantNodosMinima = config_get_int_value(archivoConfig, "CANT_MINIMA_NODOS");

	strcpy(valueProperty, config_get_string_value(archivoConfig, "PUERTO_LISTEN"));
	iPuertoFS = config_get_int_value(archivoConfig, "PUERTO_LISTEN");

	log_info(loggerFS, "Se termino de cargar los datos de configuracion del FS");

	config_destroy(archivoConfig);
}


void obtenerComando(char inputBuffer[], char *args[]) {
	int length, /* # of characters in the command line */
	i, /* loop index for accessing inputBuffer array */
	start, /* index where beginning of next command parameter is */
	ct = 0; /* index of where to place the next parameter into args[] */

	/* Read what the user enters on the command line */
	length = read(STDIN_FILENO, inputBuffer, 100);

	start = -1;
	if (length == 0)
		exit(0); /* ^d was entered, end of user command stream */
	if (length < 0) {
		perror("error reading the command");
		exit(-1); /* terminate with error code of -1 */
	}

	/* examine every character in the inputBuffer */
	for (i=0; i < length; i++) {
		switch (inputBuffer[i]) {
		case ' ':
		case '\t': /* argument separators */
			if (start != -1) {
				args[ct] = &inputBuffer[start]; /* set up pointer */
				ct++;
			}
			inputBuffer[i] = '\0'; /* add a null char; make a C string */
			start = -1;
			break;

		case '\n': /* should be the final char examined */
			if (start != -1) {
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			args[ct] = NULL; /* no more arguments to this command */
			break;

		default: /* some other character */
			if (start == -1)
				start = i;
		}
	}
	args[ct] = NULL; /* just in case the input line was > 100 */
}
