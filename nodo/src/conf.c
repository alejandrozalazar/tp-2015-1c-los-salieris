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

char* BLOCK_SIZE_IN_BYTES = "BLOCK_SIZE_IN_BYTES";
char* TAMANIO_ESPACIO_DATOS_EN_BYTES = "TAMANIO_ESPACIO_DATOS_EN_BYTES";

void cargarConfiguracionPorDefecto(conf_nodo* conf) {
	puts("Cargando configuracion por defecto");
	conf->NOMBRE_NODO = NULL;
	conf->PUERTO_FS = 3000; //	3000	Puerto donde el FileSystem está escuchando solicitudes de nuevas conexiones
	conf->IP_FS = "127.0.0.1"; //	x.y.z.w	IP de la computadora donde está ejecutándose el FileSystem
	conf->ARCHIVO_BIN = "data.bin"; // 	data.bin	Nombre del archivo que contiene los bloques de datos
	conf->DIR_TEMP = "/tmp"; //	/tmp	Directorio donde se van a almacenar los archivos temporales
	conf->NODO_NUEVO = true; //	SI/NO	Un nodo nuevo es aquel que nunca formó parte del cluster. Este valor debe estar en SI la primera vez que un nodo ingresa al sistema y luego cambiar a NO.
	conf->IP_NODO = "127.0.0.1"; //	a.b.c.d.	IP de la computadora donde está ejecutándose este proceso nodo. (puede obtenerse del sistema)
	conf->PUERTO_NODO = 6000; //	6000	Puerto en el cual este proceso Nodo espera recibir conexiones nuevas.
	conf->TIPO_CONFIGURACION = "Por defecto";
	conf->BLOCK_SIZE_IN_BYTES = 20 * 1024; // 20k Tamano de bloque
	conf->TAMANIO_ESPACIO_DATOS_EN_BYTES = "204800"; // 20k Tamano de bloque
}

void cargarConfiguracionPorParametros(conf_nodo* conf, int argc, char *argv[], char* (funcionObtenerRutaArchivoConfiguracion)(int argc, char *argv[]), void (funcionCargarConfiguracionDeArchivo)(conf_nodo*, t_config*)) {

	char* rutaArchivoConfiguracion = funcionObtenerRutaArchivoConfiguracion(argc, argv);

	if(!string_is_null_or_empty(rutaArchivoConfiguracion)) {

		t_config* archivoConfiguracion = config_create(rutaArchivoConfiguracion);

		if (archivoConfiguracion != NULL && archivoConfiguracion->properties->elements_amount > 0) {

			funcionCargarConfiguracionDeArchivo(conf, archivoConfiguracion);

			//config_destroy(archivoConfiguracion); //todonodo
		}
	}
}

char* obtenerRutaArchivoConfiguracion(int argc, char *argv[]) {
	if (argc > 1) {
		return argv[1];
	}
	return NULL;
}

bool si_o_no_to_bool(char* esNodoNuevo) {
	return string_equals_ignore_case(esNodoNuevo, "SI");
}

char* bool_to_si_o_no(bool booleano) {
	if(booleano) {
		return "SI";
	} else {
		return "NO";
	}
}

