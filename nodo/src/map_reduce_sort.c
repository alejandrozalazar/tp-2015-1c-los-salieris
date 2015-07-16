/*
 * system_calls_use.c
 *
 *  Created on: 19/4/2015
 *      Author: Alejandro Zalazar
 */

#include "map_reduce_sort.h"

void ejecutarOperacion(int nroBloque, t_estado* estado, char* nombreArchivo, char* sufijo, void (*operacion)(int, int, char*)) {
	char* pathArchivoTemporal = generarPathArchivoTemporal(estado, nombreArchivo);
	char* destinationFileName = string_new();
	string_append_with_format(&destinationFileName, "%s%s", pathArchivoTemporal, sufijo);
	int pipeContent[2];
	pipe(pipeContent);
	//creamos el archivo
	//	FILE *fpResultado;
	//	fpResultado = fopen(destinationFileName, "w");
	int fpResultado = abrirOCrearArchivoLecturaEscritura(destinationFileName, estado->logger);
	char* contenido = getBloque(nroBloque, estado);
	//creamos el archivo
	FILE* fp;
	fp = fdopen(pipeContent[WRITE], "w");
	fprintf(fp, contenido);
	fclose(fp);
	//	write(pipeContent[WRITE], contenido, strlen(contenido));
	//	write(pipeContent[WRITE], '\0', 1);
	operacion(pipeContent[READ], fpResultado, pathArchivoTemporal);
	close(pipeContent[READ]);
	close(fpResultado);
	printFile(destinationFileName, nroBloque);
}

void ejecutarMapeo(int nroBloque, char* nombreArchivo, t_estado* estado) {
	ejecutarOperacion(nroBloque, estado, nombreArchivo, "_map_result", mapSortDescriptor);
}

void ejecutarReduce(int nroBloque, char* nombreArchivo, t_estado* estado) {
	ejecutarOperacion(nroBloque, estado, nombreArchivo, "_reduce_result", reduceDescriptor);
}

void printFile(char* file, int nroBloque) {
	FILE *fp;
	fp = fopen(file, "r");

	char * linea = NULL;
	size_t len = 0;
	ssize_t leido;

	printf("Imprimiendo contenido bloque nro: %d\n", nroBloque);
	while ((leido = getline(&linea, &len, fp)) != -1) {
		char *resultadoSubString = string_substring_until(linea, string_length(linea)-1);

		if(linea != NULL) {
			free(linea);
			linea = NULL;
		}
		printf("%s\n", resultadoSubString);
	}

	if(linea != NULL) {
		free(linea);
		linea = NULL;
	}
	fclose(fp);
}

void catMapReduceSortFromFd(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, int fdSource,
		t_log* logger) {
	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName, logger);
	int filedes[2];
	int filedes2[2];
	int filedes3[2];
	pipe(filedes);
	pipe(filedes2);
	pipe(filedes3);
	pid_t pid = fork();
	if (pid == 0) {
		dup2(filedes[WRITE], 1);

		char * linea = NULL;
		size_t len = 0;
		ssize_t leido;

		while ((leido = getline(&linea, &len, (FILE *) fdSource)) != -1) {

			if (linea != NULL) {

				fputs(linea, (FILE *) filedes[WRITE]);

				free(linea);
				linea = NULL;
			}

		}

		if (linea != NULL) {

			fputs(linea, (FILE *) filedes[WRITE]);

			free(linea);
			linea = NULL;
		}

		if (0) {
			char *argv[] = { "cat", sourceFileName, NULL };
			/*int result = */execv("/bin/cat", argv);
			//printf("proc %d, if CAT file result = %d\n", result, pid);
		}
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
}

void catMapReduceSort(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger) {

	int fdSource = abrirArchivoSoloLectura(sourceFileName, logger);
	catMapReduceSortFromFd(mapScriptPath, reduceScriptPath, sourceFileName, destinationFileName, fdSource, logger);
	//printf("closing\n");
}

void mapReduceSort(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger) {

	int sourceFileDescriptor = abrirArchivoSoloLectura(sourceFileName, logger);
	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName, logger);

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

void mapFile(int inputDescriptor, int outputDescriptor, char* mapScriptPath) {
	pid_t pid2 = fork();
	if (pid2 == 0) {
		dup2(inputDescriptor, 0);
		dup2(outputDescriptor, 1);
		char *argv[] = { extraerNombreArchivo(mapScriptPath), "-c", NULL };
		/*int result = */execv(mapScriptPath, argv);
		//printf("proc %d, if MAP file result = %d\n", result, pid2);
	} else {
		close(outputDescriptor);
		//printf("proc %d, if ELSE MAP file\n", pid2);
	}
	waitpid(pid2, (int *) NULL, (int) NULL);
}

