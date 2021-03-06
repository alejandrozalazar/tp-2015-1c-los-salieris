/*
 * map_reduce_sort.h
 *
 *  Created on: 18/5/2015
 *      Author: Alejandro Zalazar
 */

#ifndef SRC_MAP_REDUCE_SORT_H_
#define SRC_MAP_REDUCE_SORT_H_

#include <commons/log.h>
#include <commons/string.h>
#include <mensajeria/mensajes.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <files/files.h>
#include "struct.h"


enum PIPES {
	READ, WRITE
};

void catMapReduceSort(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger);

void mapReduceSort(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger);

void reduceRefactor(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger);

void reduceDescriptor(int sourceFileDescriptor, int destinationFileDescriptor, char* reduceScriptPath);

void mapSortDescriptor(int sourceFileDescriptor, int destinationFileDescriptor, char* mapScriptPath);

bool atLeastALine(char* lines[], int linesCount);

char* readLine(int fp);

#endif /* SRC_MAP_REDUCE_SORT_H_ */


