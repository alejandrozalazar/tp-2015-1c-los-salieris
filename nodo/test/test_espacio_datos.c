/*
 * test_espacio_datos.c
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
#include "../src/espacio_datos.h"
#include "cunit_tools.h"

static int init_suite() {
	return 0;
}

static int clean_suite() {
	return 0;
}


static void crearArchivoMmapParaTest(char *pathArchivo, int tamanioArchivoEnBytes) {
	char *command = string_new();
	string_append_with_format(&command, "truncate -s %d %s", tamanioArchivoEnBytes, pathArchivo);
	int systemResult = system(command);
	if(systemResult < 0) {
		perror("truncate");
		exit(1);
	}
}

static void test_creacion_espacio_datos() {
	int tamanio_espacio_datos_test_en_bytes = 20 * 1024; // 20 Kb

	char* pathArchivoEspacioDatos = "/tmp/espacioDatosTest";

	crearArchivoMmapParaTest(pathArchivoEspacioDatos, tamanio_espacio_datos_test_en_bytes);

	int fdEspacioDatos = abrirArchivoEspacioDatos(pathArchivoEspacioDatos);
	struct stat statArchivoEspacioDatos = describirArchivoEspacioDatos(pathArchivoEspacioDatos);
    int tamanioArchivoEspacioDatos = statArchivoEspacioDatos.st_size;

    int offset = 0;

    char *espacioDatos = crearEspacioDeDatos(fdEspacioDatos, tamanioArchivoEspacioDatos);


    char *contenido = "1234567890";
	escribirEnEspacioDatos(espacioDatos, contenido, offset);

    int cantidadALeer = 5;
    char *leido = leerEspacioDatos(espacioDatos, 5, cantidadALeer);
    CU_ASSERT_STRING_EQUAL(leido, "67890"); // el archivo esta inicialmente vacio

    eliminarEspacioDeDatos(espacioDatos, tamanioArchivoEspacioDatos);
	cerrarArchivoEspacioDeDatos(fdEspacioDatos);
}

static CU_TestInfo tests[] = {
	{ "Test basico creacion espacio datos", test_creacion_espacio_datos },
	CU_TEST_INFO_NULL,
};

CUNIT_MAKE_SUITE(espacio_datos, "Tests manejo espacio de datos", init_suite, clean_suite, tests)
