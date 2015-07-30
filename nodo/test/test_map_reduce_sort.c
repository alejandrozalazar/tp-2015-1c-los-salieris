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

static void test_map_sort_reduce_file_path() {

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


	mapSortReduceFilePath(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, testLogger);

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

static void test_map_sort_file_path() {

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


	mapSortFilePath(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, testLogger);

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

static void test_reduce_descriptor_list() {

	char* mapScriptPath = "/home/utnso/Documentos/fakereduce.sh";


	int pipeContent1[2];

	char* stringModelo = "map a\n";
	int tamanioString = strlen(stringModelo);

	int count1 = 0;
	//creamos el archivo
	char* testSource1 = "/tmp/test-source1";
	if(existeArchivo(testSource1)) {
		borrarArchivo(testSource1);
	}
	int fp1 = abrirOCrearArchivoLecturaEscritura(testSource1, testLogger);
	dprintf(fp1, "map a\n");
	count1++;
	dprintf(fp1, "map c\n");
	count1++;
	dprintf(fp1, "map e\n");
	count1++;
	dprintf(fp1, "map j\n");
	count1++;
	dprintf(fp1, "map p\n");
	count1++;
	dprintf(fp1, "map q\n");
	count1++;
	dprintf(fp1, "map s\n");
	count1++;
	dprintf(fp1, "map u\n");
	count1++;
	dprintf(fp1, "map v\n");
	count1++;
	close(fp1);
	pipeContent1[READ] = abrirOCrearArchivoLecturaEscritura(testSource1, testLogger);

	int pipeContent2[2];

	int count2 = 0;
	//creamos el archivo
	char* testSource2 = "/tmp/test-source2";
	if(existeArchivo(testSource2)) {
		borrarArchivo(testSource2);
	}
	int fp2 = abrirOCrearArchivoLecturaEscritura(testSource2, testLogger);
	dprintf(fp2, "map d\n");
	count2++;
	dprintf(fp2, "map f\n");
	count2++;
	dprintf(fp2, "map k\n");
	count2++;
	dprintf(fp2, "map l\n");
	count2++;
	dprintf(fp2, "map m\n");
	count2++;
	dprintf(fp2, "map o\n");
	count2++;
	dprintf(fp2, "map w\n");
	count2++;
	dprintf(fp2, "map y\n");
	count2++;
	close(fp2);
	pipeContent2[READ] = abrirOCrearArchivoLecturaEscritura(testSource2, testLogger);

	int count3 = 0;
	int pipeContent3[2];

	//creamos el archivo
	char* testSource3 = "/tmp/test-source3";
	if(existeArchivo(testSource3)) {
		borrarArchivo(testSource3);
	}
	int fp3 = abrirOCrearArchivoLecturaEscritura(testSource3, testLogger);
	dprintf(fp3, "map b\n");
	count3++;
	dprintf(fp3, "map g\n");
	count3++;
	dprintf(fp3, "map h\n");
	count3++;
	dprintf(fp3, "map i\n");
	count3++;
	dprintf(fp3, "map n\n");
	count3++;
	dprintf(fp3, "map r\n");
	count3++;
	dprintf(fp3, "map t\n");
	count3++;
	dprintf(fp3, "map x\n");
	count3++;
	dprintf(fp3, "map z\n");
	count3++;
	close(fp3);
	pipeContent3[READ] = abrirOCrearArchivoLecturaEscritura(testSource3, testLogger);

	int pipeResult[2];
	pipe(pipeResult);

	//agregar a una lista
	t_list* lista = list_create();

	int nodoFd = pipeContent1[READ];
	t_archivo_nodo archivoNodo1;
	archivoNodo1.nodo.fd = nodoFd;
	archivoNodo1.tamanioArchivo = count1 * tamanioString;
	list_add(lista, &archivoNodo1);

	nodoFd = pipeContent2[READ];
	t_archivo_nodo archivoNodo2;
	archivoNodo2.nodo.fd = nodoFd;
	archivoNodo2.tamanioArchivo = count2 * tamanioString;
	list_add(lista, &archivoNodo2);

	nodoFd = pipeContent3[READ];
	t_archivo_nodo archivoNodo3;
	archivoNodo3.nodo.fd = nodoFd;
	archivoNodo3.tamanioArchivo = count3 * tamanioString;
	list_add(lista, &archivoNodo3);

	reduceListDescriptor(lista, pipeResult[WRITE], mapScriptPath);

	close(pipeResult[WRITE]);

	FILE *fp = fdopen(pipeResult[READ], "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

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
	close(pipeContent2[READ]);
	close(pipeContent3[READ]);
	close(pipeResult[READ]);
	close(pipeResult[WRITE]);

}

static void test_map_sort_para_array() {

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

static void test_map_sort_reduce_combinado() {

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


	mapSortFilePath(mapScriptPath, reduceScriptPath, sourceFileName, intermediateFileName, testLogger);
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
	{ "Test basico map sort reduce por file path ", test_map_sort_reduce_file_path },
	{ "Test basico map sort por file path ", test_map_sort_file_path },
	{ "Test basico map sort reduce combinado", test_map_sort_reduce_combinado },
	{ "Test basico map sort reduce para array", test_map_sort_para_array },
	{ "Test basico reduce varios descriptores", test_reduce_descriptor_list },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(map_reduce_sort, "Tests ejecucion map, reduce, sort", init_suite, clean_suite, tests)
