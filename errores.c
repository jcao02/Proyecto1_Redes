#include <stdio.h>

int errorMem() {

	printf("\nError en memoria.\n");
	return -1;
}

int errorHilo() {

    printf("\nError en hilo.\n");
    return -2;
}

int errorFichero() {

	printf("\nError en fichero.\n");
	return -3;
}

int errorSocket() {

    printf("\nError en socket.\n");
    return -4;
}
