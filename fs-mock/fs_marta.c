/*
 * fs.c
 *
 *  Created on: 06/06/2015
 *      Author: utnso
 */

#include "filesystem.h"

#define PUERTO 3000
#define LOGGER "fs-mock.log"

int fs_marta(){

	t_log* logger = log_create(LOGGER, "fs-mock", true, LOG_LEVEL_INFO);
	bool termina = false;
	struct sockaddr_in my_addr, their_addr;
	int socketFD, socketCliente;
	socketFD = crearSocket();

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PUERTO);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', sizeof(struct sockaddr_in));

	bindearSocket(socketFD, my_addr);

	escucharEn(socketFD);
	socklen_t sin_size;
	sin_size = sizeof(struct sockaddr_in);

	if((socketCliente = accept(socketFD,(struct sockaddr *)&their_addr, &sin_size)) == -1)
	{
		perror("accept");
		return EXIT_FAILURE;
	}

	log_trace(logger, "Recibí conexion de %s", inet_ntoa(their_addr.sin_addr));

	while(!termina){
		t_mensaje mensaje;
		t_header header = recibirDeserializado(logger, socketCliente, false, &mensaje);

		if(header == JOB_TO_MARTA_FILES){
			puts("llegamos! y respondemos!");
			size_t tamanio = 4 * sizeof(size_t) + 9 * sizeof(t_bloque_nodo);
			char* payload = serializarContenido(tamanio);
			enviarSerializado(logger, socketCliente, false, MARTA_TO_JOB_FILE_FOUND, tamanio, payload);

		}


		termina = false;

	}

	log_info(logger, "termino el mock");
	return EXIT_SUCCESS;
}

char* serializarContenido(size_t tamanio){
	char *serializedPackage = malloc(tamanio);
	size_t cantidad = 3;
	t_bloque_nodo bloques[9];

	int i;
	for(i = 0; i < 9; i++){
		bloques[i] = generar_bloque_nodo(i);
	}

	int offset = 0;
	int size_to_send;

	size_to_send =  sizeof(size_t);
	memcpy(serializedPackage + offset, &cantidad, size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(size_t);
	memcpy(serializedPackage + offset, &cantidad, size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[0], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[1], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[2], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(size_t);
	memcpy(serializedPackage + offset, &cantidad, size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[3], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[4], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[5], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(size_t);
	memcpy(serializedPackage + offset, &cantidad, size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[6], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[7], size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(t_bloque_nodo);
	memcpy(serializedPackage + offset, &bloques[8], size_to_send);
	offset += size_to_send;

	return serializedPackage;
}

t_bloque_nodo generar_bloque_nodo(int i){
	t_bloque_nodo bloque;
	switch(i){
		case 0:
			bloque.nro_bloque = 6;
			bloque.nodo = generar_nodo(1);
		break;
		case 1:
			bloque.nro_bloque = 9;
			bloque.nodo = generar_nodo(2);
			break;
		case 2:
			bloque.nro_bloque = 11;
			bloque.nodo = generar_nodo(3);
			break;
		case 3:
			bloque.nro_bloque = 6;
			bloque.nodo = generar_nodo(4);
			break;
		case 4:
			bloque.nro_bloque = 9;
			bloque.nodo = generar_nodo(5);
			break;
		case 5:
			bloque.nro_bloque = 11;
			bloque.nodo = generar_nodo(1);
			break;
		case 6:
			bloque.nro_bloque = 7;
			bloque.nodo = generar_nodo(4);
			break;
		case 7:
			bloque.nro_bloque = 4;
			bloque.nodo = generar_nodo(3);
			break;
		case 8:
			bloque.nro_bloque = 33;
			bloque.nodo = generar_nodo(4);
			break;
	}
	return bloque;
}

t_nodo generar_nodo(int  i){
	t_nodo nodo;
	switch(i){
		case 1:
			strcpy(nodo.nombre_nodo, "Nodo1");
			strcpy(nodo.ip_nodo, "127.0.0.1");
			nodo.puerto_nodo = 5001;
			break;
		case 2:
			strcpy(nodo.nombre_nodo, "Nodo2");
			strcpy(nodo.ip_nodo, "127.0.0.1");
			nodo.puerto_nodo = 5002;
			break;
		case 3:
			strcpy(nodo.nombre_nodo, "Nodo3");
			strcpy(nodo.ip_nodo, "127.0.0.1");
			nodo.puerto_nodo = 5003;
			break;
		case 4:
			strcpy(nodo.nombre_nodo, "Nodo4");
			strcpy(nodo.ip_nodo, "127.0.0.1");
			nodo.puerto_nodo = 5004;
			break;
		case 5:
			strcpy(nodo.nombre_nodo, "Nodo5");
			strcpy(nodo.ip_nodo, "127.0.0.1");
			nodo.puerto_nodo = 5005;
			break;
	}
	return nodo;
}
