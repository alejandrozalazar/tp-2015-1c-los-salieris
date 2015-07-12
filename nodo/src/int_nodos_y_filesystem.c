/*
 * int_nodos_y_filesystem.c
 *
 *  Created on: 18/5/2015
 *      Author: Alejandro Zalazar
 */

#include "int_nodos_y_filesystem.h"

char* getBloque(int numeroBloque, t_estado* estado) {

	return leerEspacioDatos(estado->espacioDatos, numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES, estado->conf->BLOCK_SIZE_IN_BYTES);
}
void getBloqueYEscribir(int numeroBloque, t_estado* estado, int fileDescriptorToWrite) {
	int offset = numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES;
	int cantidadALeerRestante = estado->conf->BLOCK_SIZE_IN_BYTES;
	int bufferSize = 100;
	void* buffer = malloc(bufferSize);
	while(cantidadALeerRestante > 0) {

		int cantidadALeerPorVez = cantidadALeerRestante > bufferSize? bufferSize: cantidadALeerRestante;

		char* lectura = leerEspacioDatos(estado->espacioDatos, offset, cantidadALeerPorVez);
		cantidadALeerRestante = cantidadALeerRestante - cantidadALeerPorVez;
	}
}
void escribirBloque(int numeroBloque, t_estado* estado, char* contenido) {

	log_debug(estado->logger, "INICIO escribir en espacio de datos bloque nro: %d\n", numeroBloque);
	log_debug(estado->logger, "%s \n", contenido);

	escribirEnEspacioDatos(estado->espacioDatos, contenido, numeroBloque * estado->conf->BLOCK_SIZE_IN_BYTES);

	log_debug(estado->logger, "FIN escribir en espacio de datos bloque nro: %d \n", numeroBloque);
}
void getFileContent(char* nombreArchivoTemporal) {

}

char* setFileContent(char* contenidoArchivoTemporal, t_estado* estado) {
	char* nombreArchivoTemporal = generarNombreArchivoTemporal(estado->conf->DIR_TEMP);

	char* pathArchivoTemporal = string_new();
	string_append_with_format(&pathArchivoTemporal, "%s/%s", estado->conf->DIR_TEMP, nombreArchivoTemporal);

	log_debug(estado->logger, "Se genero path para archivo temporal %s \n", pathArchivoTemporal);

	FILE *fp;
	fp = fopen(pathArchivoTemporal, "w");
	fprintf(fp, contenidoArchivoTemporal);

	fclose(fp);

	return nombreArchivoTemporal;
}

char* generarNombreArchivoTemporal(char* tempDirectory) {
	char* base = "temp_script_";
	int count = 0;

	char* nombreArchivo = string_new();
	string_append_with_format(&nombreArchivo, "%s%d", base, count++);

	char* tempFilePath = string_new();
	string_append_with_format(&tempFilePath, "%s/%s", tempDirectory, nombreArchivo);
	bool generatedOk = false;

	while(generatedOk == false) {

		if( access( tempFilePath, F_OK ) == -1 ) {
			generatedOk = true;
		} else {
			nombreArchivo = string_new();
			string_append_with_format(&nombreArchivo, "%s%d", base, count++);

			tempFilePath = string_new();
			string_append_with_format(&tempFilePath, "%s/%s", tempDirectory, nombreArchivo);
		}
	}

	return nombreArchivo;
}

