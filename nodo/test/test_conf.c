/*
 * test_conf.c
 *
 *  Created on: 25/5/2015
 *      Author: Alejandro Zalazar
 */

#include <CUnit/CUnit.h>
#include <commons/string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/conf.h"
#include "cunit_tools.h"

static int init_suite() {
	return 0;
}

static int clean_suite() {
	return 0;
}

static void test_cargar_configuracion_por_defecto() {

	conf_nodo* conf = cargarConfiguracion(&conf);

	CU_ASSERT_EQUAL(conf->PUERTO_FS, 3000);
	CU_ASSERT_STRING_EQUAL(conf->IP_FS, "127.0.0.1");
	CU_ASSERT_STRING_EQUAL(conf->ARCHIVO_BIN, "data.bin");
	CU_ASSERT_STRING_EQUAL(conf->DIR_TEMP, "/tmp");
	CU_ASSERT_EQUAL(conf->NODO_NUEVO, true);
	CU_ASSERT_STRING_EQUAL(conf->IP_NODO, "127.0.0.1");
	CU_ASSERT_EQUAL(conf->PUERTO_NODO, 6000);
	CU_ASSERT_EQUAL(conf->BLOCK_SIZE_IN_BYTES, 20480);
}



static CU_TestInfo tests[] = {
	{ "Test basico carga configuracion por defecto", test_cargar_configuracion_por_defecto },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(conf, "Tests configuracion", init_suite, clean_suite, tests)
