#include <stdio.h>
#include <string.h>
#include "extra.h"
#define DEFAULT_CHAR "\033[0m"
#define UNDER_CHAR "\033[4m"
#define BOLD_CHAR "\033[1m"

void imprimirHB() {
	printf("AYUDA");
}

void imprimirEB() {
	printf("uso: "BOLD_CHAR"bomba –n nombreBomba –cp capacidadMaxima ");
	printf("–i inventario –c consumo –fc FicheroCentros\n"DEFAULT_CHAR);
	printf("o: "BOLD_CHAR"bomba -h"DEFAULT_CHAR" para mas informacion\n");
}

int llamadaB (int argc, char **argv, char **nombre, char **fich, int *cap, int *inv, int *con) {

	int i, flags;

	switch (argc) {

		case 1:			/*sin flags*/
			imprimirEB();
			return -1;
		case 2:			/*-h*/
			if (strcmp(argv[2], "-h") == 0) {
				imprimirHB();
			} else {
				imprimirEB();
			}
			return -1;
		default:		/*flags*/
			/*si no tiene el numero de flags adecuado*/
			if (argc != 11) {
				imprimirEB();
				return -1;
			}

			for (i = 1; i < argc; ++i) {

				/*Si ya reviso esta posicion*/
				if (argv[i] == NULL) {
					continue;
				}
				
				/*Si consigue -n*/
				if (strcmp(argv[i], "-n") == 0) {
					*nombre = argv[i+1];

				/*Si consigue -cp*/
				} else if (strcmp(argv[i], "-cp") == 0) {
					*cap = atoi(argv[i+1]);

				/*Si consigue -i*/
				} else if (strcmp(argv[i], "-i") == 0) {
					*inv = atoi(argv[i+1]);

				/*Si consigue -c*/
				} else if (strcmp(argv[i], "-c") == 0) {
					*con = atoi(argv[i+1]);

				/*Si consigue -fc*/
				} else if (strcmp(argv[i], "-fc") == 0) {
					*fich = argv[i+1];

				/*Si no es un flag valido*/
				} else {

					imprimirEB();
					return -1;
				}

				/*Indico que ya conoce los valores de estas*/
				argv[i] = argv[i+1] = NULL;
			}



	}

	return 0;
}

int llamadaC (int argc, char **argv) {


}