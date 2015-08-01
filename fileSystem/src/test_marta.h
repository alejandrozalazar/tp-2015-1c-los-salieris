/*
 * test_marta.h
 *
 *  Created on: Aug 1, 2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_TEST_MARTA_H_
#define SRC_TEST_MARTA_H_

#include "ABM.h"

t_nodo* convertir_t_nodo_self_a_t_nodo(t_nodo_self* nodoSelf);

t_bloque_archivo create_t_bloque_archivo(t_bloque_archivo_self* self);
t_bloque_nodo* create_t_bloque_nodo(t_copia_self* copia_self);

#endif /* SRC_TEST_MARTA_H_ */
