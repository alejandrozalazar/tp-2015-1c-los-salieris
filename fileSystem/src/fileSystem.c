#include "fileSystem.h"

t_log* loggerFS;

char** vListaNodosMinima;
unsigned short iPuertoFS;
//u_int16_t iPuertoFS;

int main(int argc, char *argv[]){

	if (argc==1){
		perror("No se puede iniciar el File System, falta indicar archivo de configuracion.");
		exit(EXIT_FAILURE);
	}

	// INICIO - Carga de configuracion del personaje

		cargarConfiguracion(argv[1]);

	// FIN - Carga de configuracion del personaje

		escucharNuevasConexiones(iPuertoFS);

	return EXIT_SUCCESS;
}

void cargarConfiguracion(char* pathArchiConf){

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
	fd_set master; // conjunto maestro de descriptores de fichero
	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
	//fd_set write_fds;
	struct sockaddr_in myaddr; // dirección del servidor
	struct sockaddr_in remoteaddr; // dirección del cliente
	int fdmax; // número máximo de descriptores de fichero
	int listener; // descriptor de socket a la escucha
	int newfd; // descriptor de socket de nueva conexión aceptada
	//t_contenido buff; // buffer para datos del cliente

	int yes=1; // para setsockopt() SO_REUSEADDR, más abajo
	int addrlen;

	//int j;
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
	if (listen(listener, 10) == -1) {
		log_error(loggerFS, "No se pudo escuchar en el Socket");
		return(EXIT_FAILURE);
	}

	// Añadir listener al conjunto maestro
	FD_SET(listener, &master);
	// Seguir la pista del descriptor de fichero mayor
	fdmax = listener; // por ahora es éste
	int errnos = 0;
	int i;

	while(1) { 	// bucle principal
		read_fds = master; // cópialo
		//write_fds = master;
		select_restart:
		log_debug(loggerFS, " ----> Escucho en 'select()'... ... ... ");
		if ((errnos = select(fdmax+1, &read_fds, NULL/*&write_fds*/, NULL, NULL)) == -1) {
			if (errnos == EINTR) {
				log_error(loggerFS, "Alguna señal me interrumpio, vuelvo a 'select()'");
				goto select_restart;
			}
			log_error(loggerFS, "FATAL Error en 'select()'");
			exit(EXIT_FAILURE);
		}
		// Explorar conexiones existentes en busca de datos que leer
		for(i=0 ; i <= fdmax; i++) {

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

					log_info(loggerFS, "Un cliente envía datos...");

					t_contenido mensaje;
					t_header header = recibirMensaje(i, mensaje, loggerFS);

					log_debug(loggerFS, "Recibí este buffer: %s", mensaje);

					if(header == ERR_CONEXION_CERRADA) {

						log_debug(loggerFS, "Remuevo el socket %d por desconexion", i);
						cerrarSocket(i, &master);

					} else {

					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
