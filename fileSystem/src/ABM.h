/*
 * filesystem.h
 *
 *  Created on: 07/06/2015
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

#include <commons/collections/list.h>
#include <mensajeria/mensajes.h>

t_list * archivos; //Lista de t_archivos
t_list * nodos; //Lista de t_nodos
t_dictionary * bloques_nodos_archivos;
t_dictionary * bloques_nodo_disponible;

typedef struct {
	int estado;
	t_list * copias; //Lista de t_copia
	int numero;
} t_bloque_archivo_self;

typedef struct {
	int estado;
	long tamanio;
	int padre;
	t_dictionary * bloques; //Lista de t_bloque_archivo
	char* nombre;
	int cantidad_bloques;
	int bloques_invalidos; //Lista de t_bloque_archivo
} t_archivo_self;

typedef struct {
	t_list* bloques;
	long tamanio;
} t_archivo_datos_self;

typedef struct {
	int estado;
	char * nombre;
	int bloques_disponibles;
//	t_list * archivos;
	t_dictionary bloques;
} t_nodo_self;

typedef struct {
	t_nodo_self * nodo;
	int bloque_nodo;
} t_nodo_bloque_self;

typedef struct {
	t_nodo_bloque_self * nodo_bloque;
	int archivo_bloque;
} t_copia_self;

typedef struct {
	char* nombre_archivo;
	int archivo_bloque;
} t_bloque_archivo_control_self;

#define CANTIDAD_BLOQUES_NODO_DEFAULT 50

#define ARCHIVO_NUEVO "/home/utnso/Escritorio/Nuevo.txt"
#define ARCHIVO_NUEVO2 "/home/utnso/Escritorio/Nuevo2.txt"

#endif /* FILESYSTEM_H_ */
