#include <stdio.h>
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

void *enviarGas(){

    /*wait para accesar a 'gas'*/
    sem_wait(&sem);

    if (gas >= CARGA){

        // Aqui se le dice que Si al cliente
        gas -= CARGA;
        printf("HILO\n");

        // Escribir en el log
    } else { 

        // Aqui se le dice que No al cliente

        // Escribir en el log
    }

    sem_post(&sem);
    

    /*Conexion con el cliente*/
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

        if ((newfd = accept(fd, (struct sockaddr *) &Bdir, &Btam)) < 0){

            return errorSocket(); 
        }

        printf("----------------------\n");
        /*peticion */
        if (pthread_create(&mensajero, NULL, enviarGas, NULL) != 0){

            return errorHilo();
        }

        /*Recuperar*/
        if (pthread_detach(mensajero) != 0){
            
            return errorHilo();
        }
    }


	return 0;
}
