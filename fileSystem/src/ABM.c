#include "ABM.h"

#define filename "/home/utnso/Escritorio/Nuevo2.txt"

#define VALIDO 1
#define INVALIDO 0

/*
 * Busca un t_archivo en una lista de archivos.
 */
t_archivo_self* find_archivo(char* nombre, t_list* archivosAux) {
	int i;
	for (i = 0; i < list_size(archivosAux); i += 1) {
		t_archivo_self * archivo = list_get(archivosAux, i);
		if (string_equals_ignore_case(archivo->nombre, nombre)) {
			return archivo;
		}
	}
	return NULL;
}

/*
 * Busca un t_nodo_tp que sea el que mas espacio disponible tenga en la lista de nodos.
 * TODO: Falta verificar que una copia del mismo bloque de archivo no este en ese nodo.
 */
t_nodo_self* find_nodo_disponible(char* nombre, int numero_bloque1) {
	int espacio_disponible = 0;
	int i;
	t_nodo_self* selecto = NULL;
	for (i = 0; i < list_size(nodos); i++) {
		t_nodo_self * nodo = list_get(nodos, i);
		if (nodo->bloques_disponibles > espacio_disponible
				&& nodo->estado == 1) {
			espacio_disponible = nodo->bloques_disponibles;
			selecto = nodo;
		}
	}

	return selecto;
}

/*
 * Devuelve que bloque de nodo esta disponible y lo quita de la lista
 * de nodos disponibles.
 */
int get_bloque_disponible(t_nodo_self* nodo) {
	t_list* lista = ((t_list*) dictionary_get(bloques_nodo_disponible,
			nodo->nombre));

	if (list_size(lista)) {
		int disponible = list_get(lista, 0);
		list_remove(lista, 0);
		return disponible;
	}

	return -1;
}

/*
 * Busca un t_nodo_tp en la lista de nodos.
 */
t_nodo_self* find_nodo(char* nombre) {
	int i;
	for (i = 0; i < list_size(nodos); i += 1) {
		t_nodo_self * nodo = list_get(nodos, i);
		if (nodo->nombre == nombre) {
			return nodo;
		}
	}
	return NULL;
}

/*
 *Agrega un t_nodo_tp nuevo a la lista de nodos.
 */
void add_nodo_to_nodos(t_nodo_self* nodo) {
	list_add(nodos, nodo);
	iniciar_disponibles(nodo->nombre);
	t_list* bloque_archivo_control = list_create();
	dictionary_put(bloques_nodos_archivos, nodo->nombre,
			bloque_archivo_control);
}

/*
 * Inicializa la lista de control de bloques por nodo.
 */
void iniciar_disponibles(char* nombre) {
	int i;
	t_list * lista = list_create();
	for (i = 1; i <= CANTIDAD_BLOQUES_NODO_DEFAULT; i++) {
		int* numero;
		numero = malloc(sizeof(int));
		numero = (int) i;
		list_add(lista, numero);
	}

	dictionary_put(bloques_nodo_disponible, nombre, lista);
}

/*
 * Setea como NO disponible a un archivo.
 */
t_archivo_self* archivo_no_disponible(t_archivo_self* archivo) {
	return archivo->estado == INVALIDO;
}

//t_list* ArchivosNoDisponibles(t_list* archivosAux) {
//	t_list* noDisponibles = list_create();
//	int i;
//	for (i = 0; i < list_size(archivosAux); i++) {
//		if (ArchivoNoDisponible(list_get(archivosAux, i))) {
//			list_add(noDisponibles);
//		}
//	}
//	return noDisponibles;
//}

/*
 * Valida si un archivo que estaba inactivado, pasa a estar activo
 * gracias a la validacion de una copia de un bloque dado.
 */
void chequear_alta_archivo(t_archivo_self* archivo, int numero_bloque) {
	if (archivo->estado == INVALIDO) {
		t_bloque_archivo_self* bloque = dictionary_get(archivo->bloques,
				string_itoa(numero_bloque));

		if (bloque->estado == INVALIDO) {
			t_list* copias = bloque->copias;

			int i;
			for (i = 0; i < list_size(copias); i++) {
				t_copia_self* copia = list_get(copias, i);
				if (copia->nodo_bloque->nodo->estado == VALIDO) {
					bloque->estado = VALIDO;
					archivo->bloques_invalidos -= 1;
					break;
				}
			}

			if (archivo->bloques_invalidos == archivo->cantidad_bloques) {
				archivo->estado = VALIDO;
			}
		}
	}
}

