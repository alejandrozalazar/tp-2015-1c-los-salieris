/*
 * conf.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include "conf.h"


void cargarConfiguracionPorDefecto(conf_nodo* conf) {
	conf->PUERTO_FS = 3000; //	3000	Puerto donde el FileSystem está escuchando solicitudes de nuevas conexiones
	conf->IP_FS = "127.0.0.1"; //	x.y.z.w	IP de la computadora donde está ejecutándose el FileSystem
	conf->ARCHIVO_BIN = "data.bin"; // 	data.bin	Nombre del archivo que contiene los bloques de datos
	conf->DIR_TEMP = "/tmp"; //	/tmp	Directorio donde se van a almacenar los archivos temporales
	conf->NODO_NUEVO = true; //	SI/NO	Un nodo nuevo es aquel que nunca formó parte del cluster. Este valor debe estar en SI la primera vez que un nodo ingresa al sistema y luego cambiar a NO.
	conf->IP_NODO = "127.0.0.1"; //	a.b.c.d.	IP de la computadora donde está ejecutándose este proceso nodo. (puede obtenerse del sistema)
	conf->PUERTO_NODO = 6000; //	6000	Puerto en el cual este proceso Nodo espera recibir conexiones nuevas.
	conf->BLOCK_SIZE_IN_BYTES = 20 * 1024; // 20k Tamano de bloque

}

void cargarDatosCalculados(conf_nodo* conf) {
	conf->otros = dictionary_create();
	cargarNombreNodoYRutaLog(conf->otros);
}

void cargarNombreNodoYRutaLog(t_dictionary* dic) {
	bool unicoNodoYMaquina = true;
	if (unicoNodoYMaquina) {
		dictionary_put(dic, "nombreNodo", "Nodo0");
		dictionary_put(dic, "rutaLog", "/tmp/Nodo0.log");
	} else {

		int nroNodo = 0;
		char * rutaBase = "/tmp";
		char * nombreNodo = malloc(1);
		char * rutaLog = malloc(1);
		bool nombreNuevo = false;

		for (nroNodo = 0; nombreNuevo == false; nroNodo++) {
			string_append_with_format(&nombreNodo, "%s%d", "Nodo", nroNodo);
			string_append_with_format(&rutaLog, "%s/%s.log", rutaBase, nombreNodo);
			if( access( rutaLog, F_OK ) == -1 ) {
				nombreNuevo = true;
			}
		}

		dictionary_put(dic, "nombreNodo", nombreNodo);
		dictionary_put(dic, "rutaLog", rutaLog);
	}

}
char * getRutaLog(conf_nodo* configuracion) {
	return (char *)dictionary_get(configuracion->otros, "rutaLog");//"/tmp/logNodo.log";
}

conf_nodo* cargarConfiguracion() {
	conf_nodo* configuracion = malloc(sizeof(conf_nodo));
	cargarConfiguracionPorDefecto(configuracion);
	cargarDatosCalculados(configuracion);
	return configuracion;
}
