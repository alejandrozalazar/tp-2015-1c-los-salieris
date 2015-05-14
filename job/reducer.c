/*
 * reducer.c
 *
 *  Created on: 13/05/2015
 *      Author: utnso
 */

#include "job.h"

void reducer(){
	printf("hola! soy un reducer! Y mi tid es %d", process_get_thread_id());
	pthread_exit(NULL);
}
