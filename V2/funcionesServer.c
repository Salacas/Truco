#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "serverHeader.h"

void enviarCartas(char manoCliente[][3][17])
{
	int i = 0, numbytes = 0, aux = 0;

	for(i = 0; i < 3; i++)
   	{
   		aux = strlen((*manoCliente)[i]);
    	if ( ( numbytes = send(new_fd, &aux, sizeof(int), 0) ) == -1 )//la cantidad de datos a enviar en el proximo send
    	{
        	perror("recv");
        	close(new_fd);
        	close(sockfd);
        	exit(1);
    	}

   		if ( ( numbytes = send(new_fd, (*manoCliente)[i], strlen((*manoCliente)[i]), 0) ) == -1 )//envio la mano del cliente
    	{
        	perror("recv");

        	close(new_fd);
        	close(sockfd);
        	exit(1);
    	}
    }
}

void inicializarMazo (CARTA * mazo, char *numero[], char *palo[])
{
	int i;

	for(i = 0; i < 40; i++)
	{
		strcpy(mazo[i].numero, numero[i % 10]);
		strcpy(mazo[i].palo,  palo[i /10]);
	}

}

void mezclar (CARTA *mazo)
{
	//mezclo y agrego los valores del truco a las cartas
	int i = 0, j = 0;
	CARTA aux;

	srand(time(NULL));

	for ( i = 0; i < 40; i++)
	{
		j = rand() % 40;
		strcpy(aux.numero, mazo[i].numero);
		strcpy(aux.palo, mazo[i].palo);

		strcpy(mazo[i].numero, mazo[j].numero);
		strcpy(mazo[i].palo, mazo[j].palo);

		strcpy(mazo[j].numero, aux.numero);
		strcpy(mazo[j].palo, aux.palo);
	}

	for(i = 0; i < 40; i++)
	{
			if(strcmp(mazo[i].numero,"4") == 0)
				mazo[i].valor = 0;
			if(strcmp(mazo[i].numero,"5") == 0)
				mazo[i].valor = 1;
			if(strcmp(mazo[i].numero,"6") == 0)
				mazo[i].valor = 2;
			if(strcmp(mazo[i].numero,"7") == 0)
			{
				if(strcmp(mazo[i].palo, "Basto") == 0 || strcmp(mazo[i].palo, "Copa") == 0)
					mazo[i].valor = 3;
				else if(strcmp(mazo[i].palo, "Oro") == 0)
					mazo[i].valor = 10;
				else if(strcmp(mazo[i].palo, "Espada") == 0)
					mazo[i].valor = 11;
			}
			if(strcmp(mazo[i].numero,"10") == 0)
				mazo[i].valor = 4;
			if(strcmp(mazo[i].numero,"11") == 0)
				mazo[i].valor = 5;
			if(strcmp(mazo[i].numero,"12") == 0)
				mazo[i].valor = 6;
			if(strcmp(mazo[i].numero,"1") == 0)
			{
				if(strcmp(mazo[i].palo, "Oro") == 0 || strcmp(mazo[i].palo, "Copa") == 0)
					mazo[i].valor = 7;
				else if(strcmp(mazo[i].palo, "Basto") == 0)
					mazo[i].valor = 12;
				else if(strcmp(mazo[i].palo, "Espada") == 0)
					mazo[i].valor = 13;
			}
			if(strcmp(mazo[i].numero,"2") == 0)
				mazo[i].valor = 8;
			if(strcmp(mazo[i].numero,"3") == 0)
				mazo[i].valor = 9;
		
	}
}

void repartir(CARTA *mazo, char manoServer[][3][17], char manoCliente[][3][17])
{	
	int i = 0;

	for(i = 0; i < 6; i++)
	{	
		if(i < 3)//las tres primeras cartas del mazo van al jugador server
		{
			strcpy((*manoServer)[i], mazo[i].numero);
			strcpy(&((*manoServer)[i][strlen(mazo[i].numero)]), " de ");//strlen((*manoServer)[i])
			strcpy(&((*manoServer)[i][strlen(mazo[i].numero)+ 4]), mazo[i].palo);
		}
		else if(i >= 3)//las ultimas tres al jugador cliente
		{
			strcpy((*manoCliente)[i-3], mazo[i].numero);
			strcpy(&((*manoCliente)[i-3][strlen(mazo[i].numero)]), " de ");
			strcpy(&((*manoCliente)[i-3][strlen(mazo[i].numero)+ 4]), mazo[i].palo);
		}
	}
}

