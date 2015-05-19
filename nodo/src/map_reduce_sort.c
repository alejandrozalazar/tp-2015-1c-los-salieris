/*
 * system_calls_use.c
 *
 *  Created on: 19/4/2015
 *      Author: Alejandro Zalazar
 */

#include <stdlib.h>
#include <commons/string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "files.h"

enum PIPES {
	READ, WRITE
};
void catMapReduceSort(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName) {

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
	int destinationFileDescriptor = open(destinationFileName, O_RDWR | O_CREAT, mode);
	int filedes[2];
	int filedes2[2];
	int filedes3[2];
	pipe(filedes);
	pipe(filedes2);
	pipe(filedes3);
	pid_t pid = fork();
	if (pid == 0) {
		dup2(filedes[WRITE], 1);

		char *argv[] = { "cat", sourceFileName, NULL };
		/*int result = */execv("/bin/cat", argv);
		//printf("proc %d, if CAT file result = %d\n", result, pid);
	} else {
		close(filedes[WRITE]);
		//printf("proc %d, if ELSE CAT file\n", pid);
	}
	pid_t pid2 = fork();
	if (pid2 == 0) {
		dup2(filedes[READ], 0);
		dup2(filedes2[WRITE], 1);
		char *argv[] = { extraerNombreArchivo(mapScriptPath), "-c", NULL };
		/*int result = */execv(mapScriptPath, argv);
		//printf("proc %d, if MAP file result = %d\n", result, pid2);
	} else {
		close(filedes2[WRITE]);
		//printf("proc %d, if ELSE MAP file\n", pid2);
	}
	pid_t pid3 = fork();
	if (pid3 == 0) {

		dup2(filedes2[READ], 0);
		dup2(filedes3[WRITE], 1);
		char *argv1[] = { "sort", NULL };
		/*int result = */execv("/usr/bin/sort", argv1);
		//printf("proc %d, if SORT file result = %d\n", result, pid3);
	} else {
		close(filedes3[WRITE]);
		//printf("proc %d, if ELSE SORT file\n", pid3);
	}
	pid_t pid4 = fork();
	if (pid4 == 0) {

		dup2(filedes3[READ], 0);
		dup2(destinationFileDescriptor, 1);
		char *argv1[] = { "fakereduce.sh", NULL };
		/*int result = */execv(reduceScriptPath, argv1);
		//printf("proc %d, if REDUCE file result = %d\n", result, pid4);
	} else {

		//printf("proc %d, if ELSE REDUCE file\n", pid4);
	}
	waitpid(pid, (int *) NULL, (int) NULL);
	waitpid(pid2, (int *) NULL, (int) NULL);
	waitpid(pid3, (int *) NULL, (int) NULL);
	close(filedes3[0]);
	close(filedes3[1]);
	close(filedes[0]);
	close(filedes[1]);
	close(filedes2[0]);
	close(filedes2[1]);
	close(destinationFileDescriptor);
	//printf("closing\n");
}


