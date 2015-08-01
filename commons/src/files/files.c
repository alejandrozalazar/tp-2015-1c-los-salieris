/*
 * files.c
 *
 *  Created on: 16/5/2015
 *      Author: Alejandro Zalazar
 */
#include "files.h"

mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

int abrirArchivoLecturaEscritura(char* pathArchivo, t_log* logger) {
	int fd = open(pathArchivo, O_RDWR, mode);
	if (fd == -1) {
		log_error(logger, "open rw: %s", strerror(errno));
		exit(1);
	}
	return fd;
}


int abrirOCrearArchivoLecturaEscritura(char* pathArchivo, t_log* logger) {
	int fd = open(pathArchivo, O_RDWR | O_CREAT, mode);
	if (fd == -1) {
		log_error(logger, "open or create rw", strerror(errno));
		exit(1);
	}
	return fd;
}

int abrirArchivoSoloLectura(char* pathArchivo, t_log* logger) {
	int fd = open(pathArchivo, O_RDONLY, mode);
	if (fd == -1) {
		log_error(logger, "open read only", strerror(errno));
		exit(1);
	}
	return fd;
}

char *extraerNombreArchivo(char *text) {

	if (text != NULL) {
		char **elementoPath = string_split(text, "/");

		char *ultimoElementoEncontrado = NULL;

		while (*elementoPath != NULL) {
			if(ultimoElementoEncontrado != NULL) {
				free(ultimoElementoEncontrado);
			}

			ultimoElementoEncontrado = string_duplicate(*elementoPath);
			free(*elementoPath);
			elementoPath++;
		}

		return ultimoElementoEncontrado;
	}

	return NULL;
}


struct stat describirArchivo(char* archivo, t_log* logger) {
	struct stat sbuf;
	if (stat(archivo, &sbuf) == -1) {
		log_error(logger, "stat: archivo: %s, error: %s", archivo, strerror(errno));
		exit(1);
	}
	return sbuf;
}


long int obtenerTamanioArchivo(char* path, t_log* logger) {
	struct stat statArchivoEspacioDatos;
	statArchivoEspacioDatos = describirArchivo(path, logger);
    return statArchivoEspacioDatos.st_size;
}



ssize_t readLineN(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
			if (totRead < n) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}

bool existeArchivo(char* path) {
	if( access( path, F_OK ) == -1 ) {
		return false;
	}
	return true;
}

void borrarArchivo(char *pathArchivo) {
	unlink(pathArchivo);
}
