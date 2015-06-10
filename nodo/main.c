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

int main() {


	bool runTests = true;

	if (runTests == true) {
		CU_initialize_registry();

		CU_register_suites(suites);

		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();
		CU_cleanup_registry();

		return CU_get_error();
	}

	conf_nodo* configuracion = cargarConfiguracion();
	estadoGlobal= inicializarEstado(configuracion);

	int resultado = ejecutarProgramaPrincipal(estadoGlobal);

	return resultado;
}

t_estado* inicializarEstado(conf_nodo* configuracion) {

	t_estado* miestado = malloc(sizeof(t_estado));
	t_log* logger = log_create(getRutaLog(configuracion), "Nodo", true, LOG_LEVEL_DEBUG);
	miestado->logger = logger;
	miestado->conf = configuracion;
	miestado->espacioDatos = NULL;
	return miestado;
}



