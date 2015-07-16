/*
 * test_suite1.c
 *
 *  Created on: 12/4/2015
 *      Author: Alejandro Zalazar
 */

#include <stdlib.h>
#include <string.h>
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
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(map_reduce_sort, "Tests ejecucion map, reduce, sort", init_suite, clean_suite, tests)
