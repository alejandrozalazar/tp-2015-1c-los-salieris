/*
 * struct.h
 *
 *  Created on: 7/6/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_STRUCT_H_
#define SRC_STRUCT_H_
#include <commons/log.h>
#include "conf.h"

	typedef struct {
		t_log* logger;
		conf_nodo* conf;
		char *espacioDatos;
	} t_estado;

#endif /* SRC_STRUCT_H_ */
