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
#include <termios.h>
#include "client.h"

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

    if ((numbytes = recv( sockfd , buf , buflen,   0 )) == -1  )//recibo el string
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
    int i = 0;

    printf("\nSu mano: ");
    for(i = 0; i < 3;i++)
    {
        if(strcmp(manoCliente[i], "x") != 0)
            printf("%s\t", manoCliente[i]);
        
    }

    printf("\n\n");
}

void recibirGrilla(char grillaP[][4][90])
{
    int i = 0, numbytes = 0, j = 0;
    for(i = 0;i < 4; i++)
    {
        for(j = 0; j < 90;j++)
        {
            if ((numbytes = recv( sockfd , &((*grillaP)[i][j]), sizeof(char),  0 )) == -1  )//recibo caracter por caracter
            {
                perror("recv");
                close(sockfd);
                exit(1);
            }
        }
    }
    revertirGrilla(grillaP);//la revierto
}

void imprimirGrilla(char grilla[4][90])
{
    //imprimo 2 \n para mejorar la visibilidad e imprimo la grilla caracter por caracter
    int i = 0, j = 0;

    printf("\n");

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 90; j++)
            printf("%c", grilla[i][j]);
    }

    printf("\n");
}

void revertirGrilla(char grillaP[][4][90])
{
    //revierte la orientacion de la grilla para el punto de vista del jugador cliente
    char fila1[90], fila3[90];
    int i = 0;

    for(i = 0; i < 90; i++)
    {
        fila1[i]= (*grillaP)[1][i];
    }

    for(i = 0; i < 90; i++)
    {
        fila3[i]= (*grillaP)[3][i];
    }

    for(i = 0; i < 90; i++)
    {
        (*grillaP)[1][i] =fila3[i];
    }

    for(i = 0; i < 90; i++)
    {
        (*grillaP)[3][i] =fila1[i];
    }
    
}

void recibirFlag(int *flagp)
{
    //flag = 0 turno del server
    //flag = 1 turno del cliente
    //flag = 2 nueva mano
    //flag = 3 fin del partido
    int numbytes = 0;

    if ((numbytes = recv( sockfd , flagp, sizeof(int),  0 )) == -1  )//recibo la flag
        {
            perror("recv");
            close(sockfd);
            exit(1);
        }
}

void imprimirOpciones(char manoClienteP[][3][17])
{
    //recibe las opciones disponibles y las imprime por pantalla
    //luego manda la opcion seleccionada por el cliente
    char buf[MAXDATASIZE];
    int cantOpciones = 0, opciones[MAXDATASIZE], opcionElegida = 0, i = 0, flag = 0, numbytes = 0;

    while(strcmp(buf, "Fin")!= 0)//mientras no reciba "Fin"
    {
        recibirString(buf);

        if(strcmp(buf, "Fin")!= 0)
        {
            printf("%s\n", buf);//imprimo la opcion
            opciones[cantOpciones]=atoi(strtok(buf, " "));//guardo el numero de opcion
            cantOpciones++;
        }
    }
    
    //pido por teclado la opcion
	while(flag == 0)
	{
        clean_stdin();
		fgets(buf, 4, stdin);
		opcionElegida = atoi(buf);
			
		//compruebo que la opcion sea correcta
		for(i = 0;i < cantOpciones; i++)
		{
			if(opcionElegida == opciones[i])
			flag = 1;
		}
        if(flag == 0)
        {
			printf("\nLa opcion ingresada no esta disponible\n");
            printf("Ingrese nuevamente la opcion deseada\n");
        }
	}

    //envio la opcion
    if ( ( numbytes = send(sockfd, &opcionElegida, sizeof(int), 0) ) == -1 )
    {
        perror("recv");
        close(sockfd);
        exit(1);
    }

    //elimino la carta del mazo en caso de corresponder
    if(opcionElegida == 1 || opcionElegida == 2 || opcionElegida == 3)
    strcpy((*manoClienteP)[opcionElegida - 1], "x");

}

void clean_stdin(void)
{
    //limpia el stdin para usarlo con fgets
	int stdin_copy = dup(STDIN_FILENO);
    tcdrain(stdin_copy);
    tcflush(stdin_copy, TCIFLUSH);
    close(stdin_copy);
}
