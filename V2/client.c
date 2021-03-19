#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

int main(int argc, char *argv[])
{
	struct in_addr addr;
	int  manonumero = 0, flag = 0;
	char  nombreServer[MAXDATASIZE], nombreCliente[MAXDATASIZE], buf[MAXDATASIZE];
	struct sockaddr_in their_addr;
   	char manoCliente[3][17];
    char grilla[4][90];

	if (argc != 2) 
	{
		fprintf(stderr,"usage: client hostname\n");
		exit(1);
	}
	
    if (inet_aton(argv[1], &addr) == 0) 
    {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1 ) 
	{
		perror("socket");
		exit(1);
	}
	
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr = addr;

	memset(&(their_addr.sin_zero), 0 , 8);
	
	if ( connect( sockfd , (struct sockaddr *)&their_addr,	sizeof(struct sockaddr)) == -1 ) 
	{
		perror("connect");
		close(sockfd);
		exit(1);
	}

    //inicio de conexion--------------------------------------------------------------------
	printf("Conexion exitosa con el servidor\n");
	printf("Ingrese su nombre: ");
    scanf("%s", nombreCliente);

    recibirString(&nombreServer);//recibo el nombre del jugador server
    enviarString(nombreCliente);//envio el nombre del jugador cliente

    while(flag != 3)//flag = 3 significa que termino el juego
    { 
        recibirFlag(&flag);

        if(flag == 3)//finaliza el partido
        {
            system("clear");
            recibirGrilla(&grilla);
            revertirGrilla(&grilla);
            imprimirGrilla(grilla);
            imprimirMano(manoCliente);
            printf("Ha finalizado el partido\n");
        }
        else if(flag == 2)//empieza una mano
        {
            manonumero++;
            if(manonumero != 1)
            {
                system("clear");
                imprimirGrilla(grilla);
                imprimirMano(manoCliente);
                printf("\nFinalizo la mano\n");
                sleep(4);
            }
            recibirCartas(&manoCliente);
        }
        else if(flag == 0)//turno del server
        {
            system("clear");
            recibirGrilla(&grilla);
            revertirGrilla(&grilla);
            imprimirGrilla(grilla);
            printf("Turno de %s\n", nombreServer);
            imprimirMano(manoCliente);
        }
        else if(flag == 1)//turno del cliente
        {
            system("clear");
            recibirGrilla(&grilla);
            revertirGrilla(&grilla);
            imprimirGrilla(grilla);
            recibirString(&buf);
            if(strcmp(buf, "Nada")!=0)
            printf("\n%s\n", buf);
            imprimirMano(manoCliente);
            imprimirOpciones(&manoCliente);
        }

    }

	close(sockfd);

	return 0;
}
