#ifndef FILESYSTEM_H_

#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <mensajeria/mensajes.h>
#include <mensajeria/sockets.h>
#include "ABM.h"

int cargarConfiguracion(char* pathArchiConf, int* iPuertoFS, int* iCantNodosMinima);
void obtenerComando(char inputBuffer[], char *args[]);
int seDesconectoUnNodo(int fdNodo, t_list* listaNodos, int iCantNodosMinima, int* isFSOperativo);
int verStatus(int iCantNodosMinima, t_list* listaNodos);

#define LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_EN_CONSOLA true
#define BACKLOG 10 // Cuántas conexiones pendientes se mantienen en cola
#define STDIN 0  // Descriptor de fichero de la entrada estándar
#define MAX_LINE 100 // 100 chars por linea, por comando


#endif /* FILESYSTEM_H_ */
