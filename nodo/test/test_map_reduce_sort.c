/*
 * test_suite1.c
 *
 *  Created on: 12/4/2015
 *      Author: Alejandro Zalazar
 */

#include <string.h> /* memset */
#include <commons/string.h>
#include <unistd.h> /* close */
#include <CUnit/CUnit.h>
#include "../src/map_reduce_sort.h"
#include "cunit_tools.h"

static int init_suite() {
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


	catMapReduceSort(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName);

	fp = fopen(destinationFileName, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	char *charsOrdenados[] = {"reduce map a", "reduce map b", "reduce map c", "reduce map d", "reduce map e", "reduce map f", "reduce map h", "reduce map z"};
	int charsOrdenadosIndice = 0;

	while ((leido = getline(&linea, &len, fp)) != -1) {
		linea = string_substring_until(linea, string_length(linea)-1);

		char *cadenaEsperada = charsOrdenados[charsOrdenadosIndice++];
		CU_ASSERT_STRING_EQUAL(linea, cadenaEsperada);
	}

	fclose(fp);

	remove(sourceFileName);
	remove(destinationFileName);

}

static CU_TestInfo tests[] = {
	{ "Test basico map reduce sort", test_map_reduce_sort },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(map_reduce_sort, "Tests ejecucion map, reduce, sort", init_suite, clean_suite, tests)