void cargarConfiguracionDeArchivo(conf_nodo* configuracion, t_config* archivoConfiguracion) {

	puts("Cargando configuracion de archivo");

	if (config_has_property(archivoConfiguracion, PUERTO_FS)) {
		configuracion->PUERTO_FS = config_get_int_value(archivoConfiguracion, PUERTO_FS);
	}

	if (config_has_property(archivoConfiguracion, IP_FS)) {
		configuracion->IP_FS = config_get_string_value(archivoConfiguracion, IP_FS);
	}

	if (config_has_property(archivoConfiguracion, ARCHIVO_BIN)) {
		configuracion->ARCHIVO_BIN = string_new();
		string_append(&configuracion->ARCHIVO_BIN, config_get_string_value(archivoConfiguracion, ARCHIVO_BIN));
	}

	if (config_has_property(archivoConfiguracion, DIR_TEMP)) {
		configuracion->DIR_TEMP = string_new();
		string_append(&configuracion->DIR_TEMP, config_get_string_value(archivoConfiguracion, DIR_TEMP));
	}
	if (config_has_property(archivoConfiguracion, NODO_NUEVO)) {
		char* esNodoNuevo = config_get_string_value(archivoConfiguracion, NODO_NUEVO);
		if (si_o_no_to_bool(esNodoNuevo)) {
			configuracion->NODO_NUEVO = true;
		} else {
			configuracion->NODO_NUEVO = false;
		}

	}

	if (config_has_property(archivoConfiguracion, NOMBRE_NODO)) {
			configuracion->NOMBRE_NODO = string_new();
			string_append(&configuracion->NOMBRE_NODO, config_get_string_value(archivoConfiguracion, NOMBRE_NODO));
		}

	if (config_has_property(archivoConfiguracion, IP_NODO)) {
		configuracion->IP_NODO = string_new();
		string_append(&configuracion->IP_NODO, config_get_string_value(archivoConfiguracion, IP_NODO));
	}
	if (config_has_property(archivoConfiguracion, PUERTO_NODO)) {
		configuracion->PUERTO_NODO = config_get_int_value(archivoConfiguracion, PUERTO_NODO);
	}

	if (config_has_property(archivoConfiguracion, BLOCK_SIZE_IN_BYTES)) {
		configuracion->BLOCK_SIZE_IN_BYTES = config_get_int_value(archivoConfiguracion, BLOCK_SIZE_IN_BYTES);
	}

	if (config_has_property(archivoConfiguracion, TAMANIO_ESPACIO_DATOS_EN_BYTES)) {
		configuracion->TAMANIO_ESPACIO_DATOS_EN_BYTES = config_get_string_value(archivoConfiguracion, TAMANIO_ESPACIO_DATOS_EN_BYTES);
	}

	configuracion->TIPO_CONFIGURACION = string_new();
	sprintf(configuracion->TIPO_CONFIGURACION, "Obtenida de archivo %s", archivoConfiguracion->path);
}

void cargarDatosCalculados(conf_nodo* conf) {
	cargarNombreNodoYRutaLog(conf);

	char *rutaEspacioDatos = string_new();
	string_append_with_format(&rutaEspacioDatos, "%s/%s", conf->DIR_TEMP, conf->ARCHIVO_BIN);
	conf->RUTA_ESPACIO_DATOS = string_new();
	string_append(&conf->RUTA_ESPACIO_DATOS, rutaEspacioDatos);
}

void cargarNombreNodoYRutaLog(conf_nodo* conf) {

	int nroNodo = conf->PUERTO_NODO;
	char* rutaBase = "/tmp";

	char *nombreNodo = string_new();
	string_append_with_format(&nombreNodo, "%s_%d", "Nodo", nroNodo);
	conf->NOMBRE_NODO = string_new();
	string_append(&conf->NOMBRE_NODO, nombreNodo);

	char *rutaLog = string_new();
	string_append_with_format(&rutaLog, "%s/%s.log", rutaBase, nombreNodo);
	conf->RUTA_LOG = string_new();
	string_append(&conf->RUTA_LOG, rutaLog);
}

char * getRutaLog(conf_nodo* configuracion) {
	return configuracion->RUTA_LOG;
}

char * getRutaEspacioDatos(conf_nodo* configuracion) {
	return configuracion->RUTA_ESPACIO_DATOS;
}

char * getValue(t_dictionary* dic, char* value) {
	return (char *) dictionary_get(dic, value);
}

conf_nodo* cargarConfiguracion(int argc, char *argv[]) {
	puts("Iniciando carga de configuracion");
	conf_nodo* configuracion = malloc(sizeof(conf_nodo));
	cargarConfiguracionPorDefecto(configuracion);
	cargarConfiguracionPorParametros(configuracion, argc, argv, obtenerRutaArchivoConfiguracion, cargarConfiguracionDeArchivo);
	cargarDatosCalculados(configuracion);
	puts("Finalizando carga de configuracion");
	return configuracion;
}

void loguearDatosConfiguracion(conf_nodo* conf, t_log* logger) {

	log_info(logger, "Estado inicial del nodo\n");

	log_info(logger, "NOMBRE_NODO    %s", conf->NOMBRE_NODO);

	log_info(logger, "IP_FS          %s", conf->IP_FS);
	log_info(logger, "PUERTO_FS      %d", conf->PUERTO_FS);

	log_info(logger, "IP_NODO        %s", conf->IP_NODO);
	log_info(logger, "PUERTO_NODO    %d", conf->PUERTO_NODO);

	log_info(logger, "DIR_TEMP       %s", conf->DIR_TEMP);
	log_info(logger, "ARCHIVO_BIN    %s", conf->ARCHIVO_BIN);

	log_info(logger, "NODO_NUEVO     %s", bool_to_si_o_no(conf->NODO_NUEVO));


	log_info(logger, "RUTA_LOG       %s", conf->RUTA_LOG);

	log_info(logger, "RUTA_ESPACIO_DATOS %s\n", conf->RUTA_ESPACIO_DATOS);
}
