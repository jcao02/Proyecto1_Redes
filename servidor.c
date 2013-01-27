/*
 * Prueba conexion: Servidor
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define minuto 100000

int main(int argc, char **argv){
    int sockfd; /*file descriptor socket*/
    struct sockaddr_in  mi_dir; 
    struct sockaddr_in  su_dir; 
    int tiempo=0;

    while (tiempo < 480){
        minuto;
        printf("%d\n",tiempo);
    }
}
