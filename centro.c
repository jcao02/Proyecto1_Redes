#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "errores.h"
#define MINUTO 100000

int gas     = 0;    /*Cantidad de gasolina actual*/
int max     = 0;    /*Cantidad maxima de gasolina*/
int entrada = 0;    /*Cantidad de gasolina que entra en litros por minuto*/
sem_t sem;          /*Semaforo para control de acceso a 'gas'*/

void *controlGas(){

    /*Ciclo infinito para montar el servidor*/
    while (1){
        /*Duerme por un minuto (0.1 seg)*/
        usleep(MINUTO);

        /*wait para accesar a 'gas'*/
        sem_wait(&sem);

        /*Si hay menos espacio que la entrega por minuto*/
        if (entrada + gas >= max){

            gas = max;
        }
        else{

            gas += entrada;
        }
        
        /*signal para liberar 'gas'*/
        sem_post(&sem);
    }
    
    /*Termina el hilo*/
    pthread_exit(NULL);
}

void *enviarGas(void* i){

    /*Conexion con el cliente*/
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t controlador;     /*Hilo que controla la entrada de gasolina al centro*/
    pthread_t mensajero;       /*Hilo que responde a las llamadas de las bombas*/
    
    /*inicializacion semaforo*/
    sem_init(&sem,0,1);
    /*procedimiento que obtiene los valores generales*/
    
    /*Creo hilo que controla entrada de gasolina*/
    if (pthread_create(&controlador, NULL, controlGas, NULL) != 0){

        errorHilo();
    }

    /*aqui la conexion con el cliente*/

    while (1){

        /*peticion */
        if (pthread_create(&mensajero, NULL, enviarGas, NULL) != 0){

            errorHilo();
        }
    }


	return 0;
}
