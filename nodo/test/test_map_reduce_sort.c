/*
 * test_suite1.c
 *
 *  Created on: 12/4/2015
 *      Author: Alejandro Zalazar
 */

#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <mensajeria/mensajes.h>
#include <commons/string.h>
#include <unistd.h>
#include <CUnit/CUnit.h>
#include "../src/map_reduce_sort.h"
#include "cunit_tools.h"

t_log* testLogger;

static int init_suite() {
	testLogger = log_create("/tmp/testNodo.log", "Test Nodo", true, LOG_LEVEL_DEBUG);
	return 0;
}

static int clean_suite() {
	return 0;
}

static void test_map_reduce_sort() {

	char* sourceFileName = "/tmp/1.txt";
	char* destinationFileName = "/tmp/resultFile.txt";
	char* reduceScriptPath = "/home/utnso/Documentos/fakereduce.sh";
	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";

	//creamos el archivo
	FILE *fp;
	fp = fopen(sourceFileName, "w");
	fprintf(fp, "a\n");
	fprintf(fp, "d\n");
	fprintf(fp, "c\n");
	fprintf(fp, "e\n");
	fprintf(fp, "f\n");
	fprintf(fp, "b\n");
	fprintf(fp, "z\n");
	fprintf(fp, "h\n");
	fclose(fp);

	int var;
	int max = 123;
	for (var = 97; var < max; ++var) {
		printf("%d %c\\n\n", var, (var));
	}
	for (var = 97; var < max; ++var) {
		printf("\"reduce map %c\", \n", var, (var));
	}
	for (var = 97; var < max; ++var) {
		printf("\"map %c\", \n", var, (var));
	}
	for (var = 97; var < max; ++var) {
		printf("fprintf(fp, \"%c\\n\"); \n", var, (var));
	}//fprintf(fp, "a\n");


	catMapReduceSort(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, testLogger);

	fp = fopen(destinationFileName, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"reduce map a", "reduce map b", "reduce map c", "reduce map d", "reduce map e", "reduce map f", "reduce map h", "reduce map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {

		char* resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}

	fclose(fp);

	remove(sourceFileName);
	remove(destinationFileName);

}

