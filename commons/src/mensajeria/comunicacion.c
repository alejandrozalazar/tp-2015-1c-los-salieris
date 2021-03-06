#include "comunicacion.h"

header_t* crearHeader() {
	header_t *nuevoHeader;
	nuevoHeader = calloc(1, sizeof(header_t));
	return nuevoHeader;
}

void initHeader(header_t* header) {
	memset(header, '\0', sizeof(header_t));
}

int enviar(int sock, char *buffer, int tamano)
{
	int escritos;

	if ((escritos = send (sock, buffer, tamano, 0)) <= 0)
	{
		printf("Error en el send\n\n");
		return WARNING;
	}
	else if (escritos != tamano)
	{
		printf("La cantidad de bytes enviados es distinta de la que se quiere enviar\n\n");
		return WARNING;
	}

	return EXITO;
}


int recibir(int sock, char *buffer, int tamano)
{
	int val;
	int leidos = 0;

	memset(buffer, '\0', tamano);

	while (leidos < tamano)
	{
		//printf("while leidos %d, tamano %d\n", leidos, tamano);
		val = recv(sock, buffer + leidos, tamano - leidos, 0);
		//printf("while val %d\n", val);
		if (val < 0)
		{
			printf("Error al recibir datos: %d - %s\n", val, strerror(val)); //ENOTCONN ENOTSOCK
			switch(val) {
				// The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.  POSIX.1-2001 allows either error to be returned for this
				// case, and does not require these constants to have the same value, so a portable application should check for both possibilities.
				//case EAGAIN: printf(" - EAGAIN \n The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.\n\n"); break;
				//case EWOULDBLOCK: printf("EWOULDBLOCK \n The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.\n\n"); break;
				// The argument sockfd is an invalid descriptor.
				case EBADF: printf("EBADF \n The argument sockfd is an invalid descriptor.\n\n"); break;
				// A remote host refused to allow the network connection (typically because it is not running the requested service).
				case ECONNREFUSED: printf("ECONNREFUSED \n A remote host refused to allow the network connection (typically because it is not running the requested service).\n\n"); break;
				// The receive buffer pointer(s) point outside the process's address space.
				case EFAULT: printf("EFAULT \n The receive buffer pointer(s) point outside the process's address space.\n\n"); break;
				// The receive was interrupted by delivery of a signal before any data were available; see signal(7).
				case EINTR: printf("EINTR \n The receive was interrupted by delivery of a signal before any data were available; see signal(7).\n\n"); break;
				// Invalid argument passed.
				case EINVAL: printf("EINVAL \n Invalid argument passed.\n\n"); break;
				// Could not allocate memory for recvmsg().
				case ENOMEM: printf("ENOMEM \n Could not allocate memory for recvmsg().\n\n"); break;
				// The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).
				case ENOTCONN: printf("ENOTCONN \n The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).\n\n"); break;
				// The argument sockfd does not refer to a socket.
				case ENOTSOCK: printf("ENOTSOCK \n The argument sockfd does not refer to a socket.\n\n"); break;

			}
			return ERROR;
		} else if(val == 0)
		// Cuando recv devuelve 0 es porque se desconecto el socket.
		{
			/*printf("%d se desconecto\n", sock);*/
			return WARNING;
		} else {
			leidos += val;
		}
	}
	return EXITO;
}

int conectar(t_ip ip, int puerto, int *sock)
{
	struct sockaddr_in dirCent;
	//char *bufferMsg=NULL;
	//char *bufferMsg2=NULL;

	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Error al crear el socket\n\n");
		return ERROR;
	}

	dirCent.sin_family = AF_INET;
	dirCent.sin_port = htons(puerto);
	dirCent.sin_addr.s_addr = inet_addr(ip);
	memset(&(dirCent.sin_zero), '\0', 8);

	if (connect(*sock, (struct sockaddr *)&dirCent, sizeof(struct sockaddr)) == -1)
	{
		printf("Imposible conectar\n\n");
		return ERROR;
	}
	return EXITO;

}

int aceptar_conexion(int *listener, int *nuevo_sock)
{
	struct sockaddr_in dirRemota;
	//size_t dirLong;
	socklen_t dirLong;
	dirLong = sizeof(dirRemota);

	if ((*nuevo_sock = accept(*listener, (struct sockaddr *)&dirRemota, &dirLong)) == -1)
	{
		puts("error accept");
		return ERROR;
	}

	return EXITO;
}

int enviar_struct (int sock, void* myStruct, size_t structSizeOf) {
	int ret;
	void *buffer = calloc(1,sizeof(structSizeOf));
	memcpy(buffer, myStruct, structSizeOf);

	if ((ret = enviar(sock, buffer, structSizeOf)) != EXITO)
	{
		printf("enviar_struct: ERROR al enviar enviar_struct al fd %d\n\n", sock);
		free(buffer);
		return WARNING;
	}

	free(buffer);

	return ret;
}