void enviarString(char *buf)
{
	int aux = 0, numbytes = 0;

	aux = strlen(buf);

    if ( ( numbytes = send(new_fd, &aux, sizeof(int), 0) ) == -1 )//la cantidad de datos a enviar en el proximo send
    {
        perror("recv");
        close(new_fd);
        close(sockfd);
        exit(1);
    }

   	if ( ( numbytes = send(new_fd, buf, strlen(buf), 0) ) == -1 )//envio la string
    {
        perror("recv");
        close(new_fd);
        close(sockfd);
        exit(1);
    }
}

void recibirString(char *buf)
{
	int buflen = 0, numbytes = 0;

	if ((numbytes = recv( new_fd , &buflen, sizeof(int),  0 )) == -1  )//la cantidad de datos a recibir en el proximo recv
    {
        perror("recv");
        close(new_fd);
        close(sockfd);
        exit(1);
    }

    if ((numbytes = recv( new_fd , buf , buflen,   0 )) == -1  )//recibo la mano del cliente
    {
        perror("recv");
        close(new_fd);
        close(sockfd);
        exit(1);
    }

    buf[numbytes] = '\0';

    if(strlen(buf) != buflen)
    {
    	printf("Error al recibir la string\n");
    	close(new_fd);
    	close(sockfd);
        exit(1);
    }
}

void inicializarGrilla(char grilla[][4][90], char *nombreServer, char *nombreCliente, int puntosMaximos)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++)//inicializo todo con espacios
	{
		for(j = 0; j < 90; j++)
			(*grilla)[i][j]= ' ';
	}
	//hago la primera linea
	strcpy((*grilla)[0],"          Primera          Segunda          Tercera          ");
	strcpy(&((*grilla)[0][strlen((*grilla)[0])]), nombreServer);
	strcpy(&((*grilla)[0][strlen((*grilla)[0])]), "     ");
	strcpy(&((*grilla)[0][strlen((*grilla)[0])]), nombreCliente);
	(*grilla)[0][strlen((*grilla)[0])]=' ';//reemplazo el null por un espacio

	//hago la segunda linea
	(*grilla)[1][13]='x';
	(*grilla)[1][30]='x';
	(*grilla)[1][47]='x';

	//en la tercera linea pongo el score
	(*grilla)[2][61+strlen(nombreServer)/2]='0';
	(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2]='0';

	//hago la cuarta linea
	(*grilla)[3][13]='x';
	(*grilla)[3][30]='x';
	(*grilla)[3][47]='x';
	(*grilla)[3][61+strlen(nombreServer)/2]='A';
	(*grilla)[3][61+strlen(nombreServer)/2+1]=' ';

	if(puntosMaximos == 15)
	{
		(*grilla)[3][61+strlen(nombreServer)/2+2]='1';
		(*grilla)[3][61+strlen(nombreServer)/2+3]='5';
	}
	else if(puntosMaximos == 30)
	{
		(*grilla)[3][61+strlen(nombreServer)/2+2]='3';
		(*grilla)[3][61+strlen(nombreServer)/2+3]='0';
	}

	(*grilla)[3][61+strlen(nombreServer)/2+4]=' ';
	(*grilla)[3][61+strlen(nombreServer)/2+5]='p';
	(*grilla)[3][61+strlen(nombreServer)/2+6]='t';
	(*grilla)[3][61+strlen(nombreServer)/2+7]='s';
	(*grilla)[3][61+strlen(nombreServer)/2+8]='.';

	//pongo saltos de linea
	(*grilla)[0][89]='\n';
	(*grilla)[1][89]='\n';
	(*grilla)[2][89]='\n';
	(*grilla)[3][89]='\n';


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

void imprimirMano(char manoServer[3][17])
{
	//imprime las cartas que no tengan la x
	int i = 0;

	fflush(stdin);

	printf("\nSu mano: ");
	for(i = 0; i < 3;i++)
	{
		if(strcmp(manoServer[i], "x") != 0)
			printf("%s\t", manoServer[i]);
		
	}

	printf("\n\n");
}

