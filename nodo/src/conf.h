/*
 * conf.h
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_CONF_H_
#define SRC_CONF_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct conf_nodo {

	int PUERTO_FS; //	3000	Puerto donde el FileSystem está escuchando solicitudes de nuevas conexiones
	char* IP_FS; //	x.y.z.w	IP de la computadora donde está ejecutándose el FileSystem
	char *ARCHIVO_BIN; // 	data.bin	Nombre del archivo que contiene los bloques de datos
	char *DIR_TEMP; //	/tmp	Directorio donde se van a almacenar los archivos temporales
	bool NODO_NUEVO; //	SI/NO	Un nodo nuevo es aquel que nunca formó parte del cluster. Este valor debe estar en SI la primera vez que un nodo ingresa al sistema y luego cambiar a NO.
	char* IP_NODO; //	a.b.c.d.	IP de la computadora donde está ejecutándose este proceso nodo. (puede obtenerse del sistema)
	int PUERTO_NODO; //	6000	Puerto en el cual este proceso Nodo espera recibir conexiones nuevas.
	int BLOCK_SIZE_IN_BYTES; // 20k Tamano de bloque
} conf_nodo;

conf_nodo* cargarConfiguracion();

void cargarConfiguracionPorDefecto(conf_nodo* conf);

char * getRutaLog();

#endif /* SRC_CONF_H_ */