static void test_map_reduce_sort_NUEVO() {

	char* sourceFileName = "/tmp/1.txt";
	char* destinationFileName = "/tmp/resultFile.txt";
	char* reduceScriptPath = "/home/utnso/Documentos/fakereduce.sh";
	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";

	//creamos el archivo
	FILE *fp;
	fp = fopen(sourceFileName, "w");
	fprintf(fp, "a\n");
	fprintf(fp, "d\n");
	fprintf(fp, "c\n");
	fprintf(fp, "e\n");
	fprintf(fp, "f\n");
	fprintf(fp, "b\n");
	fprintf(fp, "z\n");
	fprintf(fp, "h\n");
	fclose(fp);


	mapReduceSort(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, testLogger);

	fp = fopen(destinationFileName, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"reduce map a", "reduce map b", "reduce map c", "reduce map d", "reduce map e", "reduce map f", "reduce map h", "reduce map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}
	fclose(fp);

	remove(sourceFileName);
	remove(destinationFileName);

}

static void test_map_sort_reduce_REFACTOR() {

	char* sourceFileName = "/tmp/1.txt";
	char* destinationFileName = "/tmp/resultFile.txt";
	char* reduceScriptPath = "/home/utnso/Documentos/fakereduce.sh";
	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";

	//creamos el archivo
	FILE *fp;
	fp = fopen(sourceFileName, "w");
	fprintf(fp, "a\n");
	fprintf(fp, "d\n");
	fprintf(fp, "c\n");
	fprintf(fp, "e\n");
	fprintf(fp, "f\n");
	fprintf(fp, "b\n");
	fprintf(fp, "z\n");
	fprintf(fp, "h\n");
	fclose(fp);


	mapSortReduceRefactor(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, testLogger);

	fp = fopen(destinationFileName, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"reduce map a", "reduce map b", "reduce map c", "reduce map d", "reduce map e", "reduce map f", "reduce map h", "reduce map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}
	fclose(fp);

	remove(sourceFileName);
	remove(destinationFileName);

}

static void test_map_sort_REFACTOR() {

	char* sourceFileName = "/tmp/1.txt";
	char* destinationFileName = "/tmp/resultFile.txt";
	char* reduceScriptPath = "/home/utnso/Documentos/fakereduce.sh";
	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";

	//creamos el archivo
	FILE *fp;
	fp = fopen(sourceFileName, "w");
	fprintf(fp, "a\n");
	fprintf(fp, "d\n");
	fprintf(fp, "c\n");
	fprintf(fp, "e\n");
	fprintf(fp, "f\n");
	fprintf(fp, "b\n");
	fprintf(fp, "z\n");
	fprintf(fp, "h\n");
	fclose(fp);


	mapSortRefactor(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, testLogger);

	fp = fopen(destinationFileName, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"map a", "map b", "map c", "map d", "map e", "map f", "map h", "map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}
	fclose(fp);

	remove(sourceFileName);
	remove(destinationFileName);

}

t_archivo_nodo* crearNodo(FILE* nodoFd) {
	t_archivo_nodo* archivoNodo = malloc(sizeof(t_archivo_nodo));
	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->fd = nodoFd;
	archivoNodo->nodo = *nodo;
	return archivoNodo;
}

static void test_map_sort_REFACTOR_PARA_ARRAY_descriptores() {

	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";


	int pipeContent1[2];
	pipe(pipeContent1);

	//creamos el archivo
	FILE *fp1;
	fp1 = fdopen(pipeContent1[WRITE], "w");
	fprintf(fp1, "map a\n");
	fprintf(fp1, "map c\n");
	fprintf(fp1, "map e\n");
	fprintf(fp1, "map j\n");
	fprintf(fp1, "map p\n");
	fprintf(fp1, "map q\n");
	fprintf(fp1, "map s\n");
	fprintf(fp1, "map u\n");
	fprintf(fp1, "map v\n");
	fclose(fp1);

	int pipeContent2[2];
	pipe(pipeContent2);

	//creamos el archivo
	FILE *fp2;
	fp2 = fdopen(pipeContent2[WRITE], "w");
	fprintf(fp2, "map d\n");
	fprintf(fp2, "map f\n");
	fprintf(fp2, "map k\n");
	fprintf(fp2, "map l\n");
	fprintf(fp2, "map m\n");
	fprintf(fp2, "map o\n");
	fprintf(fp2, "map w\n");
	fprintf(fp2, "map y\n");
	fclose(fp2);

	//creamos el archivo
	int pipeContent3[2];
	pipe(pipeContent3);

	FILE *fp3;
	fp3 = fdopen(pipeContent3[WRITE], "w");
	fprintf(fp3, "map b\n");
	fprintf(fp3, "map g\n");
	fprintf(fp3, "map h\n");
	fprintf(fp3, "map i\n");
	fprintf(fp3, "map n\n");
	fprintf(fp3, "map r\n");
	fprintf(fp3, "map t\n");
	fprintf(fp3, "map x\n");
	fprintf(fp3, "map z\n");
	fclose(fp3);

	int pipeResult[2];
	pipe(pipeResult);

	//agregar a una lista
	t_list* lista = list_create();

	FILE* nodoFd = pipeContent1[READ];
	t_archivo_nodo* archivoNodo = crearNodo(nodoFd);
	list_add(lista, archivoNodo);

	nodoFd = pipeContent2[READ];
	archivoNodo = crearNodo(nodoFd);
	list_add(lista, archivoNodo);

	nodoFd = pipeContent3[READ];
	archivoNodo = crearNodo(nodoFd);
	list_add(lista, archivoNodo);

	reduceListDescriptor(lista, pipeResult[WRITE], mapScriptPath);

	FILE *fp = fdopen(pipeResult[READ], "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	//char *charsOrdenados[] = {"map a", "map b", "map c", "map d", "map e", "map f", "map h", "map z"};
	char *charsOrdenados[] = { "reduce map a", "reduce map b", "reduce map c", "reduce map d", "reduce map e", "reduce map f", "reduce map g", "reduce map h", "reduce map i", "reduce map j", "reduce map k", "reduce map l",
			"reduce map m", "reduce map n", "reduce map o", "reduce map p", "reduce map q", "reduce map r", "reduce map s", "reduce map t", "reduce map u", "reduce map v", "reduce map w", "reduce map x", "reduce map y", "reduce map z" };
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}

	close(pipeContent1[READ]);
	close(pipeContent1[WRITE]);
	close(pipeContent2[READ]);
	close(pipeContent2[WRITE]);
	close(pipeContent3[READ]);
	close(pipeContent3[WRITE]);
	close(pipeResult[READ]);
	close(pipeResult[WRITE]);

}

static void test_map_sort_REFACTOR_PARA_ARRAY() {

	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";


	int pipeContent[2];
	pipe(pipeContent);

	//creamos el archivo
	FILE *fp;
	fp = fdopen(pipeContent[WRITE], "w");
	fprintf(fp, "a\n");
	fprintf(fp, "d\n");
	fprintf(fp, "c\n");
	fprintf(fp, "e\n");
	fprintf(fp, "f\n");
	fprintf(fp, "b\n");
	fprintf(fp, "z\n");
	fprintf(fp, "h\n");
	fclose(fp);

	int pipeResult[2];
	pipe(pipeResult);

	mapSortDescriptor(pipeContent[READ], pipeResult[WRITE], mapScriptPath);

	fp = fdopen(pipeResult[READ], "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"map a", "map b", "map c", "map d", "map e", "map f", "map h", "map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}

	close(pipeContent[READ]);
	close(pipeContent[WRITE]);
	close(pipeResult[READ]);
	close(pipeResult[WRITE]);

}

static void test_map_sort_reduce_REFACTOR_COMBINADO() {

	char* sourceFileName = "/tmp/1.txt";
	char* intermediateFileName = "/tmp/intermediateFile.txt";
	char* destinationFileName = "/tmp/resultFile.txt";
	char* reduceScriptPath = "/home/utnso/Documentos/fakereduce.sh";
	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";

	//creamos el archivo
	FILE *fp;
	fp = fopen(sourceFileName, "w");
	fprintf(fp, "a\n");
	fprintf(fp, "d\n");
	fprintf(fp, "c\n");
	fprintf(fp, "e\n");
	fprintf(fp, "f\n");
	fprintf(fp, "b\n");
	fprintf(fp, "z\n");
	fprintf(fp, "h\n");
	fclose(fp);


	mapSortRefactor(mapScriptPath, reduceScriptPath, sourceFileName, intermediateFileName, testLogger);
	reduceRefactor(mapScriptPath, reduceScriptPath, intermediateFileName, destinationFileName, testLogger);

	fp = fopen(destinationFileName, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"reduce map a", "reduce map b", "reduce map c", "reduce map d", "reduce map e", "reduce map f", "reduce map h", "reduce map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(resultadoSubString, cadenaEsperada);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}
	fclose(fp);

	remove(sourceFileName);
	remove(intermediateFileName);
	remove(destinationFileName);

}




static CU_TestInfo tests[] = {
	{ "Test basico map reduce sort", test_map_reduce_sort },
	{ "Test basico map reduce sort NUEVO ", test_map_reduce_sort_NUEVO },
	{ "Test basico map sort reduce REFACTOR ", test_map_sort_reduce_REFACTOR },
	{ "Test basico map sort REFACTOR ", test_map_sort_REFACTOR },
	{ "Test basico map sort reduce REFACTOR COMBINADO", test_map_sort_reduce_REFACTOR_COMBINADO },
	{ "Test basico map sort reduce REFACTOR COMBINADO PARA ARRAY", test_map_sort_REFACTOR_PARA_ARRAY },
	{ "Test basico map sort reduce REFACTOR COMBINADO PARA ARRAY descriptores", test_map_sort_REFACTOR_PARA_ARRAY_descriptores },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(map_reduce_sort, "Tests ejecucion map, reduce, sort", init_suite, clean_suite, tests)
