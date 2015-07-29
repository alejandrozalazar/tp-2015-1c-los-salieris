/*
 * main.c
 *
 *  Created on: 12/4/2015
 *      Author: Alejandro Zalazar
 */

#include <stdbool.h>
#include "CUnit/Basic.h"
#include "src/principal.h"
#include "test/cunit_def.h"

t_estado* estadoGlobal;

int main(int argc, char *argv[]) {


	bool runTests = false;

	if (runTests == true) {
		CU_initialize_registry();

		CU_register_suites(suites);

		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();
		CU_cleanup_registry();

		return CU_get_error();
	}

	puts("Inicializando");
	conf_nodo* configuracion = cargarConfiguracion(argc, argv);
	estadoGlobal= inicializarEstado(configuracion);

	int resultado = ejecutarProgramaPrincipal(estadoGlobal);

	return resultado;
}

void inicializarEspacioDatos(t_estado* estado) {
	t_log* logger = estado->logger;
	estado->espacioDatos = nuevoEspacioDeDatos(estado->conf->RUTA_ESPACIO_DATOS, estado->conf->TAMANIO_ESPACIO_DATOS_EN_BYTES, logger);
}


t_estado* inicializarEstado(conf_nodo* configuracion) {

	t_estado* miestado = malloc(sizeof(t_estado));
	t_log* logger = log_create(getRutaLog(configuracion), "Nodo", true, LOG_LEVEL_DEBUG);
	miestado->logger = logger;
	miestado->conf = configuracion;
	inicializarEspacioDatos(miestado);
	loguearDatosConfiguracion(configuracion, logger);
	return miestado;
}



