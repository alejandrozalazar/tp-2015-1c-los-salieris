
#ifndef SOCKETS_H
#define SOCKETS_H

#include <string.h>

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

/*Commons includes*/
#include "../commons/string.h"
#include "../commons/log.h"

#include "mensajes.h"

#define MSG_SIZE 256
#define STRUCT_SIZE (MSG_SIZE + sizeof(t_header))
#define BACKLOG 10 // cantidad máxima de sockets a mantener en pool de espera
#define WARNING 2

typedef struct sockaddr_in tSocketInfo;
typedef struct sockaddr_in t_socket_info;

char* getDescription(int item);

/* Defino los tipos de señales que se pueden mandar
 *
 * Sintaxis correcta para escribir una nueva señal:
 *
 * 	#define [Origen]_TO_[Destino]_[Mensaje] numeroIncremental
 *
 * 	Donde:
 * 		[Origen] es quien manda el mensaje
 * 		[Destino] es quien recive el mensaje
 * 		[Mensaje] lo que se le quiere mandar
 * 		numeroIncrementar un numero mas que la señal anterior
 */

typedef char t_contenido[MSG_SIZE];

typedef struct ts_mensajes {
	t_header id;
	t_contenido contenido;
}t_mensajes;


void *get_in_addr(struct sockaddr *sa);

/*
 * crearSocket: Crea un nuevo socket.
 * @return: El socket creado.
 * 			EXIT_FAILURE si este no pudo ser creado.
 */
int crearSocket();


/*
 * bindearSocket: Vincula un socket con una direccion de red.
 *
 * @arg socket:		Socket a ser vinculado.
 * @arg socketInfo: Protocolo, direccion y puerto de escucha del socket.
 * @return:			0 si el socket ha sido vinculado correctamente.
 * 					EXIT_FAILURE si se genera error.
 */
int bindearSocket(int unSocket, tSocketInfo socketInfo);


/*
 * escucharEn: Define el socket de escucha.
 *
 * @arg socket:		Socket de escucha.
 * @return:			EXIT_SUCCESS si la conexion es exitosa.
 * 					EXIT_FAILURE si se genera error.
 */
int escucharEn(int unSocket);


/*
 * conectarServidor: Crea una conexion a un servidor como cliente.
 *
 * @arg puertoDestino: Puerto de escucha del servidor.
 * @arg ipDestino: 	   IP del servidor.
 * @return: 		   int: Socket de conexión con el server.
 *					   EXIT_FAILURE: No se pudo conectar.
 */
int conectarAServidor(char *ipDestino, unsigned short puertoDestino);


/*
 * desconectarseDe: Se desconecta el socket especificado.
 *
 * @arg socket: Socket al que estamos conectados.
 * @return:		EXIT_SUCCESS: La desconexion fue exitosa.
 * 				EXIT_FAILURE: Se genero error al desconectarse.
 */
int desconectarseDe(int socket);

/*
 * Funcion enviar mensaje, recibe como parámetros:
 * socket, el header y el mensaje propiamente dicho
 * devuelve el numero de bytes enviados
 */
int enviarMensaje(int numSocket, t_header header, t_contenido mensaje, t_log *logger);

int enviar(int sock, char *buffer, int tamano);

int recibir(int sock, void *buffer, int tamano);
/*
 * Funcion recibir mensaje, recive como parámetros:
 * socket y mensaje donde se pondŕa en mensaje,
 * y devuelve el header, para evaluar la accion a tomar.
 */

t_header recibirMensaje(int numSocket, t_contenido mensaje, t_log *logger);

int enviarSerializado(t_log* logger, int socket, bool esTexto, t_header header, size_t tamanio, char* contenido);

t_header recibirDeserializado(t_log *logger, int socketCliente, bool esTexto, t_mensaje* package);

/*
 * Funcion cerrar un socket, y eliminarlo de un fd_set[opcional]:
 * retorna EXIT_SUCCESS si se realizó bien, EXIT_FAILURE sino.t_header recibirCodigo(int numSocket, t_header authorizedHeader, char** stringCode, t_log *logger)
 * ej: cerrarSocket(mySocket, &read_fds);t_header recibirCodigo(int numSocket, t_header authorizedHeader, char**t_header recibirCodigo(int numSocket, t_header authorizedHeader, char** stringCode, t_log *logger) stringCode, t_log *logger)
 */
int cerrarSocket(int numSocket, fd_set* fd);

void freeMensaje(t_mensaje* mensaje);

#endif
