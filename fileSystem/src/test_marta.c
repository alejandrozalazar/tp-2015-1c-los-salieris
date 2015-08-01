/*
 * test_marta.c
 *
 *  Created on: 12/07/2015
 *      Author: utnso
 */

#include "fileSystem.h"

t_nodo* new_nodo(char* nombre, char* ip, int puerto){
	t_nodo* new_nodo = calloc(1, sizeof(t_nodo));
	new_nodo->puerto = puerto;
	strcpy(new_nodo->ip, ip);
	strcpy(new_nodo->nombre, nombre);
	new_nodo->disponible = true;
	return new_nodo;
}

void mock_agregar_nodos(){
//	list_add_in_index(listaNodos, 0, new_nodo("Nodo1", "127.0.0.1", 3001));
//	list_add_in_index(listaNodos, 1, new_nodo("Nodo2", "127.0.0.1", 3002));
//	list_add_in_index(listaNodos, 2, new_nodo("Nodo3", "127.0.0.1", 3003));
//	list_add_in_index(listaNodos, 3, new_nodo("Nodo4", "127.0.0.1", 3004));
//	list_add_in_index(listaNodos, 4, new_nodo("Nodo5", "127.0.0.1", 3005));
}

t_nodo mock_obtener_nodo(int i){
	log_debug(loggerFS, "mock_obtener_nodo: buscamos el nodo %d", i);
//	t_nodo nodo;
//	memcpy(&nodo, list_get(listaNodos, i), sizeof(t_nodo));
//	return nodo;
	t_nodo_self* nodo = malloc(sizeof(t_nodo));
	t_list* listaNodos = list_create();
	memcpy(nodo, list_get(nodos, i), sizeof(t_nodo));
	t_nodo* tnodo = convertir_t_nodo_self_a_t_nodo(nodo);
	return *tnodo;
}

t_nodo* convertir_t_nodo_self_a_t_nodo(t_nodo_self* nodoSelf) {

	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->estado = nodoSelf->estado;
//	nodo->nombre = string_duplicate(nombre);
	memcpy(nodo->nombre, nodoSelf->nombre, NAME_SIZE+1);
	nodo->nodo_id = string_duplicate(nodoSelf->nodo_id);
	nodo->bloques_disponibles = nodoSelf->bloques_disponibles;
	nodo->bloques = dictionary_create();

	nodo->carga = nodoSelf->carga;
	nodo->disponible = nodoSelf->disponible;
	nodo->fd = nodoSelf->fd;
	memcpy(nodo->ip, nodoSelf->ip, IP_SIZE+1);
//	nodo->nombre = nombre;
	nodo->puerto = nodoSelf->puerto;
	return nodo;
}

t_nodo mock_obtener_nodo_viejo(int i){
	log_debug(loggerFS, "mock_obtener_nodo: buscamos el nodo %d", i);
//	t_nodo nodo;
//	memcpy(&nodo, list_get(listaNodos, i), sizeof(t_nodo));
//	return nodo;
	t_nodo* nodo = malloc(sizeof(t_nodo));
	t_list* listaNodos = list_create();
	memcpy(nodo, list_get(listaNodos, i), sizeof(t_nodo));
	return *nodo;
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
//	mock_obtener_bloques_archivo(bloques);
	t_archivo_self* archivo = find_archivo(contenido, archivos);
	if (archivo->estado == VALIDO) {
		int var;
		for (var = 0; var < archivo->cantidad_bloques; ++var) {
			char* key = string_itoa(var);
			t_bloque_archivo_self* self = dictionary_get(archivo->bloques, key);
			bloques[var] = create_t_bloque_archivo(self);
		}

		log_debug(loggerFS, "mando ack. tamanio %d", sizeof(bloques));
		enviar_ack(socketMarta, sizeof(bloques), 3);
		log_debug(loggerFS, "estoy enviando los bloques del archivo! tamanio %d", sizeof(bloques));
		enviar(socketMarta, (char*)bloques, sizeof(bloques));
	}
}
t_bloque_archivo create_t_bloque_archivo(t_bloque_archivo_self* self) {
	t_bloque_archivo b_archivo;
	b_archivo.mapeado = self->estado;
	b_archivo.nro_bloque = self->numero;
	int var;
	for (var = 0; var < 3; var++) {
		t_bloque_nodo* bloqueNodo = create_t_bloque_nodo(list_get(self->copias, var));
		b_archivo.bloques_nodo[var] = *bloqueNodo;
	}
	return b_archivo;
}

t_bloque_nodo* create_t_bloque_nodo(t_copia_self* copia_self) {
	if(copia_self) {
		t_bloque_nodo bloqueNodo;
		bloqueNodo.nro_bloque = copia_self->nodo_bloque->bloque_nodo;
		t_nodo_self* nodo_self = copia_self->nodo_bloque->nodo;
		t_nodo* nodo = crear_t_nodo(nodo_self->nombre, nodo_self->estado, nodo_self->nodo_id, nodo_self->bloques_disponibles, nodo_self->carga, nodo_self->disponible, nodo_self->fd, nodo_self->ip, nodo_self->puerto, nodo_self->cantidad_bloques);
		bloqueNodo.nodo = *nodo;
		return &bloqueNodo;
	}
	return NULL;
}