void ponerCartaServer(int numeroCarta, char manoServer[][3][17], char grilla[][4][90])
{
	//elimina la carta de la mano y la agrego a la grilla

	//busco el primer lugar donde no se haya puesto una carta
	if((*grilla)[3][13]=='x')//si no se puso una carta todavia
	{
		strcpy(&((*grilla)[3][13-strlen((*manoServer)[numeroCarta])/2]), (*manoServer)[numeroCarta]);
		(*grilla)[3][strlen((*grilla)[3])]=' ';//saco el null puesto por el strcpy
	}
	
	else//si se puso ya
	{
		if((*grilla)[3][30]=='x')
		{
			strcpy(&((*grilla)[3][30-strlen((*manoServer)[numeroCarta])/2]), (*manoServer)[numeroCarta]);
			(*grilla)[3][strlen((*grilla)[3])]=' ';
		}
		else
		{
			if((*grilla)[3][47]=='x')
			{
				strcpy(&((*grilla)[3][47-strlen((*manoServer)[numeroCarta])/2]), (*manoServer)[numeroCarta]);
				(*grilla)[3][strlen((*grilla)[3])]=' ';
			}
		}
	}
		

	

	strcpy((*manoServer)[numeroCarta], "x");//intercambio la carta de la mano por una x

}

void ponerCartaCliente(int numeroCarta, char manoCliente[][3][17], char grilla[][4][90])
{
    //elimina la carta de la mano y la agrego a la grilla

    //busco el primer lugar donde no se haya puesto una carta
    if((*grilla)[1][13]=='x')//si no se puso una carta todavia
    {
        strcpy(&((*grilla)[1][13-strlen((*manoCliente)[numeroCarta])/2]), (*manoCliente)[numeroCarta]);
        (*grilla)[1][strlen((*grilla)[1])]=' ';//saco el null puesto por el strcpy
    }
    else//si se puso ya
    {
        if((*grilla)[1][30]=='x')
        {
            strcpy(&((*grilla)[1][30-strlen((*manoCliente)[numeroCarta])/2]), (*manoCliente)[numeroCarta]);
            (*grilla)[1][strlen((*grilla)[1])]=' ';
        }
        else
        {
            if((*grilla)[1][47]=='x')
            {
                strcpy(&((*grilla)[1][47-strlen((*manoCliente)[numeroCarta])/2]), (*manoCliente)[numeroCarta]);
                (*grilla)[1][strlen((*grilla)[1])]=' ';
            }
        }
    }

    strcpy((*manoCliente)[numeroCarta], "x");//intercambio la carta de la mano por una x

}

void enviarGrilla(char grilla[4][90])
{
	int i = 0, numbytes = 0, j = 0;

	for(i = 0; i <4;i++)
	{
		for(j = 0;j < 90;j++)
		{
    		if ( ( numbytes = send(new_fd, &(grilla[i][j]), sizeof(char), 0) ) == -1 )//envia char por char
    		{
        		perror("recv");
        		close(new_fd);
        		close(sockfd);
        		exit(1);
  	  		}
  	  	}
  	}
}

void enviarFlag(int flag)
{
	int numbytes = 0;
	//flag = 0 turno del server
	//flag = 1 turno del cliente
	//flag = 2 empieza nueva mano
	//flag = 3 termino el juego
	if ( ( numbytes = send(new_fd, &flag, sizeof(int), 0) ) == -1 )
    {
       	perror("recv");
       	close(new_fd);
       	close(sockfd);
       	exit(1);
  	}
}

