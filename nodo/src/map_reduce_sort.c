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

	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName);
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
		char *argv1[] = { extraerNombreArchivo(reduceScriptPath), NULL };
		/*int result = */execv(reduceScriptPath, argv1);
		//printf("proc %d, if REDUCE file result = %d\n", result, pid4);
	} else {

		//printf("proc %d, if ELSE REDUCE file\n", pid4);
	}
	waitpid(pid, (int *) NULL, (int) NULL);
	waitpid(pid2, (int *) NULL, (int) NULL);
	waitpid(pid3, (int *) NULL, (int) NULL);
	close(filedes3[READ]);
	close(filedes3[WRITE]);
	close(filedes[READ]);
	close(filedes[WRITE]);
	close(filedes2[READ]);
	close(filedes2[WRITE]);
	close(destinationFileDescriptor);
	//printf("closing\n");
}

void mapReduceSort(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName) {

	int sourceFileDescriptor = abrirArchivoSoloLectura(sourceFileName);
	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName);

	int filedes2[2];
	int filedes3[2];

	pipe(filedes2);
	pipe(filedes3);

	pid_t pid2 = fork();
	if (pid2 == 0) {
		dup2(sourceFileDescriptor, 0);
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
		char *argv1[] = { extraerNombreArchivo(reduceScriptPath), NULL };
		/*int result = */execv(reduceScriptPath, argv1);
		//printf("proc %d, if REDUCE file result = %d\n", result, pid4);
	} else {

		//printf("proc %d, if ELSE REDUCE file\n", pid4);
	}
	waitpid(pid2, (int *) NULL, (int) NULL);
	waitpid(pid3, (int *) NULL, (int) NULL);

	close(filedes3[READ]);
	close(filedes3[WRITE]);
	close(filedes2[READ]);
	close(filedes2[WRITE]);
	close(sourceFileDescriptor);
	close(destinationFileDescriptor);
}


