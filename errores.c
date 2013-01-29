#include <stdio.h>

int errorMem() {

	printf("\nError en memoria.\n");
	return -1;
}

int errorHilo() {

    printf("\nError en creacion de hilo.\n");
    return -2;
}

int errorFichero() {

	printf("\nError en fichero.\n");
	return -3;
}