/*
 * Activa el nodo y busca los archivos utilizados por el nodo reactivado y
 * si estan invalidos verifica si deben ser dados de alta.
 */
void reactivar_nodo(t_nodo_self* nodo) {
	if (nodo != NULL) {
		int i;
		nodo->estado = 1;
		t_list* archivos_nodo = dictionary_get(bloques_nodos_archivos,
				nodo->nombre);

		for (i = 0; i < list_size(archivos_nodo); i++) {
			t_bloque_archivo_control_self* bloque_archivo_control = list_get(
					archivos_nodo, i);
			t_archivo_self * archivo = find_archivo(
					bloque_archivo_control->nombre_archivo, archivos);
			chequear_alta_archivo(archivo,
					bloque_archivo_control->archivo_bloque);
		}
	}
}

/*
 * Agrega o reactiva un nodo que ingreso al FS.
 */
void alta_nodo(char* nombre) {
	t_nodo_self* nodoAux = find_nodo(nombre);
	if (nodoAux == NULL) {
		t_nodo_self* nodo;
		nodo = malloc(sizeof(t_nodo_self));
		nodo->estado = 1;
		nodo->nombre = nombre;
		nodo->bloques_disponibles = CANTIDAD_BLOQUES_NODO_DEFAULT;
		add_nodo_to_nodos(nodo);
	} else {
		reactivar_nodo(nodoAux);
	}
}

/*
 * Dado un archivo y un bloque determinados, verifica si el bloque es invalido
 * y lo setea. En el caso de que lo sea, invalida tambien al archivo.
 */
void chequear_baja_archivo(t_archivo_self* archivo, int archivo_bloque) {
	t_bloque_archivo_self* bloque = dictionary_get(archivo->bloques,
			string_itoa(archivo_bloque));

	if (bloque->estado == VALIDO) {
		int i;
		t_list* copias = bloque->copias;
		int desactivados = 0;

		for (i = 0; i < list_size(copias); i++) {
			t_copia_self* copia = list_get(copias, i);
			if (copia->nodo_bloque->nodo->estado == INVALIDO) {
				desactivados++;
			}
		}

		bloque->estado = desactivados != list_size(copias)
				&& list_size(copias) != 0;

		if (bloque->estado == INVALIDO) {
			archivo->bloques_invalidos += 1;
			archivo->estado = INVALIDO;
		}
	}
}

/*
 * Desactiva un nodo.
 */
void baja_nodo(char* nombre) {
	t_nodo_self* nodo = find_nodo(nombre);
	if (nodo != NULL) {
		int i;
		nodo->estado = 0;
		t_list* archivos_nodo = dictionary_get(bloques_nodos_archivos, nombre);

		if (archivos_nodo != NULL) {
			for (i = 0; i < list_size(archivos_nodo); i++) {
				t_bloque_archivo_control_self* bloque_archivo_control =
						list_get(archivos_nodo, i);
				t_archivo_self * archivo = find_archivo(
						bloque_archivo_control->nombre_archivo, archivos);
				chequear_baja_archivo(archivo,
						bloque_archivo_control->archivo_bloque);
			}
		}
	}
}

/*
 * Crea un archivo nuevo.
 */
t_archivo_self* create_archivo(int padre, int tamanio, char* nombre) {
	t_archivo_self* archivo;
	archivo = malloc(sizeof(t_archivo_self));
	archivo->estado = 0;
	archivo->bloques = dictionary_create();
	archivo->padre = padre;
	archivo->tamanio = tamanio;
	archivo->nombre = nombre;
	archivo->bloques_invalidos = 0;

	return archivo;
}

/*
 * Crea un bloque nuevo.
 */
t_bloque_archivo_self* create_bloque(int numero) {
	t_bloque_archivo_self* bloque;
	bloque = malloc(sizeof(t_bloque_archivo_self));
	bloque->copias = list_create();
	bloque->estado = 1;
	bloque->numero = numero;

	return bloque;
}

/*
 * Crea una copia de un bloque determinado de un archivo y lo asigna a un bloque de un nodo.
 */
