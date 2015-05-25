#include "fileSystem.h"

t_log* loggerFS;

char** vListaNodosMinima;
u_int16_t iPuertoFS;

int main(int argc, char *argv[]){

	if (argc==1){
		perror("No se puede iniciar el File System, falta indicar archivo de configuracion.");
		exit(EXIT_FAILURE);
	}

	// INICIO - Carga de configuracion del personaje

		cargarConfiguracion(argv[1]);

	// FIN - Carga de configuracion del personaje

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

