#include "fileSystem.h"

int main(int argc, char *argv[]){


	if (argc==1){
		perror("No se puede iniciar el File System, falta indicar archivo de configuracion.");
		exit(EXIT_FAILURE);
	}
	if(argc==3 && strcmp(argv[2], "testnodo") == 0) {
		return mainAlternativo(argc, argv);
	}

	int iCantNodosMinima;
	int iPuertoFS;
//	listaNodos = list_create();
	if (cargarConfiguracion(argv[1], &iPuertoFS, &iCantNodosMinima)){
//		list_destroy_and_destroy_elements(listaNodos, (void*)free);
		perror("Problemas al cargar la configuracion.");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in myaddr; // dirección del servidor
	struct sockaddr_in remoteaddr; // dirección del cliente

	int fdmax; // número máximo de descriptores de fichero
	int listener; // descriptor de socket a la escucha
	int newfd; // descriptor de socket de nueva conexión aceptada

	int yes = 1; // para setsockopt() SO_REUSEADDR, más abajo
	int addrlen;

	fd_set master; // conjunto maestro de descriptores de fichero
	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()

	FD_ZERO(&master);// borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);  // obtener socket a la escucha

	int i;

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

	header_t* elMensaje = malloc(sizeof(header_t));
	int tamanioMensaje;
	char* contenido;
	char** vContenido;

	t_log* logger = loggerFS;

	int isFSOperativo = false;

	iniciar();

	do { // bucle principal del Select
		read_fds = master; // cópialo

		consola();

//		printf("COMMAND->\n");
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(loggerFS, "FATAL Error en 'select()' %s", strerror(errno));
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
					}

				} else { // Hay un socket que nos envia un mensaje

					if(i == STDIN) {
						procesarComando(loggerFS);
						break;
					}

					if (false && i == STDIN) {
						char inputBuffer[MAX_LINE];
						char *args[MAX_LINE];
						obtenerComando(inputBuffer, args);

						if (strcmp(args[0], "exit") == 0) {
							log_info(loggerFS, "Exit");
							log_destroy(loggerFS);
							list_destroy(nodos);
							exit(EXIT_SUCCESS);
						} else if (strcmp(args[0], "status") == 0) {
							printf("Cantidad de Nodos necesarios: %d\n", iCantNodosMinima);
							printf("Cantidad de Nodos conectados: %d\n", list_size(nodos));
							break;
						} else if (strcmp(args[0], "cp") == 0){
							if (isFSOperativo){
								printf("Se copia el archivo %s en los Nodos.\n", args[1]);
								//agregar_archivo(args[1]);
								int cantNodos = list_size(nodos);
								int var;
//								char* setBloqueContent = obtenerContenidoArchivo(args[1]); //viejo, se rompia
								long int tamanioBloque = obtenerTamanioArchivo(args[1], loggerFS);
//								int tamanioBloque = 204800;
								t_header tipoSetBloque = FS_TO_NODO_SET_BLOQUE;
								int nroBloque = 0;
								for (var = 0; var < cantNodos; var++) {

									//aca podria comenzar un hilo

									t_nodo_self* nodoSelf = list_get(nodos, var);
									t_nodo* nodoActual = convertir_t_nodo_self_a_t_nodo(nodoSelf);
									//viejo, se rompia por que levantaba el archivo completo, ahora paso el fd nomas
//									int resultadoSetBloque2 = enviarFsToNodoSetBloque(nodoActual->fd, logger, nroBloque, setBloqueContent, tipoSetBloque);
									int fdContenido = abrirArchivoSoloLectura(args[1], loggerFS);

									//mando el contenido al nodo, aca lo manda full, se deberia mandar el bloque nomas
									int resultadoSetBloque2 = enviarFsToNodoSetBloqueFromDescriptor(nodoActual->fd, logger, nroBloque, fdContenido, tamanioBloque, tipoSetBloque);

									close(fdContenido);
									if(resultadoSetBloque2 != EXITO) {
										log_error(loggerFS, "Se produjo un error enviando el contenido del archivo %s al nodo %s por el socket [%d]\n", args[1], nodoActual->nombre, nroBloque);
										break;
									}
								}
								break;
							} else {
								printf("El FS no esta operativo.\n");
							}
							break;
						} else {
							printf("No se reconoce el comando.\n");
							break;
						}
					} else {
						log_debug(loggerFS, "Un cliente envía datos.");

						if (recibir_header_simple(i, elMensaje) != EXITO) {
							// Got error or connection closed by client
							log_info(loggerFS, "El cliente en el socket %d se fue.", i);
							// TODO: HAY QUE CHEQUEAR SI ES UN NODO, DESCONTARLO, VER SI SE DESACTIVA EL FS y BAJAR LOS ARCHIVOS QUE TENIA EL NODO
							if (seDesconectoUnNodo(i, nodos, iCantNodosMinima, &isFSOperativo)){

							}
							close(i); // bye!
							FD_CLR(i, &master); // remove from master set
						} else {

							contenido = strdup(elMensaje->contenido);
							log_info(loggerFS, "Recibi un mensaje del socket %d\nheader: %s\nmensaje: %s", i, getDescription(elMensaje->tipo), elMensaje->contenido);

							switch(elMensaje->tipo) {
							   case MARTA_TO_FS_HANDSHAKE:
								   // TODO: MOCK. Se fuerza a que el fs sea operativo (despues vemos el tema de la conexion de los nodos)
								   isFSOperativo = true;

								   t_header respuesta = isFSOperativo ? ACK : NAK;
								   log_info(loggerFS, "%s: Respondo con el header %s. ", getDescription(MARTA_TO_FS_HANDSHAKE), getDescription(respuesta));
								   enviar_t_mensaje(i, respuesta, 0, "");

								   break;
							   case NODO_TO_FS_HANDSHAKE:

								   log_info(logger, "Intentamos recibir t_nodo por el socket [%d]\n", i);
								   int ret;
								   t_nodo* elNodo = malloc(sizeof(t_nodo));
								   if ((ret = recibir_struct(i, elNodo, sizeof(t_nodo))) != EXITO) {
									   log_error(logger, "Error recibiendo t_nodo por el socket [%d]\n", i);
									   return ret;
								   }

								   // Respondemos
								   elMensaje->tipo = FS_TO_NODO_HANDSHAKE_OK;
								   contenido = "Esperar activacion";
								   elMensaje->largo_mensaje = strlen(contenido);
								   memcpy(elMensaje->contenido, contenido, strlen(contenido));

								   if ((ret = enviarFSToNodoHandshakeOk(i, contenido, logger)) != EXITO) {
									   log_error(logger, "Error al enviar el handshake OK al Nodo por el socket [%d]\n", i);
									   return ret;
								   }

								   elNodo->fd = i;

//								   add_nodo_to_nodos(nodoSelf, false);
								   alta_nodo(elNodo->nombre, elNodo);
								   int cantNodos = list_size(nodos);
								   log_debug(loggerFS, "CANTIDAD NODOS:%d ", cantNodos);
								   if (cantNodos >= iCantNodosMinima){
									   isFSOperativo = true;
									   // TODO: VER SI HAY AVISAR A TODOS LOS NODOS QUE YA ESTAN OPERATIVOS
								   }
								   break;
							   case MARTA_TO_FS_BUSCAR_ARCHIVO:
								   // TODO: no tiene sentido preguntar si está operativo o no, ya que nunca se recibirá
								   // este msj si el fs no está operativo

								   buscar_bloques_y_responder(i, contenido);
								   log_debug(loggerFS, "y ahora?");

//								   if (isFSOperativo){
//									   t_archivo_self* buscarArchivo = malloc(sizeof(t_mensaje));
//									   vContenido = string_get_string_as_array(contenido);
//									   strcpy(buscarArchivo->nombre, vContenido[0]);
//									   buscarArchivo->padre = atoi(vContenido[1]);
//								   } else {
//									   // Respondemos
//									   elMensaje->tipo = FS_TO_MARTA_ESPERAR_ACTIVACION;
//									   strcpy(elMensaje->contenido, "Esperar activacion");
//									   elMensaje->tamanio = strlen(elMensaje->contenido);
//									   if (send(newfd, elMensaje, sizeof(*elMensaje), 0) == -1){
//										   log_error(loggerFS, "No se pudo enviar mensaje de espera a Marta");
//									   }
//								   }
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

t_nodo_self* convertir_t_nodo_a_t_nodo_self(t_nodo* tnodo) {

	return crear_nodo(tnodo->nombre, VALIDO, NULL, 0, tnodo->carga, tnodo->disponible, tnodo->fd, tnodo->ip, tnodo->puerto);
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

	if(config_has_property(archivoConfig, "MOCK") && config_get_int_value(archivoConfig, "MOCK")){
		mock_agregar_nodos();
	}

	config_destroy(archivoConfig);

	return EXIT_SUCCESS;
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


int seDesconectoUnNodo(int fdNodo, t_list* listaNodos, int iCantNodosMinima, int* isFSOperativo){
	int i;
	for (i=0; i < list_size(listaNodos); ++i) {
		t_nodo_self* nodo = list_get(listaNodos, i);
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
