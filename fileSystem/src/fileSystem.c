#include "fileSystem.h"

#define BUFSIZE 80

t_log* loggerFS;

int main(int argc, char *argv[]) {

	if (argc == 1) {
		perror("No se puede iniciar el File System, falta indicar archivo de configuracion.");
		exit(EXIT_FAILURE);
	}

	int iCantNodosMinima;
	int iPuertoFS;
	if (cargarConfiguracion(argv[1], &iPuertoFS, &iCantNodosMinima)) {
		perror("Problemas al cargar la configuracion.");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in myaddr; // Dirección del servidor
	struct sockaddr_in remoteaddr; // Dirección del cliente

	int fdmax; // Número máximo de descriptores de fichero
	int listener; // Descriptor de socket a la escucha
	int newfd; // Descriptor de socket de nueva conexión aceptada

	int yes = 1; // Para setsockopt() SO_REUSEADDR
	int addrlen;

	fd_set master; // Conjunto maestro de descriptores de fichero
	fd_set read_fds; // Conjunto temporal de descriptores de fichero para select()

	FD_ZERO(&master); // Borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);  // Obtener socket a la escucha

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
	// Seguir la pista del descriptor de fichero mayor, por ahora es listener
	fdmax = listener;

	int i;
	bool repetir = true;
	t_mensaje* elMensaje = malloc(sizeof(t_mensaje));
	int tamanioMensaje;
	char* contenido = malloc(TAMCONTENIDO * sizeof(char) + 1);
	t_list * listaNodos = list_create();
	t_nodo* unNodo = malloc(sizeof(t_nodo));
	int isFSOperativo = false;

	do { // Bucle principal del Select
		read_fds = master; // Igualamos los 2 conjuntos, el temporal y el maestro.

		printf("COMMAND->\n");
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
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
						FD_SET(newfd, &master); // Agregamos el FD al conjunto maestro
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						log_info(loggerFS, "Nueva conexión desde %s en el socket %d", inet_ntoa(remoteaddr.sin_addr), newfd);
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
							repetir = false;
							break;
						} else if (strcmp(args[0], "status") == 0) {
							verStatus(iCantNodosMinima, listaNodos);

							break;
						} else if (strcmp(args[0], "cp") == 0) {
							if (true/*TODO: descomentar esto isFSOperativo*/) {
								printf("Se copia el archivo %s en los Nodos.\n", args[1]);

								FILE *file;
								file = fopen("/home/utnso/projects/tp-2015-1c-los-salieris/fileSystem/Debug/miniArchivo.txt", "rb");
								if (file == NULL) {
									printf("No se encontro el archivo.");
									break;
								}

								fseek(file, 0, SEEK_END);
								long filelen = ftell(file);
								rewind(file);

								int cantidadBloques = filelen / BUFSIZE;
								int resto = filelen % BUFSIZE;
								char* segmento = malloc(BUFSIZE * sizeof(char));
								int j, k;
								// Eviar Segmentos del archivo leido
								elMensaje->tamanio = BUFSIZE; // Avisamos de cuanto va a ser el segmento enviado.
								for (j=0; j < cantidadBloques; j++) {
									fread(segmento, BUFSIZE*sizeof(char), 1, file);

									for (k=0; k < BUFSIZE*sizeof(char); ){
										strcpy(elMensaje->contenido, string_substring(segmento, k, k+TAMCONTENIDO));
										k = k + TAMCONTENIDO;
										unNodo = list_get(listaNodos, 0);
										if (send(unNodo->fd, elMensaje, sizeof(*elMensaje), 0) == -1)
											perror("send");
									}
								}

								if (resto != 0) {

									fread(segmento, BUFSIZE*sizeof(char), 1, file);
									segmento[resto] = '\0';
									strcpy(elMensaje->contenido, segmento);

									unNodo = list_get(listaNodos, 0);
									if (send(unNodo->fd, elMensaje, sizeof(*elMensaje), 0) == -1)
										perror("send");
								}

								fclose(file);

							} else {
								printf("El FS no esta operativo.\n");
							}
							break;
						} else if (strcmp(args[0], "mkdir") == 0) {
							printf("Se crea el directorio");
							break;
						} else {
							printf("No se reconoce el comando.\n");
							break;
						}
					} else {
						log_debug(loggerFS, "Un cliente envía datos.");

						if ((tamanioMensaje = recv(i, elMensaje, sizeof(*elMensaje), 0)) <= 0) {
							// Got error or connection closed by client
							if (tamanioMensaje == 0) {
								log_info(loggerFS, "El cliente en el socket %d se fue.", i);
								// TODO: HAY QUE CHEQUEAR SI ES UN NODO, DESCONTARLO, VER SI SE DESACTIVA EL FS y BAJAR LOS ARCHIVOS QUE TENIA EL NODO
								if (seDesconectoUnNodo(i, listaNodos, iCantNodosMinima, &isFSOperativo)) {

								}
							} else {
								log_error(loggerFS, "ERROR al Recibir en el socket %d", i);
							}
							close(i); // bye!
							FD_CLR(i, &master); // remove from master set
						} else {

							strcpy(contenido, elMensaje->contenido);

							switch (elMensaje->tipo) {
							case NODO_TO_FS_HANDSHAKE:
								log_debug(loggerFS, "Se conecto un Nodo en SOCKET:%d - MENSAJE:\"%s\" ", i, contenido);

								char** vContenido = string_split(contenido, ",");
								int it = 0;
								while (vContenido[it] != NULL) {
									string_trim(&(vContenido[it]));
									it++;
								}

								t_nodo* elNodo = malloc(sizeof(t_nodo));
								strcpy(elNodo->nombre, vContenido[0]);
								if (strcmp("SI", vContenido[1]) == 0) {
									elNodo->disponible = true;
								} else {
									elNodo->disponible = false;
								}
								strcpy(elNodo->ip, vContenido[2]);
								elNodo->puerto = strtol(vContenido[3], NULL, 10);
								elNodo->fd = i;

								free(vContenido);

								// Respondemos
								elMensaje->tipo = FS_TO_NODO_HANDSHAKE_OK;
								strcpy(contenido, "Esperar activacion");
								elMensaje->tamanio = strlen(contenido);
								strcpy(elMensaje->contenido, contenido);

								if (send(newfd, elMensaje, sizeof(*elMensaje), 0) == -1) {
									log_error(loggerFS, "No se pudo enviar mensaje de espera al Nodo");
								}

								list_add(listaNodos, elNodo);
								int cantNodos = list_size(listaNodos);
								log_debug(loggerFS, "CANTIDAD NODOS:%d ", cantNodos);
								if (cantNodos >= iCantNodosMinima) {
									isFSOperativo = true;
									// TODO: VER SI HAY AVISAR A TODOS LOS NODOS QUE YA ESTAN OPERATIVOS
								}
								break;
							case MARTA_TO_FS_HANDSHAKE:
								// TODO: MOCK. Se fuerza a que el fs sea operativo (despues vemos el tema de la conexion de los nodos)
								isFSOperativo = true;

								//t_header respuesta = isFSOperativo ? ACK : NAK;
								//log_info(loggerFS, "%s: Respondo con el header %s. ", getDescription(MARTA_TO_FS_HANDSHAKE), getDescription(respuesta));
								//enviar_t_mensaje(i, respuesta, 0, "");
								break;
							case MARTA_TO_FS_BUSCAR_ARCHIVO:
								//buscar_bloques_y_responder(i, contenido);
								log_debug(loggerFS, "y ahora?");

								break;
							default:
								log_info(loggerFS, "El mensaje tiene un encabezado desconocido, en el socket %d.", i);
								log_info(loggerFS, "Mensaje: %s", contenido);
							}
						}
					}
				}
			}
		}
	} while (repetir);

	free(elMensaje);
	free(unNodo);

	return EXIT_SUCCESS;
}