void sortFile(int inputDescriptor2, int outputDescriptor2) {
	pid_t pid3 = fork();
	if (pid3 == 0) {

		dup2(inputDescriptor2, 0);
		dup2(outputDescriptor2, 1);
		char *argv1[] = { "sort", NULL };
		/*int result = */execv("/usr/bin/sort", argv1);
		//printf("proc %d, if SORT file result = %d\n", result, pid3);
	} else {
		close(outputDescriptor2);
		//printf("proc %d, if ELSE SORT file\n", pid3);
	}
	waitpid(pid3, (int *) NULL, (int) NULL);
}

void reduceFile(int inputDescriptor3, int outputDescriptor3, char* reduceScriptPath) {
	pid_t pid4 = fork();
	if (pid4 == 0) {

		dup2(inputDescriptor3, 0);
		dup2(outputDescriptor3, 1);
		char *argv1[] = { extraerNombreArchivo(reduceScriptPath), NULL };
		/*int result = */execv(reduceScriptPath, argv1);
		//printf("proc %d, if REDUCE file result = %d\n", result, pid4);
	} else {

		//printf("proc %d, if ELSE REDUCE file\n", pid4);
	}
	waitpid(pid4, (int *) NULL, (int) NULL);
}

void mapSortReduceRefactor(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger) {

	int sourceFileDescriptor = abrirArchivoSoloLectura(sourceFileName, logger);
	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName, logger);

	int filedes2[2];
	int filedes3[2];

	pipe(filedes2);
	pipe(filedes3);

	int inputDescriptor = sourceFileDescriptor;
	int outputDescriptor = filedes2[WRITE];
	mapFile(inputDescriptor, outputDescriptor, mapScriptPath);

	int inputDescriptor2 = filedes2[READ];
	int outputDescriptor2 = filedes3[WRITE];
	sortFile(inputDescriptor2, outputDescriptor2);

	int inputDescriptor3 = filedes3[READ];
	int outputDescriptor3 = destinationFileDescriptor;
	reduceFile(inputDescriptor3, outputDescriptor3, reduceScriptPath);

	close(filedes3[READ]);
	close(filedes3[WRITE]);
	close(filedes2[READ]);
	close(filedes2[WRITE]);
	close(sourceFileDescriptor);
	close(destinationFileDescriptor);
}

void mapSortDescriptor(int sourceFileDescriptor, int destinationFileDescriptor, char* mapScriptPath) {
	int filedes2[2];
	pipe(filedes2);

	int inputDescriptor = sourceFileDescriptor;
	int outputDescriptor = filedes2[WRITE];

	mapFile(inputDescriptor, outputDescriptor, mapScriptPath);

	int inputDescriptor2 = filedes2[READ];
	int outputDescriptor2 = destinationFileDescriptor;

	sortFile(inputDescriptor2, outputDescriptor2);

	close(filedes2[READ]);
	close(filedes2[WRITE]);
}

void mapSortRefactor(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger) {

	int sourceFileDescriptor = abrirArchivoSoloLectura(sourceFileName, logger);
	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName, logger);

	mapSortDescriptor(sourceFileDescriptor, destinationFileDescriptor, mapScriptPath);

	close(sourceFileDescriptor);
	close(destinationFileDescriptor);
}

void reduceDescriptor(int sourceFileDescriptor, int destinationFileDescriptor, char* reduceScriptPath) {
	int inputDescriptor3 = sourceFileDescriptor;
	int outputDescriptor3 = destinationFileDescriptor;
	reduceFile(inputDescriptor3, outputDescriptor3, reduceScriptPath);
}

void reduceRefactor(char* mapScriptPath, char* reduceScriptPath, char* sourceFileName, char* destinationFileName, t_log* logger) {

	int sourceFileDescriptor = abrirArchivoSoloLectura(sourceFileName, logger);
	int destinationFileDescriptor = abrirOCrearArchivoLecturaEscritura(destinationFileName, logger);

	reduceDescriptor(sourceFileDescriptor, destinationFileDescriptor, reduceScriptPath);

	close(sourceFileDescriptor);
	close(destinationFileDescriptor);
}

