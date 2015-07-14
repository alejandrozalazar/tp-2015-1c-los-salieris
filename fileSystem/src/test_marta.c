/*
 * test_marta.c
 *
 *  Created on: 12/07/2015
 *      Author: utnso
 */

#include "fileSystem.h"

t_log* loggerFS;
t_list * listaNodos;

t_nodo* new_nodo(char* nombre, char* ip, int puerto){
	t_nodo* new_nodo = calloc(1, sizeof(t_nodo));
	new_nodo->puerto = puerto;
	strcpy(new_nodo->ip, ip);
	strcpy(new_nodo->nombre, nombre);
	new_nodo->disponible = true;
	return new_nodo;
}

void mock_agregar_nodos(){
	list_add_in_index(listaNodos, 0, new_nodo("Nodo1", "127.0.0.1", 3001));
	list_add_in_index(listaNodos, 1, new_nodo("Nodo2", "127.0.0.1", 3002));
	list_add_in_index(listaNodos, 2, new_nodo("Nodo3", "127.0.0.1", 3003));
	list_add_in_index(listaNodos, 3, new_nodo("Nodo4", "127.0.0.1", 3004));
	list_add_in_index(listaNodos, 4, new_nodo("Nodo5", "127.0.0.1", 3005));
}

t_nodo mock_obtener_nodo(int i){
	log_debug(loggerFS, "mock_obtener_nodo: buscamos el nodo %d", i);
	t_nodo nodo;
	memcpy(&nodo, list_get(listaNodos, i), sizeof(t_nodo));
	return nodo;
}

void mock_obtener_bloques_nodo(t_bloque_nodo* bloques, int i){
	log_debug(loggerFS, "mock_obtener_bloques_nodo");

	switch (i) {
		case 0:
			bloques[0].nro_bloque = 6;
			bloques[1].nro_bloque = 9;
			bloques[2].nro_bloque = 11;
			bloques[0].nodo = mock_obtener_nodo(0);
			bloques[1].nodo = mock_obtener_nodo(1);
			bloques[2].nodo = mock_obtener_nodo(2);

			break;
		case 1:
			bloques[0].nro_bloque = 6;
			bloques[1].nro_bloque = 9;
			bloques[2].nro_bloque = 11;
			bloques[0].nodo = mock_obtener_nodo(3);
			bloques[1].nodo = mock_obtener_nodo(4);
			bloques[2].nodo = mock_obtener_nodo(0);

			break;
		case 2:
			bloques[0].nro_bloque = 7;
			bloques[1].nro_bloque = 4;
			bloques[2].nro_bloque = 33;
			bloques[0].nodo = mock_obtener_nodo(3);
			bloques[1].nodo = mock_obtener_nodo(2);
			bloques[2].nodo = mock_obtener_nodo(3);

			break;
		default:
			break;
	}
}

void mock_obtener_bloques_archivo(t_bloque_archivo* bloques){
	log_debug(loggerFS, "mock_obtener_bloques_archivo");
	int i;
	for(i = 0; i < 3; i++){
		memset(&bloques[i], 0, sizeof(t_bloque_archivo));
		bloques[i].nro_bloque = i;
		mock_obtener_bloques_nodo(bloques[i].bloques_nodo, i);
	}
}

// TODO: mejorar esta función! no tiene que estar harcodeada!
// es mas. desde la tabla de archivos con solo el nombre debería devolver directamente la lista de bloques!
void buscar_bloques_y_responder(int socketMarta, char* contenido){
	log_debug(loggerFS, "buscar_bloques_y_responder");
	t_bloque_archivo bloques[3];
	mock_obtener_bloques_archivo(bloques);

	log_debug(loggerFS, "mando ack. tamanio %d", sizeof(bloques));
	//enviar_ack(socketMarta, sizeof(bloques), 3);
	log_debug(loggerFS, "estoy enviando los bloques del archivo! tamanio %d", sizeof(bloques));
	enviar(socketMarta, (char*)bloques, sizeof(bloques));
}
