/*
 * MDFS.h *
 *  Created on: 15/04/2015
 *      Author: maxiutn
 */

#ifndef MDFS_H_
#define MDFS_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <string.h>
#include <pthread.h>
#include <commons/config.h>
#include <stdlib.h>

typedef struct{

	int nrobloque;
	long int bytedelchar;

}t_bloquesdelMDFS;

typedef struct
{
	char * nombre;
	int nrobloque;
	char disponible;

}t_nodo;

typedef struct{
	t_nodo * nodo1;
	t_nodo * nodo2;
	t_nodo * nodo3;
}t_bloquedelarchivo;

typedef struct
{
	char estado;
	long int tamanio;
	t_list * bloques;
	int padre;

}t_archivo;

typedef struct{

	char * nombre;
	int padre;

}t_directorio;

typedef struct{
	int tamaniobloques;
	int puertolisten;
	int cantnodos;
	char ** vecnodos;

}t_configuracion;

void * lanzarConsola();
void cargarconfig();

#endif /* MDFS_H_ */
