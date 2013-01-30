#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include "extra.h"
#include "errores.h"
#define MINUTO 100000
#define QLENGTH 5


char *nombre = NULL;	/*Nombre de la bomba*/
int gas      = 0;       /*Cantidad de gasolina actual*/
int max      = 0;       /*Cantidad maxima de gasolina*/
int entrada  = 0;       /*Cantidad de gasolina que entra en litros por minuto*/
int tiempo   = 0;       /*Tiempo de respuesta del centro*/
sem_t sem;              /*Semaforo para control de acceso a 'gas'*/

/*
 * Funcion que aumenta el inventario de gasolina del centro
 */
void *controlGas(){

    /*Ciclo infinito que recibe gasolina*/
    while (1){
        /*Duerme por un minuto (0.1 seg)*/
        usleep(MINUTO);

        printf("%d\n",gas);

        /*wait para accesar a 'gas'*/
        sem_wait(&sem);

        /*Si hay menos espacio que la entrega por minuto*/
        if (entrada + gas >= max){

            gas = max;
        } else{

            gas += entrada;
        }
        
        /*signal para liberar 'gas'*/
        sem_post(&sem);
    }
    
    /*Termina el hilo*/
    pthread_exit(NULL);
}

void *manejo_peticion(void *fd){

    int newfd = (int) fd;
    char pet;
    char *res;

    if (read(newfd, &pet, 1) != 1) {
        
        errorFile();
        pthread_exit(NULL);
    }

    printf("\n\n-------------------------------------- '%c'\n\n", pet);

    /*Si es una peticion de tiempo de respuesta*/
    if (pet == 't' || pet == 'T') {

        /*Memoria para el string de respuesta*/
        if ((res = (char *) calloc(4, sizeof(char))) == NULL) {

            errorMem();
            pthread_exit(NULL);
        }

        sprintf(res, "%d", tiempo);

        /*Escribe el tiempo de respuesta al cliente*/
        if (write(newfd, res, 4) < 1) {

            free(res);
            errorFile();
            pthread_exit(NULL);
        }

        pthread_exit(NULL);
    }

    if (pet != 'g' && pet != 'G') {

        pthread_exit(NULL);
    }

    /*wait para accesar a 'gas'*/
    sem_wait(&sem);

    /*Si tengo suficiente gasolina*/
    if (gas >= CARGA){

        pet = '1';

        /*Escribe que SI enviara la gasolina al cliente*/
        if (write(newfd, &pet, 1) != 1) {

            errorFile();
            pthread_exit(NULL);
        }

        gas -= CARGA;
        printf("HILO\n");

        // Escribir en el log
    /*Si no hay suficiente gasolina*/
    } else { 

        pet = '0';

        /*Escribe que NO enviara la gasolina al cliente*/
        if (write(newfd, &pet, 1) != 1) {

            errorFile();
            pthread_exit(NULL);
        }

        // Escribir en el log
    }

    sem_post(&sem);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t controlador;              /*Hilo que controla la entrada de gasolina al centro*/
    pthread_t mensajero;                /*Hilo que responde a las llamadas de las bombas*/
    int puerto;                         /*Puerto a utilizar por el servidor*/
    int fd, newfd;                      /*File descriptor del socket y del socket que recibe la conexion*/
    struct sockaddr_in Bdir, Cdir;      /*Estructuras para los sockets*/
    socklen_t Btam;                     /*Tamaño del 'Bdir'*/
    
    /*Inicializacion semaforo en 1*/
    sem_init(&sem,0,1);

    /*procedimiento que obtiene los valores generales*/
	if (llamadaC(argc, argv, &nombre, &max, &gas, &tiempo, &entrada, &puerto) < 0) {
		return -1;
	}

    printf("Nombre: %s\nCapacidad: %d\nInventario: %d\nTiempo: %d\nSuministro: %d\nPuerto: %d\n",
            nombre, max, gas, tiempo, entrada, puerto);

    
    /*Creo hilo que controla entrada de gasolina*/
    if (pthread_create(&controlador, NULL, controlGas, NULL) != 0){

        return errorHilo();
    }

    /*El servidor se monta para recibir conexiones*/

    /*Abriendo socket con protocolo TCP*/
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        
        return errorSocket();
    }

    /*Inicializacion de la estructura 'Bdir' para el bind*/
    bzero(&Bdir, sizeof(Bdir));
    Cdir.sin_family = AF_INET;
    Cdir.sin_addr.s_addr = htonl(INADDR_ANY); 
    Cdir.sin_port = htons(puerto);

    /*Nombramos al socket con bind*/
    if (bind(fd, (struct sockaddr *) &Cdir, sizeof(Cdir)) != 0){
            
        return errorSocket();
    }

    if (listen(fd, QLENGTH) < 0){

        return errorSocket();
    }

    /*Ciclo para recibir conexiones*/
    while (1){

        Btam = sizeof(Bdir);
        /*Se aceptan conexiones con el cliente*/

        printf("----------------------\n");

        if ((newfd = accept(fd, (struct sockaddr *) &Bdir, &Btam)) < 0){

            return errorSocket(); 
        }

        /*peticion */
        if (pthread_create(&mensajero, NULL, manejo_peticion, (void *) newfd) != 0){

            return errorHilo();
        }

        /*Recuperar*/
        if (pthread_detach(mensajero) != 0){
            
            return errorHilo();
        }
    }


	return 0;
}
