#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "queue.h"
#include "errores.h"
#define LIMITE 480

char *nombre = NULL;	/*Nombre de la bomba*/
int gas = 0;			/*Cantidad de gasolina actual*/
int max = 0;			/*Capacidad maxima de la bomba*/
int pet = 1;			/*Numero de peticiones a hacer*/
queue centros = NULL;	/*Cola de prioridad con los centros de distribuicion*/
sem_t sem;				/*Semaforo para control de acceso a 'gas' y 'pet'*/

int analizarFichero(char *fich) {

	FILE *fd = NULL;		/*File descriptor del fichero de centros*/
	char buffer[100];		/*Buffer de lectura para el archivo.*/
	distr cent;				/*Variable que representa un centro*/
	char *nom, *DNS;		/*Nombre y DNS de centro de distribucion*/
	int puerto, respuesta;	/*Puerto y tiempo de respuesta de centro de distribucion*/
	int min_resp = MAX_INT;	/*Tiempo de respuesta minimo de los centros de distribucion*/

	/*Abre el archivo*/
	if ((fd = fopen(fich, "r")) == NULL) {
		return errorFichero();
	}

	/*Lee el archivo hasta el final*/
	while (fscanf(fd, "%s", buffer) != EOF) {

		nombre = strtok(buffer, "&");
		DNS = strtok(NULL, "&");
		puerto = atoi(strtok(NULL, "&"));
		respuesta = 1;
		//respuesta ===== CONEXION A EL SERVIDOR PARA PEDIR TIEMPO DE RESPUESTA

		/*Si consegui un nuevo minimo tiempo de respuesta*/
		if (respuesta < min_resp) {
			min_resp = respuesta;
		}

		printf("'%s', '%s', '%d', '%d'\n",nombre, DNS, puerto, respuesta);
		if((cent = create_distr(nombre, DNS, puerto, respuesta)) == NULL) {
			return -1;
		}

		if (!add(&centros, cent)) {
			return -1;
		}
	}

	return min_resp;
}

void *pedirGas() {

	int respuesta = 0;		/*Tiempo de respuesta del centro*/
	iterator it = NULL;		/*Iterador sobre la cola de prioridad*/
	distr cent;				/*Para el chequeo de la cola*/

	printf("HILO\n");

	//EN EL for QUITAR EL '0' DE LA CONDICION, DEBE SER: ''  (si vacio, es TRUE en un for)
	//ESTA PORQUE NO HEMOS HECHO LO DE REDES
	/*Itera sobre todos los centros hasta conseguir uno disponible*/
	for (cent = next_it(it = create_iterator(centros)); 0; cent = next_it(it)) {

		/*Si ya reviso todos los elementos, volvemos a comenzar*/
		if (cent == NULL) {
			it = create_iterator(centros);
			continue;
		}

		//MONTA CONEXION
	}

	respuesta = cent->pr;

	respuesta = 20; //recibe tiempo de respuesta

	usleep(respuesta * MINUTO);

	/*Wait para accesar a 'gas' y 'pet'*/
	sem_wait(&sem);

	/*Agrega la carga del envio e indica que la peticion fue atendida*/
	gas += CARGA;
	--pet;

	/*Signal para liberar 'gas' y 'pet'*/
	sem_post(&sem);

	pthread_exit(NULL);
}

int main(int argc, char **argv) {

	char *fich;					/*Nombre del fichero con lista de centros*/
	int tiempo = 0;				/*Tiempo transcurrido*/
	int salida = 0;				/*Consumo en litros por minuto de gasolina*/
	int espera = MAX_INT;		/*Tiempo de espera minimo*/
	pthread_t mensajero; 		/*Hilo que hace peticiones de gasolina*/
	int fd;						/*File descriptor del socket*/
	struct sockaddr_in Cdir;	/**/
	
	/*Si hubo error en la invocacion del porgrama*/
	if (llamadaB(argc, argv, &nombre, &fich, &max, &gas, &salida) < 0) {
		return -1;
	}

	printf("nombre: '%s'\n fichero: '%s'\n capacidad: '%d'\n",nombre, fich, max);
	printf("inicial: '%d'\n consumo: '%d'\n", gas, salida);

	/*Revisa el fichero de centros, si hay error termina el programa.*/
	if ((espera = analizarFichero(fich)) < 0) {
		return -3;
	}

	/*Inicializa el semaforo en 1*/
	sem_init(&sem, 0, 1);

	/*mientras no pasen las 8 horas*/
	while (tiempo < LIMITE) {
		
		++tiempo;
		usleep(MINUTO);

		printf("%d\n", gas);
		
		/*Wait para accesar a 'gas' y 'pet'*/
		sem_wait(&sem);

		/*Si no se ha acabado la gasolina*/
		if (gas - salida >= 0) {
			gas -= salida;
		/*Si se acabo la gasolina*/
		} else {
			gas = 0;
		}

		/*Si hay espacio suficiente en el inventario*/
		while (max - (gas * espera) >= CARGA * pet) {

			/*Aumenta el numero de peticiones a hacer*/
			++pet;
			/*Crea un hilo que maneje la peticion*/
			if (pthread_create(&mensajero, NULL, pedirGas, NULL) != 0) {

				errorHilo();
			}
		}

		/*Signal para liberar 'gas' y 'pet'*/
		sem_post(&sem);
	}

	return 0;
}