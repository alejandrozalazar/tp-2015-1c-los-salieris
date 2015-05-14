/*
 * job.h
 *
 *  Created on: 18/04/2015
 *      Author: utnso
 */

#ifndef JOB_H_
#define JOB_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/process.h>
#include <mensajeria/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mensajeria/sockets.h>

#define CONF_PATH "job.conf"
#define LOG_PATH "job.log"

t_config* CONFIG;
t_log* LOG;
int32_t socketMarta;

t_config* loadConfig(char *path, t_log* logger);
bool validarConfig();
void finish();

#endif /* JOB_H_ */
