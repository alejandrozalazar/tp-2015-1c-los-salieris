/*
 * conf.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include "conf.h"
#include <stdbool.h>


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

}

char * getRutaLog() {
	return "/tmp/logNodo.log";
}

conf_nodo* cargarConfiguracion() {
	conf_nodo* configuracion = malloc(sizeof(conf_nodo));
	cargarConfiguracionPorDefecto(configuracion);
	cargarDatosCalculados(configuracion);
	return configuracion;
}
