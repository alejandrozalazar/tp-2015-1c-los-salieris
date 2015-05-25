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

#endif /* FILESYSTEM_H_ */
