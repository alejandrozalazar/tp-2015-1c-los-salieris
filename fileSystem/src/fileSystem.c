#include "fileSystem.h"
#include "ABM.h"
#include <unistd.h>

t_log* loggerFS;

char** vListaNodosMinima;
unsigned short iPuertoFS;

int main(int argc, char *argv[]){

//	if (argc==1){
//		perror("No se puede iniciar el File System, falta indicar archivo de configuracion.");
//		exit(EXIT_FAILURE);
//	}

	// INICIO - Carga de configuracion del personaje

		cargarConfiguracion(NULL);

	// FIN - Carga de configuracion del personaje

		//escucharNuevasConexiones(iPuertoFS);
		if(escuchar(iPuertoFS, 0, (void*)tratarMensaje, NULL, loggerFS) < 0)
		{
			log_info(loggerFS, "No se pudo escuchar el puerto configurado");
		}

	return EXIT_SUCCESS;
}


// TODO: por cada switch del mensaje, deberia haber una funcion que la trate
void tratarMensaje(int numSocket, header_t* mensaje, void* extra, t_log* LOGGER){

	switch(mensaje->tipo){
		case NODO_TO_FS_HANDSHAKE:
			log_info(loggerFS, "Mensaje recibido: [%s] del socket [%d]", getDescription(mensaje->tipo), numSocket);
			enviarFSToNodoHandshakeOk(numSocket, loggerFS);
			sleep(3);
			log_info(loggerFS, "YA ESPERE");

			int nroBloque = 0;
			enviarFSToNodoGetBloque(numSocket, loggerFS, nroBloque);
		break;

		default: log_error(LOGGER, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  mensaje->tipo);

	}

}

