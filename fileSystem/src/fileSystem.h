#ifndef FILESYSTEM_H_

#define FILESYSTEM

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <mensajeria/mensajes.h>
#include <mensajeria/sockets.h>

void cargarConfiguracion(char* pathArchiConf);
struct sockaddr_in* createSockAddr(char* sIPyPuerto);
int escucharNuevasConexiones(int usPuerto);
void obtenerComando(char inputBuffer[], char *args[]);

#define BACKLOG 10 // Cuántas conexiones pendientes se mantienen en cola
#define STDIN 0  // Descriptor de fichero de la entrada estándar
#define MAX_LINE 100 // 100 chars por linea, por comando

#endif /* FILESYSTEM_H_ */