t_copia_self* create_copia(int bloque, t_nodo_self* nodo, char* nombre_archivo) {
	t_copia_self* copia;
	copia = malloc(sizeof(t_copia_self));
	copia->archivo_bloque = bloque;
	asignar_bloque_nodo_a_copia(copia, nodo);

	// Agrega el nodo_bloque usado a la lista de control.
	t_bloque_archivo_control_self* control;
	control = malloc(sizeof(t_bloque_archivo_control_self));
	control->archivo_bloque = 1;
	control->nombre_archivo = nombre_archivo;
	t_list* lista = dictionary_get(bloques_nodos_archivos, nodo->nombre);
	list_add(lista, control);

	return copia;
}

/*
 * Asigna un bloque de t_nodo_tp a una t_copia.
 */
void asignar_bloque_nodo_a_copia(t_copia_self* copia, t_nodo_self* nodo) {
	t_nodo_bloque_self *nodo_bloque;
	nodo_bloque->nodo = nodo;
	nodo_bloque->bloque_nodo = get_bloque_disponible(nodo);
	copia->nodo_bloque = nodo_bloque;
	nodo->bloques_disponibles -= 1;
}

/*
 * Asigna una t_copia a las copias de un t_bloque_archivo.
 */
void asignar_copia_a_bloque_archivo(t_bloque_archivo_self* bloque,
		t_copia_self* copia) {
	list_add(bloque->copias, copia);
}

void asignar_bloque_archivo_a_archivo(t_archivo_self* archivo,
		t_bloque_archivo_self* bloque) {
	dictionary_put(archivo->bloques, string_itoa(bloque->numero), bloque);
}

void add_archivo_to_archivos(t_archivo_self* archivo) {
	list_add(archivos, archivo);
}

t_archivo_datos_self* dividir_archivo(char* nombre) {
	t_archivo_datos_self* archivo;
	archivo = malloc(sizeof(archivo));

	FILE *file;
	long filelen;
	int i;
	archivo->bloques = list_create();
	int max = 20000000;
	int maxRead = max - sizeof(char);

	file = fopen(nombre, "rb");
	if (file == NULL){
		printf("No se encontro el archivo.");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	filelen = ftell(file);
	archivo->tamanio = filelen;
	rewind(file);

	int cantidadBloques = filelen / maxRead;
	int resto = filelen % maxRead;

	for (i = 0; i < cantidadBloques; i++) {
		char* segmento;
		segmento = (char *) malloc((max) * sizeof(char));
		fread(*&segmento, maxRead, 1, file);
		segmento = strcat(segmento, "\0");
		list_add(archivo->bloques, segmento);
	}

	if (resto != 0) {
		char* segmento;
		segmento = malloc(max);
		fread(*&segmento, max, 1, file);

		for (i = 0; i < resto; i++) {
			segmento = strcat(segmento, "\0");
		}
		list_add(archivo->bloques, segmento);
	}

	fclose(file);

	return archivo;
}

void agregar_archivo(char* nombre) {
	t_archivo_self* archivo;

	t_archivo_datos_self* archivo_datos = dividir_archivo(nombre);

	archivo = create_archivo(5, archivo_datos->tamanio, nombre);
	int i;
	for (i = 0; i < list_size(archivo_datos->bloques); i++) {
		t_bloque_archivo_self* bloque = create_bloque(i);

		t_copia_self* copia1 = create_copia(i, find_nodo_disponible(nombre, i),
				nombre);
		t_copia_self* copia2 = create_copia(i, find_nodo_disponible(nombre, i),
				nombre);
		t_copia_self* copia3 = create_copia(i, find_nodo_disponible(nombre, i),
				nombre);

		asignar_copia_a_bloque_archivo(bloque, copia1);
		asignar_copia_a_bloque_archivo(bloque, copia2);
		asignar_copia_a_bloque_archivo(bloque, copia3);

		asignar_bloque_archivo_a_archivo(archivo, bloque);
	}

	add_archivo_to_archivos(archivo);
}

//int main() {
//	archivos = list_create();
//	nodos = list_create();
//	bloques_nodos_archivos = dictionary_create();
//	bloques_nodo_disponible = dictionary_create();
//
//	alta_nodo("A");
//	alta_nodo("B");
//	alta_nodo("C");
//	alta_nodo("D");
//	alta_nodo("E");
//
//	baja_nodo("B");
//
//	agregar_archivo(ARCHIVO_NUEVO2);
//
//	baja_nodo("A");
//	baja_nodo("C");
//	baja_nodo("D");
//	baja_nodo("E");
//
//	alta_nodo("A");
//
//	alta_nodo("C");
//	alta_nodo("D");
//	alta_nodo("E");
//}