int cargarConfiguracion(char* pathArchiConf, int* iPuertoFS, int* iCantNodosMinima) {
	t_config* archivoConfig = config_create(pathArchiConf);
	char* valueProperty;

	if (config_has_property(archivoConfig, "PATH_LOG")) {
		valueProperty = (char*) config_get_string_value(archivoConfig, "PATH_LOG");
		loggerFS = log_create(valueProperty, "FileSystem", LOG_EN_CONSOLA, LOG_LEVEL);
	} else {
		loggerFS = log_create("logFileSystem.log", "FileSystem", LOG_EN_CONSOLA, LOG_LEVEL);
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

	*iCantNodosMinima = config_get_int_value(archivoConfig, "CANT_MINIMA_NODOS");

	strcpy(valueProperty, config_get_string_value(archivoConfig, "PUERTO_LISTEN"));
	*iPuertoFS = config_get_int_value(archivoConfig, "PUERTO_LISTEN");

	log_info(loggerFS, "Se termino de cargar los datos de configuracion del FS");

	config_destroy(archivoConfig);

	return EXIT_SUCCESS;
}

void obtenerComando(char inputBuffer[], char *args[]) {
	int length, /* # of characters in the command l<ine */
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
	for (i = 0; i < length; i++) {
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

int seDesconectoUnNodo(int fdNodo, t_list* listaNodos, int iCantNodosMinima, int* isFSOperativo) {
	int i;
	for (i=0; i < list_size(listaNodos); ++i) {
		t_nodo* nodo = list_get(listaNodos, i);
		if (nodo->fd == fdNodo) {
			list_remove(listaNodos, i);
			if (list_size(listaNodos) < iCantNodosMinima) {
				*isFSOperativo = false;
			}
			return true;
		}
	}
	return false;
}

int verStatus(int iCantNodosMinima, t_list* listaNodos){
	printf("Cantidad de Nodos necesarios: %d\n", iCantNodosMinima);
	int tamLista = list_size(listaNodos);
	printf("Cantidad de Nodos conectados: %d\n", tamLista);
	if (tamLista > 0) {
		int it;
		t_nodo* unNodo = malloc(sizeof(t_nodo));
		for (it = 0; it < tamLista; it++) {
			unNodo = list_get(listaNodos, it);
			printf("Nombre:%s ", unNodo->nombre);
			printf("IP:%s ", unNodo->ip);
			printf("Puerto:%d ", unNodo->puerto);
			printf("FD:%d \n", unNodo->fd);
		}
	}
	return EXIT_SUCCESS;
}
