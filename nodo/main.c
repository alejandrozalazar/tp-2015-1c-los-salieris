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

	int resultado = ejecutarProgramaPrincipal();

	return resultado;
}



