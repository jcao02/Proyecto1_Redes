#include <stdio.h>
#include <semaphore.h>
#include "errores.h"
#include "extra.h"
#define LIMITE 480

int gas = 0;			/*Cantidad de gasolina actual*/
int max = 0;			/*Capacidad maxima de la bomba*/
int pet = 1;			/*Numero de peticiones a hacer*/
struct distr *centros;	/*Arreglo con los centros de distribuicion*/
int numCentros;			/*Numero de centros de distribuicion*/
sem_t sem;				/*Semaforo para control de acceso a 'gas' y 'pet'*/

void *pedirGas() {

	int i;					/*Variable de uso generico*/
	int respuesta;

	/*Itera sobre todos los centros hasta conseguir uno disponible*/
	for (i = 0; i < numCentros; i = (i + 1) % numCentros) {

		//MONTA CONEXION
	}

	respuesta = 1; //mientras

	usleep(respuesta * MINUTO);

	/*Wait para accesar a 'gas' y 'pet'*/
	sem_wait(&sem);

	/*Agrega la carga del envio e indica que la peticion fue atendida*/
	gas += CARGA;
	--pet;

	/*Signal para liberar 'gas' y 'pet'*/
	sem_post(&sem);

}

int main(int argc, char **argv) {
	
	int tiempo = 0;			/*Tiempo transcurrido*/
	int salida = 0;			/*Consumo en litros por minuto de gasolina*/
	int espera = MAX_INT;	/*Tiempo de espera minimo*/
	pthread_t mensajero; 	/*Hilo que hace peticiones de gasolina*/
	
	//llamada(...);

	/*Inicializa el semaforo en 1*/
	sem_init(&sem, 0, 1);

	/*mientras no pasen las 8 horas*/
	while (tiempo < LIMITE) {
		
		++tiempo;
		usleep(MINUTO);

		/*Wait para accesar a 'gas' y 'pet'*/
		sem_wait(&sem);
		gas -= salida;

		/*Si hay espacio suficiente en el inventario*/
		if (max - (gas * espera) >= CARGA * pet) {
			
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