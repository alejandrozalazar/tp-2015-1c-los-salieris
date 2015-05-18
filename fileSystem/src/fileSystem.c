/*
 ============================================================================
 Name        : fileSystem.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/temporal.h>

int main(void) {
	char* tiempo = temporal_get_string_time();
	puts(tiempo); /* prints  */
	free(tiempo);

	return EXIT_SUCCESS;
}
