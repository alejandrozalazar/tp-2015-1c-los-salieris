/*
 * main.c
 *
 *  Created on: 12/4/2015
 *      Author: Alejandro Zalazar
 */

#include "CUnit/Basic.h"
#include "test/cunit_def.h"


int main() {
  CU_initialize_registry();

  CU_register_suites(suites);

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();

  return CU_get_error();
}



