#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extra.h"
#define DEFAULT_CHAR "\033[0m"
#define UNDER_CHAR "\033[4m"
#define BOLD_CHAR "\033[1m"

/*
 *
 */
distr create_distr(char *nombre, char *DNS, int puerto, int pr) {
	distr d;
	char* temp;
	int size;

	/*Pide memoria para la estrucura de distribuidor*/
	if ((d = (distr) malloc(sizeof(struct distr))) == NULL) {
		errorMem();
		return NULL;
	}

	/*Copiamos los atributos para evitar errores de apuntadores*/
	size = strlen(nombre);
	if ((temp = (char *) malloc((size + 1) * sizeof(char))) == NULL) {
		errorMem();
	}
	strcpy(temp, nombre);
	d->nombre = temp;

	size = strlen(DNS);
	if ((temp = (char *) malloc((size + 1) * sizeof(char))) == NULL) {
		errorMem();
	}
	strcpy(temp, DNS);
	d->DNS = temp;

	d->puerto = puerto;
	d->pr = pr;
}

/*
 *
 */
void imprimirHB() {
	printf(UNDER_CHAR BOLD_CHAR"\nAYUDA\n\n"DEFAULT_CHAR);
}

/*
 *
 */
void imprimirEB() {
	printf("uso: "BOLD_CHAR"bomba ["DEFAULT_CHAR" -h "BOLD_CHAR"|"DEFAULT_CHAR);
	printf(" –n nombreBomba –cp capacidadMaxima –i inventario –c consumo");
	printf(" –fc FicheroCentros "BOLD_CHAR"]\n"DEFAULT_CHAR);
}

/*
 * 
 */
void imprimirHC() {
	printf(UNDER_CHAR BOLD_CHAR"\nAYUDA\n\n"DEFAULT_CHAR);
}

/*
 *
 */
void imprimirEC() {
	printf("uso: "BOLD_CHAR"centro ["DEFAULT_CHAR" -h "BOLD_CHAR"|"DEFAULT_CHAR);
	printf(" –n nombreCentro –cp capacidadMaxima –i inventario –t tiempo");
	printf(" –s suministro -p puerto "BOLD_CHAR"]\n"DEFAULT_CHAR);
}

/*
 * 'nombre': nombre de la bomba
 * 'fich': fichero de centros de distribucion
 * 'max': capacidad maxima de la bomba
 * 'inv': inventario inicial de la bomba
 * 'con': consumo de gasolina por minuto en la bomba
 */
int llamadaB (int argc, char **argv, char **nombre, char **fich, int *max, int *inv, int *con) {

	int i, flags;

	switch (argc) {

		case 1:			/*sin flags*/
			imprimirEB();
			return -1;
		case 2:			/*-h ayuda*/
			if (strcmp(argv[1], "-h") == 0) {
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
				
				/*Si consigue -n nombre*/
				if (strcmp(argv[i], "-n") == 0) {
					*nombre = argv[i+1];

				/*Si consigue -cp capacidad maxima*/
				} else if (strcmp(argv[i], "-cp") == 0) {
					*max = atoi(argv[i+1]);

				/*Si consigue -i inventario*/
				} else if (strcmp(argv[i], "-i") == 0) {
					*inv = atoi(argv[i+1]);

				/*Si consigue -c consumo de gasolina*/
				} else if (strcmp(argv[i], "-c") == 0) {
					*con = atoi(argv[i+1]);

				/*Si consigue -fc fichero de centros*/
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

/*
 * 
 */
int llamadaC (int argc, char **argv, char **nombre, int *max, int *inv, int *tiempo, int *sum, int *puerto ) {
    int i, flags;

    switch (argc) {

        case 1:			/*sin flags*/
            imprimirEC();
            return -1;
        case 2:			/*-h ayuda*/
            if (strcmp(argv[1], "-h") == 0) {
                imprimirHC();
            } else {
                imprimirEC();
            }
            return -1;
        default:		/*flags*/
            /*si no tiene el numero de flags adecuado*/
            if (argc != 13) {
                imprimirEC();
                return -1;
            }

            for (i = 1; i < argc; ++i) {

                /*Si ya reviso esta posicion*/
                if (argv[i] == NULL) {
                    continue;
                }

                /*Si consigue -n nombre*/
                if (strcmp(argv[i], "-n") == 0) {
                    *nombre = argv[i+1];

                    /*Si consigue -cp capacidad maxima*/
                } else if (strcmp(argv[i], "-cp") == 0) {
                    *max = atoi(argv[i+1]);

                    /*Si consigue -i inventario*/
                } else if (strcmp(argv[i], "-i") == 0) {
                    *inv = atoi(argv[i+1]);

                    /*Si consigue -t tiempo de respuesta*/
                } else if (strcmp(argv[i], "-t") == 0) {
                    *tiempo = atoi(argv[i+1]);

                    /*Si consigue -s suministro*/
                } else if (strcmp(argv[i], "-s") == 0) {
                    *sum = atoi(argv[i+1]);

                    /*Si consigue -p puerto del servidor*/
                } else if (strcmp(argv[i], "-p") == 0) {
                    *puerto = atoi(argv[i+1]);

                    /*Si no es un flag valido*/
                } else {

                    imprimirEC();
                    return -1;
                }

                /*Indico que ya conoce los valores de estas*/
                argv[i] = argv[i+1] = NULL;
            }



    }

    return 0;


}
