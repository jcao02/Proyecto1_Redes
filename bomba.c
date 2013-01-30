#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include "queue.h"
#include "errores.h"
#define LIMITE 480

char *nombre = NULL;	/*Nombre de la bomba*/
int gas = 0;			/*Cantidad de gasolina actual*/
int max = 0;			/*Capacidad maxima de la bomba*/
int pet = 1;			/*Numero de peticiones a hacer*/
queue centros = NULL;	/*Cola de prioridad con los centros de distribuicion*/
sem_t sem;				/*Semaforo para control de acceso a 'gas' y 'pet'*/

int conectar_centro(char tipo, int puerto, char *DNS) {

	char *respuesta;			/*Respuesta del servidor*/
	int fd;						/*File descriptor del socket*/
	struct sockaddr_in Cdir;	/*Estructura para el socket del servidor (centro de distribucion)*/

	/*Si la peticion no es ni de gasolina ni de tiempo de respuesta*/
	if ((tipo != 'G') && (tipo != 'g') && (tipo != 'T') && (tipo != 't')) {

		/*Respuesta negativa*/
		return -1;
	}
printf("tipo: '%c' | puerto: '%d' | DNS: '%s'\n", tipo,puerto,DNS);
	if ((respuesta = (char *) malloc(4 * sizeof(char))) == NULL) {

		errorMem();
		return -1;
	}

	/*Abriendo socket con protocolo TCP*/
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		free(respuesta);
		errorSocket();
		return -1;
	}

	/*Inicializacion de la estructura 'Cdir'*/
	bzero(&Cdir, sizeof(Cdir));
	Cdir.sin_family = AF_INET;
	Cdir.sin_addr = *((struct in_addr *) gethostbyname(DNS)->h_addr);
	Cdir.sin_port = htons(puerto);
printf("CONECTARE\n");
	/*Conecto con el servidor*/
	if (connect(fd,(struct sockaddr *) &Cdir, sizeof(Cdir)) < 0) {

		free(respuesta);
		close(fd);
		//errorSocket();
		return -1;
	}
printf("EXITO\n");
	/*Enviamos peticion de gasolina*/
	if (write(fd, &tipo, 1) != 1) {

		free(respuesta);
		close(fd);
		errorFile();
		return -1;
	}

	/*Esperamos la respuesta*/
	if (read(fd, respuesta, 4) < 1) {

		free(respuesta);
		close(fd);
		errorFile();
		return -1;
	}

	respuesta[3] = '\0';

	close(fd);

	return  atoi(respuesta);
}

int analizar_fichero(char *fich) {

	FILE *fd = NULL;		/*File descriptor del fichero de centros*/
	char buffer[100];		/*Buffer de lectura para el archivo.*/
	distr cent;				/*Variable que representa un centro*/
	char *nom, *DNS;		/*Nombre y DNS de centro de distribucion*/
	int puerto, respuesta;	/*Puerto y tiempo de respuesta de centro de distribucion*/
	int min_resp = MAX_INT;	/*Tiempo de respuesta minimo de los centros de distribucion*/
	int i;					/*Variable de uso generico*/

	/*Abre el archivo*/
	if ((fd = fopen(fich, "r")) == NULL) {
		return errorFile();
	}

	/*Lee el archivo hasta el final*/
	while (fscanf(fd, "%s", buffer) != EOF) {

		nombre = strtok(buffer, "&");
		DNS = strtok(NULL, "&");
		puerto = atoi(strtok(NULL, "&"));

		/*Intenta conseguir una conexion con el servidor 5 veces, si no lo logra lo ignora*/
		for (i = 0; i < 5; ++i) {

			/*Para conseguir el tiempo de respuesta de este servidor*/
			if ((respuesta = conectar_centro('t', puerto, DNS)) >= 0) {

				break;
			}
		}

		/*Si no logro conectarse con el servidor*/
		if (i == 5) {

			continue;
		}

		/*Si consegui un nuevo minimo tiempo de respuesta*/
		if (respuesta < min_resp) {
			min_resp = respuesta;
		}

		if((cent = create_distr(nombre, DNS, puerto, respuesta)) == NULL) {
			return -1;
		}

		if (!add(&centros, cent)) {
			return -1;
		}
	}

	/*Si no logro conectarse nunca*/
	if (is_empty(centros)) {

		return 0;
	}

	return min_resp;
}

void *pedir_gas() {

	int respuesta, envio;		/*Tiempo de respuesta del centro y si enviara la gasolina*/
	iterator it = NULL;			/*Iterador sobre la cola de prioridad*/
	distr cent;					/*Para el chequeo de la cola*/

printf("--------------HILO\n");

	/*Itera sobre todos los centros hasta conseguir uno disponible*/
	for (cent = next_it(it = create_iterator(centros)); ; cent = next_it(it)) {

		/*Si ya reviso todos los elementos, volvemos a comenzar*/
		if (cent == NULL) {

			it = create_iterator(centros);
			continue;
		}

		envio = conectar_centro('g', cent->puerto, cent->DNS);

		/*Si el servidor responde positivamente*/
		if (envio != -1 && envio != 0) {

			break;
		}
	}

	/*Tiempo de respuesta en llegar la gasolina*/
	respuesta = cent->pr;
	usleep(respuesta * MINUTO);

	/*Wait para accesar a 'gas' y 'pet'*/
	sem_wait(&sem);

	/*Agrega la carga del envio e indica que la peticion fue atendida*/
	gas += CARGA;
	--pet;
printf("----------------LLEGO CARGA\n");
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
	
	/*Inicializa el semaforo en 1*/
	sem_init(&sem, 0, 1);

	/*Si hubo error en la invocacion del porgrama*/
	if (llamadaB(argc, argv, &nombre, &fich, &max, &gas, &salida) < 0) {
		return -1;
	}

	printf("Nombre: '%s'\nFichero: '%s'\nCapacidad: '%d'\n",nombre, fich, max);
	printf("Inicial: '%d'\nConsumo: '%d'\n", gas, salida);

	/*Revisa el fichero de centros, si hay error termina el programa.*/
	if ((espera = analizar_fichero(fich)) < 0) {
		return -3;
	}

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
		while (max - (gas - (espera) * salida) >= CARGA * pet) {

			/*Aumenta el numero de peticiones a hacer*/
			++pet;
			/*Crea un hilo que maneje la peticion*/
			if (pthread_create(&mensajero, NULL, pedir_gas, NULL) != 0) {

				errorHilo();
			}
		}

		/*Signal para liberar 'gas' y 'pet'*/
		sem_post(&sem);
	}

	return 0;
}