int imprimirOpciones(char manoServerP[][3][17], char compararP[][2][17],char grillaP[][4][90], int j, int *envidoP,int *trucoP,
 int *seCantoTrucoP, int *seCantoEnvidoP, int *hayQueProcesarEnvidoP)
{
	//imprime las opciones disponibles y pide por teclado la opcion deseada
    //ademas cambia las variables correspondientes

	//j indica si estamos en primera, segunda o tercera
    //cada opcion la identifico con un numero
    //de 1 a 3 es tirar cartas, 4 cantar truco, 5 cantar retruco, 6 cantar quiero vale cuatro
    // 7 cantar envido, 8 cantar real envido, 9 cantar falta envido
    // 10 querer, 11 no querer
	int i = 0, salida = 0, flag = 0, cantOpciones = 0, opciones[MAXDATASIZE];

	fflush(stdin);
	printf("Presione:\n");

	//imprimo las opciones para tirar cartas
    if(*seCantoTrucoP == 0 && *seCantoEnvidoP == 0)//si no tengo que aceptar envido ni truco
    {
	    for(i = 0; i < 3;i++)
	    {
		    if(strcmp((*manoServerP)[i], "x") != 0)//solo imprimo las cartas disponibles para tirar
		    {
			    printf("%d para tirar %s\n", i+1, (*manoServerP)[i]);
			    opciones[cantOpciones]=i+1;
			    cantOpciones++;
		    }
	    }
    }

    //imprimo las opciones del truco
    if(*seCantoEnvidoP == 0)//si no se esta cantando el envido
    {
        if(*trucoP == 1)
        {
            printf("4 para cantar truco\n");
            opciones[cantOpciones]=4;
            cantOpciones++;
        }
        else if(*trucoP == 2)
        {
            printf("5 para cantar retruco\n");
            opciones[cantOpciones]=5;
            cantOpciones++;
        }
        else if(*trucoP == 3)
        {
            printf("6 para cantar quiero vale cuatro\n");
            opciones[cantOpciones]=6;
            cantOpciones++;
        }
    }

    //imprimo las opciones para el envido
	if(j == 1)
	{
		//si se canto el truco y este se acepto ya no se puede cantar envido.
		//tampoco si ya se canto y se acepto o no se acepto
        if((*trucoP != 1 && *seCantoTrucoP == 0)||(*envidoP>0 && *seCantoEnvidoP== 0))
        i = i;
        else
        {
		    if(*envidoP == 0 || *envidoP == 2)
		    {
			    printf("7 para cantar envido\n");
		    	printf("8 para cantar real envido\n");
			    printf("9 para cantar falta envido\n");
			    opciones[cantOpciones]=7;
			    cantOpciones++;
			    opciones[cantOpciones]=8;
			    cantOpciones++;
			    opciones[cantOpciones]=9;
			    cantOpciones++;
		    }
		    else if(*envidoP == 3)
		    {
			    printf("9 para cantar falta envido\n");
			    opciones[cantOpciones]=9;
			    cantOpciones++;
	    	}
		    else if(*envidoP == 4)
		    {
		    	printf("8 para cantar real envido\n");
		    	printf("9 para cantar falta envido\n");
			    opciones[cantOpciones]=8;
			    cantOpciones++;
			    opciones[cantOpciones]=9;
			    cantOpciones++;
		    }
		    else if(*envidoP == 5)
		    {
			    printf("9 para cantar falta envido\n");
			    opciones[cantOpciones]=9;
			    cantOpciones++;
	    	}
        }
	}

    //imprimo el quiero y el no quiero en caso de corresponder
    if(*seCantoTrucoP == 1 || *seCantoEnvidoP == 1)
    {
        printf("10 para querer\n");
        printf("11 para no querer\n");
        opciones[cantOpciones]=10;
        cantOpciones++;
        opciones[cantOpciones]=11;
        cantOpciones++;
    }
	
    //pido por teclado la opcion
	while(flag == 0)
	{
		scanf("%d", &salida);//guardo la opcion elegida en salida
		//compruebo que la opcion sea correcta
		for(i = 0;i < cantOpciones; i++)
		{
			if(salida == opciones[i])
			flag = 1;
		}
        if ( flag == 0)
        {
			printf("La opcion ingresada no esta disponible\n");
            printf("Ingrese nuevamente la opcion deseada\n");
        }
	}
	
	//si se tiro una carta la saco de la mano, la agrego a la grilla y la copio en comparar
	//para usarla despues en la funcion quienGano
	if(salida == 1 || salida == 2 || salida == 3)
	{
		strcpy((*compararP)[0], (*manoServerP)[salida - 1]);
        ponerCartaServer(salida-1, manoServerP, grillaP);
	}
	

	//aumento el envido si fue cantado
	if(j == 1)//si estamos en primera
	{
		if(salida == 7)//si se canto envido
        {
			(*envidoP) += 2;
            (*seCantoEnvidoP)= 1;
        }
		else if(salida == 8 )//si se canto real envido
        {
			(*envidoP) += 3;
            (*seCantoEnvidoP)= 1;
        }
		else if(salida == 9)//si se canto falta envido
        {
			(*envidoP) = 30;
            (*seCantoEnvidoP)= 1;
        }
	}

    //aumento el truco si fue cantado
    if(salida == 4 || salida == 5 || salida == 6)
    {
		(*trucoP)++;
        (*seCantoTrucoP) = 1;
    }

    //si se acepto o no se quiso algo reinicio variables
    if(salida == 10 || salida == 11)
    {
        if(*seCantoEnvidoP == 1)
		{
        	(*seCantoEnvidoP) = 0;
			(*hayQueProcesarEnvidoP)=1;
		}
        else if(*seCantoTrucoP == 1)
        (*seCantoTrucoP) = 0;
    }

	return salida;
}

