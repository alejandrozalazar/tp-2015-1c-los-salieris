/*
 * test_suite1.c
 *
 *  Created on: 12/4/2015
 *      Author: Alejandro Zalazar
 */

#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <CUnit/CUnit.h>
#include "cunit_tools.h"

static int init_suite() {
	return 0;
}

static int clean_suite() {
	return 0;
}

static void test_extrae_nombre_archivo_de_path() {

	char* reduceScriptPath = "/home/utnso/Documentos/fakereduce.sh";

	CU_ASSERT_STRING_EQUAL(extraerNombreArchivo(reduceScriptPath), "fakereduce.sh")

	char* mapScriptPath = "/home/utnso/Documentos/fakemap.sh";

	CU_ASSERT_STRING_EQUAL(extraerNombreArchivo(mapScriptPath), "fakemap.sh")
}

static CU_TestInfo tests[] = {
	{ "Test extrae nombre archivo", test_extrae_nombre_archivo_de_path },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(files, "Tests funciones relacionadas con archivos y paths", init_suite, clean_suite, tests)