int recibir_struct(int sock, void* myStruct, size_t structSizeOf)
{
	int ret;
	void *buffer = NULL;
	//char strAux[50];

	buffer = calloc(1, structSizeOf);

	ret = recibir(sock, buffer, structSizeOf);

	if (ret == ERROR) {
		free(buffer);
		//return trazarError(errorTrace, "Error al recibir datos :S", ERROR,"comunicacion.h", "recibirHeader()");
		return ERROR;
	}

//	memset(myStruct, '\0', structSizeOf);
	memcpy(myStruct, buffer, structSizeOf); /*ojo que el memcopy si lo haces afuera el primer parametro tiene que tener &*/
	/* Por ejemplo si la estructua no fuera por referencia y fuera local, debes hacer asi:
	memcpy(&header, buffer, sizeof(header_t));*/

	//	printf("sock: %d --- largo: %d ---- ", sock, header->largo_mensaje);
	//	printf("tipo: %d\n", header->tipo);
	free(buffer);

	return EXITO;

}

int enviar_header (int sock, header_t *header) {
	int ret;
	char *buffer = calloc(1,sizeof(header_t));
	memcpy(buffer, header, sizeof(header_t));

	if ((ret = enviar(sock, buffer, sizeof(header_t))) != EXITO)
	{
		printf("enviar_header: ERROR al enviar header_t al fd %d\n\n", sock);
		free(buffer);
		return WARNING;
	}

	free(buffer);

	return ret;
}

int recibir_header(int sock, header_t *header, fd_set *master/*por si se desconecta*/, int *seDesconecto)
{
	int ret;
	char *buffer = NULL;
	//char strAux[50];

	buffer = calloc(1, sizeof(header_t));
	*seDesconecto = false; /*False =0 define*/

	ret = recibir(sock, buffer, sizeof(header_t));

	// WARNING es cuando se desconecto el socket.
	if (ret == WARNING) {
		//	sprintf(strAux, "Se desconecto el socket: %d\n", sock);
		FD_CLR(sock, master);
		close(sock);
		*seDesconecto = true;
		free(buffer);
		return EXITO;
	}

	if (ret == ERROR) {
		free(buffer);
		//return trazarError(errorTrace, "Error al recibir datos :S", ERROR,"comunicacion.h", "recibirHeader()");
		return ERROR;
	}

	memset(header, '\0', sizeof(header_t));
	memcpy(header, buffer, sizeof(header_t)); /*ojo que el memcopy si lo haces afuera el primer parametro tiene que tener &*/
	/* Por ejemplo si la estructua no fuera por referencia y fuera local, debes hacer asi:
	memcpy(&header, buffer, sizeof(header_t));*/

	//	printf("sock: %d --- largo: %d ---- ", sock, header->largo_mensaje);
	//	printf("tipo: %d\n", header->tipo);
	free(buffer);

	return EXITO;

}

int recibir_header_simple(int sock, header_t *header)
{
	int ret;
	char *buffer = NULL;

	buffer = calloc(1, sizeof(header_t));

	ret = recibir(sock, buffer, sizeof(header_t));

	if (ret != EXITO)
		return ret;

	memset(header, '\0', sizeof(header_t));
	memcpy(header, buffer, sizeof(header_t));

	free(buffer);

	return EXITO;

}

int quitar_descriptor(int desc, fd_set *listaDesc, int *maxDesc)
{
	FD_CLR(desc, listaDesc);
	close(desc);

	//	if (desc == *maxDesc)
	//	{
	//		*maxDesc = *maxDesc-1;
	//	}
	return EXITO;
}

int agregar_descriptor(int desc, fd_set *listaDesc, int *maxDesc)
{

	FD_SET(desc, listaDesc);
	if (desc > *maxDesc)
	{
		*maxDesc = desc;
	}
	return EXITO;
}