void recibirOpcionYActualizar(int *opcionClienteP, char manoClienteP[][3][17], char compararP[][2][17],
char grillaP[][4][90], int j, int *envidoP,int *trucoP, int *seCantoTrucoP, int *seCantoEnvidoP, int *hayQueProcesarEnvidoP)
{
    //recibo la opcion selecciona por el cliente y actualizo las variables 
    //correspondientes

	int numbytes = 0;

	if ((numbytes = recv( new_fd , opcionClienteP, sizeof(int),  0 )) == -1  )
    {
        perror("recv");
        close(new_fd);
        close(sockfd);
        exit(1);
    }

    //si tiro una carta actualizo la mano y la grilla
    if(*opcionClienteP == 1 || *opcionClienteP == 2 || *opcionClienteP == 3)
	{
		strcpy((*compararP)[1], (*manoClienteP)[(*opcionClienteP) - 1]);
        ponerCartaCliente((*opcionClienteP)-1, manoClienteP, grillaP);
	}

    //aumento el envido si fue cantado
	if(j == 1)//si estamos en primera
	{
		if(*opcionClienteP == 7)//si se canto envido
        {
			(*envidoP) += 2;
            (*seCantoEnvidoP)= 1;
        }
		else if(*opcionClienteP == 8 )//si se canto real envido
        {
			(*envidoP) += 3;
            (*seCantoEnvidoP)= 1;
        }
		else if(*opcionClienteP == 9)//si se canto falta envido
        {
			(*envidoP) = 30;
            (*seCantoEnvidoP)= 1;
        }
	}
    //aumento el truco si fue cantado
    if(*opcionClienteP == 4 || *opcionClienteP == 5 || *opcionClienteP == 6)
	{
    	(*trucoP)++;
        (*seCantoTrucoP) = 1;
    }

    //si se acepto o no algo reinicio variables
    if(*opcionClienteP == 10 || *opcionClienteP == 11)
    {
        if(*seCantoEnvidoP == 1)
		{
        	(*seCantoEnvidoP) = 0;
			(*hayQueProcesarEnvidoP)=1;
		}
        else if(*seCantoTrucoP == 1)
        (*seCantoTrucoP) = 0;
    }

}

int quienGano(CARTA *mazo, char comparar[2][17])
{
	//retorna 0 si gano el server, 1 si gano el cliente y 2 si es parda
	int salida = 0, valorServer = 0, valorCliente = 0, i = 0, j = 0;
	char numero[7], palo[7], *token;

	for(i = 0; i < 2;i++)
	{
		//separo comparar en en numero y palo
		token = strtok(comparar[i], " ");
		strcpy(numero, token);
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");
		strcpy(palo, token);

		//guardo los valores de las cartas inicializados en mezclar()
		for(j = 0; j < 40; j++)
		{
			if(strcmp(numero, mazo[j].numero) == 0)
			{
				if(strcmp(palo, mazo[j].palo) == 0)
				{
					if(i == 0)
					valorServer = mazo[j].valor;
					else if(i == 1)
					valorCliente = mazo[j].valor;
				}
			}
		}
	}

	//comparo los valores
	if(valorServer > valorCliente)
		salida = 0;
	else if(valorServer < valorCliente)
		salida = 1;
	else if(valorServer == valorCliente)
		salida = 2;


	return salida;
}

