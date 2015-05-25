/*
 * test_int_nodos_y_filesystem.c
 *
 *  Created on: 19/5/2015
 *      Author: Alejandro Zalazar
 */

#include <CUnit/CUnit.h>
#include <commons/string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/int_nodos_y_filesystem.h"
#include "cunit_tools.h"

static int init_suite() {
	return 0;
}

static int clean_suite() {
	return 0;
}



static CU_TestInfo tests[] = {
	//{ "Test basico creacion espacio datos", test_creacion_espacio_datos },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(int_nodos_y_filesystem, "Tests nodos y file system", init_suite, clean_suite, tests)