int crear_listener(int puerto, int *listener)
{
	struct sockaddr_in miDir;
	int yes = 1;
	char reintentarConex = 1;

	if ((*listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Error al crear el Socket\n\n");
		return ERROR;
	}

	if (setsockopt(*listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		puts("Error en setsockopt()");
		return ERROR;
	}
	for(;reintentarConex;)
	{
		miDir.sin_family = AF_INET;
		miDir.sin_addr.s_addr = INADDR_ANY;
		miDir.sin_port = htons(puerto);
		memset(&(miDir.sin_zero), '\0', 8);

		if (bind(*listener, (struct sockaddr*)&miDir, sizeof(miDir)) == -1)
		{
			// *puerto++; tratamiento de error
			reintentarConex = 0;
		}
	}
	listen(*listener, 10);

	return EXITO;
}

void genId(char idMsg[])
{
	time_t tTiempo;
	//header_t msgId;

	int x;

	/*******Planto la semilla***********/
	tTiempo = time(&tTiempo) + (getpid() * 20);
	srand(tTiempo);

	for (x=0;x<15;x++)
		idMsg[x]= rand() % 25 + 97;

	idMsg[15]='\0';
	//strncpy(idMsg, idMsg, sizeof(idMsg));
}

int enviar_string(int sock, char* string)
{
	int tamanio = strlen(string +1), ret;
	char* buffer_payload = calloc(1, tamanio);
	memcpy(buffer_payload, string, tamanio);

	if ((ret = enviar(sock, buffer_payload, tamanio)) != EXITO)
	{
		printf("enviar_nivel: ERROR al enviar string al fd %d\n\n", sock);
		free(buffer_payload);
		return WARNING;
	}

	free(buffer_payload);

	return ret;
}

int recibir_string(int sock, char* string, int tamanio)
{
	int ret;
	char* buffer = calloc(1, tamanio);

	ret = recibir(sock, buffer, tamanio);

	if (ret == WARNING) {
		close(sock);
		free(buffer);
		return WARNING;
	}

	if (ret == ERROR) {
		free(buffer);
		return ERROR;
	}

	memcpy(string, buffer, tamanio);

	free(buffer);

	return EXITO;
}

int enviar_map_request(int sock, t_map_request* map_request)
{
	int ret;
	char *buffer_payload = calloc(1,sizeof(t_map_request));
	memcpy(buffer_payload, map_request, sizeof(t_map_request));

	if ((ret = enviar(sock, buffer_payload, sizeof(t_map_request))) != EXITO)
	{
		printf("enviar_nivel: ERROR al enviar t_map_request al fd %d\n\n", sock);
		free(buffer_payload);
		return WARNING;
	}

	free(buffer_payload);

	return ret;

	return EXITO;
}

int recibir_map_request(int sock, t_map_request* map_request)
{
	int ret;
	char *buffer = NULL;

	buffer = calloc(1, sizeof(t_map_request));

	//printf("Espero recibir t_nivel (%u)", sizeof(t_nivel));
	ret = recibir(sock, buffer, sizeof(t_map_request));

	if (ret == WARNING) {
		close(sock);
		free(buffer);
		return WARNING;
	}

	if (ret == ERROR) {
		free(buffer);
		return ERROR;
	}

	memcpy(map_request, buffer, sizeof(t_map_request));

	free(buffer);

	return EXITO;
}

int enviar_map_request_nodo(int sock, t_map_request_nodo* request)
{
	int ret;
	char *buffer_payload = calloc(1,sizeof(t_map_request_nodo));
	memcpy(buffer_payload, request, sizeof(t_map_request_nodo));

	if ((ret = enviar(sock, buffer_payload, sizeof(t_map_request_nodo))) != EXITO)
	{
		printf("enviar_nivel: ERROR al enviar t_map_request al fd %d\n\n", sock);
		free(buffer_payload);
		return WARNING;
	}

	free(buffer_payload);

	return ret;

	return EXITO;
}

int recibir_map_request_nodo(int sock, t_map_request_nodo* request, bool* seDesconecto)
{
	int ret;
	char *buffer = NULL;

	buffer = calloc(1, sizeof(t_map_request_nodo));
	*seDesconecto = false; /*False =0 define*/

	ret = recibir(sock, buffer, sizeof(t_map_request_nodo));

	if (ret == WARNING) {
		close(sock);
		*seDesconecto = true;
		free(buffer);
		return WARNING;
	}

	if (ret == ERROR) {
		free(buffer);
		return ERROR;
	}

	memcpy(request, buffer, sizeof(t_map_request_nodo));

	free(buffer);

	return EXITO;
}

int enviar_t_mensaje(int sock, t_header tipo, size_t tamanio, char* contenido)
{
	t_mensaje request;
	memset(&request, 0, sizeof(t_mensaje));
	request.tipo = tipo;
	request.tamanio = tamanio;
	strcpy(request.contenido, contenido);

	int ret;
	char *buffer_payload = calloc(1,sizeof(t_mensaje));
	memcpy(buffer_payload, &request, sizeof(t_mensaje));

	if ((ret = enviar(sock, buffer_payload, sizeof(t_mensaje))) != EXITO)
	{
		printf("enviar_nivel: ERROR al enviar t_mensaje al fd %d\n\n", sock);
		free(buffer_payload);
		return WARNING;
	}

	free(buffer_payload);

	return ret;

	return EXITO;
}

int recibir_t_mensaje(int sock, t_mensaje* request)
{
	int ret;
	char *buffer = NULL;

	buffer = calloc(1, sizeof(t_mensaje));

	ret = recibir(sock, buffer, sizeof(t_mensaje));

	if (ret == WARNING) {
		close(sock);
		free(buffer);
		return WARNING;
	}

	if (ret == ERROR) {
		free(buffer);
		return ERROR;
	}

	memcpy(request, buffer, sizeof(t_mensaje));

	free(buffer);

	return EXITO;
}
int enviar_ack(int sock, int tamanio, int cantidad) {
	header_t header;
	memset(&header, 0, sizeof(header_t));

	header.tipo = ACK;
	header.largo_mensaje = tamanio;
	header.cantidad_paquetes = cantidad;
	return enviar_header(sock, &header);
}

int enviar_nak(int sock) {
	header_t header;
	memset(&header, 0, sizeof(header_t));

	header.tipo = NAK;
	return enviar_header(sock, &header);
}
