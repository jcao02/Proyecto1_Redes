#include <stdio.h>

int errorMem() {

	printf("\nError en memoria.\n");
	return -1;
}

int errorHilo() {

    printf("\nError en hilo.\n");
    return -2;
}

int errorSocket() {

    printf("\nError en socket.\n");
    return -4;
}

int errorFile() {

	printf("\nError en escritura/lectura de archivos.\n");
	return -5;
}