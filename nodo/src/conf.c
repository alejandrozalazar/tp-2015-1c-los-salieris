/*
 * conf.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include "conf.h"


static char* NOMBRE_NODO = "NOMBRE_NODO";
static char* PUERTO_FS = "PUERTO_FS";
static char* IP_FS = "IP_FS";
static char* ARCHIVO_BIN = "ARCHIVO_BIN";
static char* DIR_TEMP = "DIR_TEMP";
static char* NODO_NUEVO = "NODO_NUEVO";
static char* IP_NODO = "IP_NODO";
static char* PUERTO_NODO = "PUERTO_NODO";

char* RUTA_ESPACIO_DATOS = "RUTA_ESPACIO_DATOS";
char* RUTA_LOG = "RUTA_LOG";

void cargarConfiguracionPorDefecto(conf_nodo* conf) {
	conf->PUERTO_FS = 3000; //	3000	Puerto donde el FileSystem está escuchando solicitudes de nuevas conexiones
	conf->IP_FS = "127.0.0.1"; //	x.y.z.w	IP de la computadora donde está ejecutándose el FileSystem
	conf->ARCHIVO_BIN = "data.bin"; // 	data.bin	Nombre del archivo que contiene los bloques de datos
	conf->DIR_TEMP = "/tmp"; //	/tmp	Directorio donde se van a almacenar los archivos temporales
	conf->NODO_NUEVO = true; //	SI/NO	Un nodo nuevo es aquel que nunca formó parte del cluster. Este valor debe estar en SI la primera vez que un nodo ingresa al sistema y luego cambiar a NO.
	conf->IP_NODO = "127.0.0.1"; //	a.b.c.d.	IP de la computadora donde está ejecutándose este proceso nodo. (puede obtenerse del sistema)
	conf->PUERTO_NODO = 6000; //	6000	Puerto en el cual este proceso Nodo espera recibir conexiones nuevas.
	conf->TIPO_CONFIGURACION = "Por defecto";
	conf->BLOCK_SIZE_IN_BYTES = 20 * 1024; // 20k Tamano de bloque

}

void cargarConfiguracionPorParametros(conf_nodo* conf, int argc, char *argv[], char* (funcionObtenerRutaArchivoConfiguracion)(int argc, char *argv[]), void (funcionCargarConfiguracionDeArchivo)(conf_nodo*, t_config*)) {

	char* rutaArchivoConfiguracion = funcionObtenerRutaArchivoConfiguracion(argc, argv);

	if(!string_is_null_or_empty(rutaArchivoConfiguracion)) {

		t_config* archivoConfiguracion = config_create(rutaArchivoConfiguracion);

		if (archivoConfiguracion != NULL && archivoConfiguracion->properties->elements_amount > 0) {

			funcionCargarConfiguracionDeArchivo(conf, archivoConfiguracion);

			config_destroy(archivoConfiguracion);
		}
	}
}

char* obtenerRutaArchivoConfiguracion(int argc, char *argv[]) {
	if (argc > 1) {
		return argv[1];
	}
	return NULL;
}

void cargarConfiguracionDeArchivo(conf_nodo* configuracion, t_config* archivoConfiguracion) {

	sprintf(configuracion->TIPO_CONFIGURACION, "Obtenida de archivo %s", archivoConfiguracion->path);
}

void cargarDatosCalculados(conf_nodo* conf) {
	conf->otros = dictionary_create();
	cargarNombreNodoYRutaLog(conf);

	char *rutaEspacioDatos = malloc(1);
	string_append_with_format(&rutaEspacioDatos, "%s/%s", conf->DIR_TEMP, conf->ARCHIVO_BIN);
	dictionary_put(conf->otros, RUTA_ESPACIO_DATOS, rutaEspacioDatos);
}

void cargarNombreNodoYRutaLog(conf_nodo* conf) {

	int nroNodo = conf->PUERTO_NODO;
	char * rutaBase = "/tmp";
	char * nombreNodo = malloc(1);
	char * rutaLog = malloc(1);

	string_append_with_format(&nombreNodo, "%s_%d", "Nodo", nroNodo);
	string_append_with_format(&rutaLog, "%s/%s.log", rutaBase, nombreNodo);

	dictionary_put(conf->otros, NOMBRE_NODO, nombreNodo);
	dictionary_put(conf->otros, RUTA_LOG, rutaLog);
}

char * getRutaLog(conf_nodo* configuracion) {
	return (char *) dictionary_get(configuracion->otros, RUTA_LOG);
}

char * getRutaEspacioDatos(conf_nodo* configuracion) {
	return (char *) dictionary_get(configuracion->otros, RUTA_ESPACIO_DATOS);
}

conf_nodo* cargarConfiguracion(int argc, char *argv[]) {
	conf_nodo* configuracion = malloc(sizeof(conf_nodo));
	cargarConfiguracionPorDefecto(configuracion);
	cargarConfiguracionPorParametros(configuracion, argc, argv, obtenerRutaArchivoConfiguracion, cargarConfiguracionDeArchivo);
	cargarDatosCalculados(configuracion);
	return configuracion;
}
