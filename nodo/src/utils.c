/*
 * utils.c
 *
 *  Created on: Jul 5, 2015
 *      Author: Alejandro Zalazar
 */

#include "utils.h"

int string_is_null_or_empty(char* cadena) {
	return cadena == NULL || string_is_empty(cadena) == true;
}
