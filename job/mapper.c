/*
 * mapper.c
 *
 *  Created on: 13/05/2015
 *      Author: utnso
 */

#include "job.h"

void mapper(){
	printf("hola! soy un mapper Y mi tid es %d", process_get_thread_id());
	pthread_exit(NULL);
}