void actualizarGrilla(char grilla[][4][90], int puntosServer, int puntosCliente, char *nombreServer, char *nombreCliente)
{
	//actualiza los puntos de cada jugador en el score
	int digito = 0;

	//si es mayor a 10 hay que agregar dos digitos por separado
	if(puntosServer < 10)
		(*grilla)[2][61+strlen(nombreServer)/2]=puntosServer + 48;//+ 48 es para igualarlo en la tabla ascii
	else
	{
		digito = puntosServer % 10;//agarro el primer digito
		(*grilla)[2][61+strlen(nombreServer)/2]=digito + 48;
		digito = puntosServer / 10;//agarro el segundo digito
		(*grilla)[2][61+strlen(nombreServer)/2 + 1]=digito + 48;
	}

	if(puntosCliente < 10)
		(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2]=puntosCliente + 48;
	else
	{
		digito = puntosCliente % 10;//agarro el primer digito
		(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2]=digito + 48;
		digito = puntosCliente / 10;//agarro el segundo digito
		(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2 + 1]=digito + 48;
	}
}

void limpiarGrilla(char grilla[][4][90],int puntosServer, int puntosCliente, char *nombreServer, char *nombreCliente, int puntosMaximos)
{
	//inicializa la grilla y le agrega los puntos de los jugadores
	//de forma que este lista para jugar la proxima mano
	inicializarGrilla(grilla,nombreServer, nombreCliente, puntosMaximos);
	actualizarGrilla(grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
}

void enviarOpciones(char manoClienteP[][3][17], int j, int *envidoP,int *trucoP,
int *seCantoTrucoP, int *seCantoEnvidoP)
{
    //procesa las opciones disponibles para el cliente y se las manda
    int i = 0;
    char buf[MAXDATASIZE];

	//envio las opciones para tirar cartas
    if(*seCantoTrucoP == 0 && *seCantoEnvidoP == 0)//si no tengo que aceptar envido ni truco
    {
	    for(i = 0; i < 3;i++)
	    {
		    if(strcmp((*manoClienteP)[i], "x") != 0)//solo envio las cartas disponibles para tirar
		    {
                buf[0]=i +1 + 48;//guardo el numero de opcion en ASCII
                strcpy(&(buf[1]), " para tirar ");
                strcpy(&(buf[strlen(buf)]), (*manoClienteP)[i]);
                enviarString(buf);
		    }
	    }
    }

    //envio las opciones del truco
    if(*seCantoEnvidoP == 0)//si no se esta cantando el envido
    {
        if(*trucoP == 1)
        {
            enviarString("4 para cantar truco");
        }
        else if(*trucoP == 2)
        {
            enviarString("5 para cantar retruco");
        }
        else if(*trucoP == 3)
        {
            enviarString("6 para cantar quiero vale cuatro");
        }
    }

    //envio las opciones para el envido
	if(j == 1)//si estamos en primera
	{
		//si se canto el truco y este se acepto ya no se puede cantar envido.
		//tampoco si ya se canto y se acepto o no se acepto
        if((*trucoP != 1 && *seCantoTrucoP == 0)||(*envidoP>0 && *seCantoEnvidoP== 0))
        i = i;
        else
        {
		    if(*envidoP == 0 || *envidoP == 2)
		    {
			    enviarString("7 para cantar envido");
			    enviarString("8 para cantar real envido");
			    enviarString("9 para cantar falta envido");
		    }
		    else if(*envidoP == 3)
		    {
			    enviarString("9 para cantar falta envido");
		    }
		    else if(*envidoP == 4)
		    {
			    enviarString("8 para cantar real envido");
			    enviarString("9 para cantar falta envido");
	    	}
		    else if(*envidoP == 5)
		    {
			    enviarString("9 para cantar falta envido");
		    }
        }
	}

    //envio el quiero y el no quiero en caso de corresponder
    if(*seCantoTrucoP == 1 || *seCantoEnvidoP == 1)
    {
        enviarString("10 para querer");
        enviarString("11 para no querer");
    }

    enviarString("Fin");
}

void procesarEnvido(char manoClienteAux[3][17],char manoServerAux[3][17], int envido, int *puntosServerP,
int *puntosClienteP, int manoNumero, int puntosMaximos)
{
	//calcula quien gano el envido y suma los puntos al ganador
	int i = 0, envidoServer = 0, envidoCliente = 0,numeros[3], alternativas[]={0,0,0};//inicializo alternativas en 0
	char *palos[3];

	//calculo envido del server
	for(i = 0;i < 3; i++)
	{
		numeros[i] = atoi(strtok(manoServerAux[i], " "));//guardo los numeros de las cartas
		strtok(NULL, " ");
		palos[i]= strtok(NULL, " ");//guardo los palos de las cartas
	}

	if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)&&(numeros[1]==10 || numeros[1]==11 || numeros[1]== 12)&&
	(numeros[2]==10 || numeros[2]==11 || numeros[2]==12))//si todas las cartas de la mano son 10,11 o 12
	{
		//si al menos 2 cartas comparte palo
		if(strcmp(palos[0], palos[1])== 0 ||strcmp(palos[1], palos[2])== 0 || strcmp(palos[0], palos[2])== 0)
		envidoServer = 20;
		else
		envidoServer = 0;
		
	}
	else//si existe al menos una carta que no sea ni 10 ni 11 ni 12
	{
		//comparo los palos de dos cartas y si hay envido lo cargo en alternativa[0]
		if(strcmp(palos[0], palos[1])== 0)
		{
			//si las dos cartas son 10,11 o 12
			if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)&&(numeros[1]==10 || numeros[1]==11 || numeros[1]== 12))
			alternativas[0]=20;
			//si una de las dos cartas es un 10,11 o 12
			else if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12 || numeros[1]==10 || numeros[1]==11 || numeros[1]== 12))
			{
				alternativas[0]=20;
				if(numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)
				alternativas[0]+=numeros[1];
				else
				alternativas[0]+=numeros[0];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
			alternativas[0]=20 + numeros[0] + numeros[1];

		}
		//comparo los palos de otras dos cartas y si hay envido lo cargo en alternativa[1]
		if(strcmp(palos[1], palos[2])== 0)
		{
			//si las dos cartas son 10,11 o 12
			if((numeros[1]==10 || numeros[1]== 11 || numeros[1]==12)&&(numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			alternativas[1]=20;
			//si una de las dos cartas es un 10,11 o 12
			else if((numeros[1]==10 || numeros[1]== 11 || numeros[1]==12 || numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			{
				alternativas[1]=20;
				if(numeros[1]==10 || numeros[1]== 11 || numeros[1]==12)
				alternativas[1]+=numeros[2];
				else
				alternativas[1]+=numeros[1];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
			alternativas[1]=20 + numeros[1] + numeros[2];

		}
		//comparo los palos de otras dos cartas y si hay envido lo cargo en alternativa[2]
		if(strcmp(palos[0], palos[2])== 0)
		{
			//si las dos cartas son 10,11 o 12
			if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)&&(numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			alternativas[2]=20;
			//si una de las dos cartas es un 10,11 o 12
			else if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12 || numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			{
				alternativas[2]=20;
				if(numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)
				alternativas[2]+=numeros[2];
				else
				alternativas[2]+=numeros[0];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
			alternativas[2]=20 + numeros[0] + numeros[2];

		}
		//si no hay al menos 2 cartas con el mismo palo
		if(alternativas[0]==0 && alternativas[1]== 0 && alternativas[2]== 0)
		{
			for(i = 0;i < 3;i++)
			{
				if(numeros[i] < 10)
				alternativas[i] = numeros[i];
			}
		}

		//comparo las alternativas y me quedo con la mas alta

		for(i = 0;i < 3;i++)
		{
			if(envidoServer<alternativas[i])
			envidoServer=alternativas[i];
		}
	}

	//reinicio alternativas
	alternativas[0]=0;
	alternativas[1]=0;
	alternativas[2]=0;

	//calculo envido del cliente
	for(i = 0;i < 3; i++)
	{
		numeros[i] = atoi(strtok(manoClienteAux[i], " "));//guardo los numeros de las cartas
		strtok(NULL, " ");
		palos[i]= strtok(NULL, " ");//guardo los palos de las cartas
	}

	if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)&&(numeros[1]==10 || numeros[1]==11 || numeros[1]== 12)&&
	(numeros[2]==10 || numeros[2]==11 || numeros[2]==12))//si todas las cartas de la mano son 10,11 o 12
	{
		//si al menos 2 cartas comparte palo
		if(strcmp(palos[0], palos[1])== 0 ||strcmp(palos[1], palos[2])== 0 || strcmp(palos[0], palos[2])== 0)
		envidoCliente = 20;
		else
		envidoCliente = 0;
		
	}
	else//si existe al menos una carta que no sea ni 10 ni 11 ni 12
	{
		//comparo los palos de dos cartas y si hay envido lo cargo en alternativa[0]
		if(strcmp(palos[0], palos[1])== 0)
		{
			//si las dos cartas son 10,11 o 12
			if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)&&(numeros[1]==10 || numeros[1]==11 || numeros[1]== 12))
			alternativas[0]=20;
			//si una de las dos cartas es un 10,11 o 12
			else if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12 || numeros[1]==10 || numeros[1]==11 || numeros[1]== 12))
			{
				alternativas[0]=20;
				if(numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)
				alternativas[0]+=numeros[1];
				else
				alternativas[0]+=numeros[0];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
			alternativas[0]=20 + numeros[0] + numeros[1];

		}
		//comparo los palos de otras dos cartas y si hay envido lo cargo en alternativa[1]
		if(strcmp(palos[1], palos[2])== 0)
		{
			//si las dos cartas son 10,11 o 12
			if((numeros[1]==10 || numeros[1]== 11 || numeros[1]==12)&&(numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			alternativas[1]=20;
			//si una de las dos cartas es un 10,11 o 12
			else if((numeros[1]==10 || numeros[1]== 11 || numeros[1]==12 || numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			{
				alternativas[1]=20;
				if(numeros[1]==10 || numeros[1]== 11 || numeros[1]==12)
				alternativas[1]+=numeros[2];
				else
				alternativas[1]+=numeros[1];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
			alternativas[1]=20 + numeros[1] + numeros[2];

		}
		//comparo los palos de otras dos cartas y si hay envido lo cargo en alternativa[2]
		if(strcmp(palos[0], palos[2])== 0)
		{
			//si las dos cartas son 10,11 o 12
			if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)&&(numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			alternativas[2]=20;
			//si una de las dos cartas es un 10,11 o 12
			else if((numeros[0]==10 || numeros[0]== 11 || numeros[0]==12 || numeros[2]==10 || numeros[2]==11 || numeros[2]== 12))
			{
				alternativas[2]=20;
				if(numeros[0]==10 || numeros[0]== 11 || numeros[0]==12)
				alternativas[2]+=numeros[2];
				else
				alternativas[2]+=numeros[0];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
			alternativas[2]=20 + numeros[0] + numeros[2];

		}
		//si no hay al menos 2 cartas con el mismo palo
		if(alternativas[0]==0 && alternativas[1]== 0 && alternativas[2]== 0)
		{
			for(i = 0;i < 3;i++)
			{
				if(numeros[i] < 10)
				alternativas[i] = numeros[i];
			}
		}
		//comparo las alternativas y me quedo con la mas alta

		for(i = 0;i < 3;i++)
		{
			if(envidoCliente<alternativas[i])
			envidoCliente=alternativas[i];
		}
	}

	//prueba
	
	printf("envidoServer: %d\n", envidoServer);
	printf("envidoCliente: %d\n", envidoCliente);
	scanf("%d", &i);

	//veo quien tiene mas envido
	if(envidoServer>envidoCliente)//si gano el server
	{
		if(envido == 30)//si es falta envido
		(*puntosServerP)+=(puntosMaximos - (*puntosClienteP));
		else
		(*puntosServerP)+=envido;
	}
	else if(envidoCliente>envidoServer)//si gano el cliente
	{
		if(envido== 30)
			(*puntosClienteP)+=(puntosMaximos- (*puntosServerP));
		else
			(*puntosClienteP)+=envido;
	}	
	else if(envidoCliente == envidoServer)//si empataron
	{
		//en caso de empate gana el que es mano
		if((manoNumero%2)== 1)//si es mano el server
		{
			if(envido == 30)
			(*puntosServerP)+=(puntosMaximos - (*puntosClienteP));
			else
			(*puntosServerP)+=envido;
		}
		else//si es mano el cliente
		{
			if(envido== 30)
			(*puntosClienteP)+=(puntosMaximos- (*puntosServerP));
			else
			(*puntosClienteP)+=envido;
		}
	}


}