void cargarConfiguracion(char* pathArchiConf){

//	pathArchiConf = "/home/utnso/dev/tp-2015-1c-los-salieris/fileSystem/src/configFileSystem";

	t_config* archivoConfig = config_create(pathArchiConf);
	char* valueProperty;

	if (config_has_property(archivoConfig, "PATH_LOG")) {
		valueProperty = (char*) config_get_string_value(archivoConfig, "PATH_LOG");
		loggerFS = log_create(valueProperty, "FileSystem", true, LOG_LEVEL_DEBUG);
	}else {
		loggerFS = log_create("logFileSystem.log", "FileSystem", true, LOG_LEVEL_DEBUG);
	}
	log_info(loggerFS, "Se inicializa el FS con el archivo %s", pathArchiConf);

	if (!config_has_property(archivoConfig, "LISTA_NODOS")) {
		log_error(loggerFS, "No se encontro la key 'LISTA_NODOS' en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (!config_has_property(archivoConfig, "PUERTO_LISTEN")) {
		log_error(loggerFS, "No se encontro la key 'PUERTO_LISTEN' en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

	valueProperty = config_get_string_value(archivoConfig, "LISTA_NODOS");
	char* listaNodosMinima = (char*)malloc(sizeof(char)*strlen(valueProperty));
	strcpy(listaNodosMinima, valueProperty);
	vListaNodosMinima = string_get_string_as_array(listaNodosMinima);

	strcpy(valueProperty, config_get_string_value(archivoConfig, "PUERTO_LISTEN"));
	iPuertoFS = atoi(valueProperty);

	log_info(loggerFS, "Se termino de cargar los datos de configuracion del FS");

	config_destroy(archivoConfig);
}

int escucharNuevasConexiones(int usPuerto){

	struct sockaddr_in myaddr; // dirección del servidor

	struct sockaddr_in remoteaddr; // dirección del cliente
	int fdmax; // número máximo de descriptores de fichero
	int listener; // descriptor de socket a la escucha
	int newfd; // descriptor de socket de nueva conexión aceptada

	int yes=1; // para setsockopt() SO_REUSEADDR, más abajo
	int addrlen;

	fd_set master; // conjunto maestro de descriptores de fichero
	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
	//fd_set write_fds;
	FD_ZERO(&master); // borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);  // obtener socket a la escucha
	//FD_ZERO(&write_fds);
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(loggerFS, "No se pudo crear el Socket");
		return(EXIT_FAILURE);
	}

	// Obviar el mensaje "address already in use" (la dirección ya se está usando)
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		log_error(loggerFS, "No se pudo setear los parametros al Socket");
		return(EXIT_FAILURE);
	}

	// Enlazar
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//myaddr.sin_addr.s_addr = htonl(INADDR_ANY); // usa mi dirección IP
	myaddr.sin_port = htons(usPuerto);
	memset(&(myaddr.sin_zero), '\0', 8);
	if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
		log_error(loggerFS, "No se pudo bindear al Socket");
		return(EXIT_FAILURE);
	}

	// Escuchar
	if (listen(listener, BACKLOG) == -1) {
		log_error(loggerFS, "No se pudo escuchar en el Socket");
		return(EXIT_FAILURE);
	}

	FD_SET(STDIN, &master);

	// Añadir listener al conjunto maestro
	FD_SET(listener, &master);
	// Seguir la pista del descriptor de fichero mayor
	fdmax = listener; // por ahora es éste
	int errnos = 0;
	int i;

	log_debug(loggerFS, " ---- Escucho en 'select()' ---- ");
	while(1) { 	// bucle principal
		read_fds = master; // cópialo
		//write_fds = master;
select_restart:
	printf("COMMAND->\n");
		if ((errnos = select(fdmax+1, &read_fds, NULL/*&write_fds*/, NULL, NULL)) == -1) {
			if (errnos == EINTR) {
				log_error(loggerFS, "Alguna señal me interrumpio, vuelvo a 'select()'");
				goto select_restart;
			}
			log_error(loggerFS, "FATAL Error en 'select()'");
			exit(EXIT_FAILURE);
		}

		// Explorar conexiones existentes en busca de datos que leer
		for(i=0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) {
				if (i == listener) {
					log_info(loggerFS, "Gestiono una nueva conexión al File System");

					addrlen = sizeof(remoteaddr);
					if ((newfd = accept(listener, (struct sockaddr 	*)&remoteaddr, &addrlen)) == -1) {
						log_error(loggerFS, "Error en 'accept()'");
					} else {
						log_info(loggerFS, "Socket %d",newfd);

						FD_SET(newfd, &master);
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						log_info(loggerFS, "Nueva conexión desde %s en el socket %d", inet_ntoa(remoteaddr.sin_addr), newfd);
					}
				} else { // hay un socket que hace un 'send'

					if (i==STDIN){
						char inputBuffer[MAX_LINE];
						char *args[MAX_LINE];
						obtenerComando(inputBuffer, args);
						//hacer el fork para atender el comando
						break;
					}
					log_info(loggerFS, "Un cliente envía datos...");

//					char *mensaje = malloc(256);
//					t_header header = recibir(i, mensaje, sizeof(t_header));

//					if(header == ERR_CONEXION_CERRADA) {
//
//						log_debug(loggerFS, "Remuevo el socket %d por desconexion", i);
//						cerrarSocket(i, &master);
//
//					} else {
//
//						log_debug(loggerFS, "Recibí este buffer: %s", mensaje);
//
//					}

					header_t mensaje;
					recibir_header_simple(i, &mensaje);
					header_t* pMensaje = &mensaje;


					switch(pMensaje->tipo){

				//		enviarSerializado(LOGGER, numSocket, false, MARTA_TO_JOB, 0, NULL);
				//		break; //HACK no encontraba el case, no se si era problema mio o falto commitear el commons

						case NODO_TO_FS_HANDSHAKE:
							log_info(loggerFS, "Mensaje recibido: [%s] del socket [%d]", getDescription(pMensaje->tipo), i);
							enviarFSToNodoHandshakeOk(i, loggerFS);

				//		pingback(numSocket);

						break;

						default: log_error(loggerFS, "ERROR mensaje NO RECONOCIDO (%d) !!\n",  pMensaje->tipo);

					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}


int enviarFSToNodoHandshakeOk(int socketNodo, t_log* logger){
	header_t header;


	initHeader(&header);
	header.tipo = FS_TO_NODO_HANDSHAKE_OK;
	header.cantidad_paquetes = 0;

	log_info(logger, "enviarFSToNodoHandshakeOk: sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	if (enviar_header(socketNodo, &header) != EXITO)
	{
		log_error(logger,"%s enviarFSToNodoHandshakeOk: Error al enviar header \n\n", getDescription(header.tipo));
		return WARNING;
	}

	return EXITO;
}



int enviarFSToNodoGetBloque(int socketNodo, t_log* logger, int nroBloque){
	header_t header;


	initHeader(&header);
	header.tipo = FS_TO_NODO_GET_BLOQUE;
	header.cantidad_paquetes = 0;

	log_info(logger, "enviarFSToNodoGetBloque: sizeof(header): %d, largo mensaje: %d \n", sizeof(header), header.largo_mensaje);

	if (enviar_header(socketNodo, &header) != EXITO)
	{
		log_error(logger,"%s enviarFSToNodoGetBloque: Error al enviar header \n\n", getDescription(header.tipo));
		return WARNING;
	}

	return EXITO;
}
void obtenerComando(char inputBuffer[], char *args[]){
	int length, /* # of characters in the command line */
	i,      /* loop index for accessing inputBuffer array */
	start,  /* index where beginning of next command parameter is */
	ct;     /* index of where to place the next parameter into args[] */

	ct = 0;

	/* Read what the user enters on the command line */
	length = read(STDIN_FILENO, inputBuffer, 100);

	start = -1;
	if (length == 0)
		exit(0);            /* ^d was entered, end of user command stream */
	if (length < 0){
		perror("error reading the command");
		exit(-1);           /* terminate with error code of -1 */
	}

	/* examine every character in the inputBuffer */
	for (i=0; i<length; i++){
		switch (inputBuffer[i]){
		case ' ':
		case '\t' :               /* argument separators */
			if(start != -1){
				args[ct] = &inputBuffer[start];    /* set up pointer */
				ct++;
			}
			inputBuffer[i] = '\0'; /* add a null char; make a C string */
			start = -1;
			break;

		case '\n':                 /* should be the final char examined */
			if (start != -1){
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			args[ct] = NULL; /* no more arguments to this command */
			break;

		default:             /* some other character */
			if (start == -1)
				start = i;
		}
	}
	args[ct] = NULL; /* just in case the input line was > 100 */
}
