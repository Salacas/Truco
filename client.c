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

#define PORT 3490 					// puerto al que vamos a conectar
#define MAXDATASIZE 100 			// máximo número de bytes que se pueden leer de una vez

int sockfd;
//prototipos-----------------------------
void recibirCartas(char manoCliente[][3][17] );

void enviarString(char *buf);

void recibirString(char *buf);

void imprimirMano(char manoCliente[3][17]);

//funciones------------------------------
void recibirCartas(char manoCliente[][3][17] )
{
	int buflen = 0, i = 0, numbytes = 0;

    for( i = 0; i < 3; i++)
    {
    	if ((numbytes = recv( sockfd , &buflen, sizeof(int),  0 )) == -1  )//la cantidad de datos a recibir en el proximo recv
    	{
        	perror("recv");
        	close(sockfd);
        	exit(1);
    	}
    	if ((numbytes = recv( sockfd , (*manoCliente)[i] , buflen,   0 )) == -1  )//recibo la mano del cliente
    	{
        	perror("recv");
        	close(sockfd);
        	exit(1);
    	}
    	(*manoCliente)[i][numbytes] = '\0';

    	if(strlen((*manoCliente)[i]) != buflen)
    	{
    		printf("Error al recibir la mano\n");
    		close(sockfd);
        	exit(1);
    	}
    }
}

void enviarString(char *buf)
{
    int aux = 0, numbytes = 0;

    aux = strlen(buf);

    if ( ( numbytes = send(sockfd, &aux, sizeof(int), 0) ) == -1 )//la cantidad de datos a enviar en el proximo send
    {
        perror("recv");
        close(sockfd);
        exit(1);
    }

    if ( ( numbytes = send(sockfd, buf, strlen(buf), 0) ) == -1 )//envio la mano del cliente
    {
        perror("recv");
        close(sockfd);
        exit(1);
    }
}

void recibirString(char *buf)
{
    int buflen = 0, numbytes = 0;

    if ((numbytes = recv( sockfd , &buflen, sizeof(int),  0 )) == -1  )//la cantidad de datos a recibir en el proximo recv
    {
        perror("recv");
        close(sockfd);
        exit(1);
    }

    if ((numbytes = recv( sockfd , buf , buflen,   0 )) == -1  )//recibo la mano del cliente
    {
        perror("recv");
        close(sockfd);
        exit(1);
    }

    buf[numbytes] = '\0';

    if(strlen(buf) != buflen)
    {
        printf("Error al recibir la string\n");
        close(sockfd);
        exit(1);
    }
}

void imprimirMano(char manoCliente[3][17])
{
    printf("Su mano: %s\t%s\t%s\n", manoCliente[0], manoCliente[1], manoCliente[2]);
}

int main(int argc, char *argv[])
{
	struct in_addr addr;
	int numbytes = 0, flag = 0, i = 0, buflen = 0;
	char buf[MAXDATASIZE], nombreServer[MAXDATASIZE], nombreCliente[MAXDATASIZE];
	struct sockaddr_in their_addr;
	//char manoServer[3][17];//la carta mas "larga" seria "Cuatro de espadas"(16 letras) + NULL
   	char manoCliente[3][17];

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

	printf("Conexion exitosa con el servidor\n");
	printf("Ingrese su nombre: ");
	scanf("%s", nombreCliente);

    recibirString(nombreServer);//recibo el nombre del jugador server
    enviarString(nombreCliente);//envio el nombre del jugador cliente
/*
    while(flag == 0)
    {
        recibirCartas(&manoCliente);



    }
  */
    recibirCartas(&manoCliente);
  	printf("Mi mano:%s%s%s\n", manoCliente[0], manoCliente[1], manoCliente[2]);
	close(sockfd);

	return 0;
}
