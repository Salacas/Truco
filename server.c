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

#define MYPORT 	3490		// Puerto al que conectarán los usuarios
#define BACKLOG 0			// Cuántas conexiones pendientes se mantienen en cola
#define MAXDATASIZE 100     // máximo número de bytes que se pueden leer de una vez

int sockfd, new_fd;

typedef struct carta
{
	char numero[7];//el numero mas largo es el cuatro(seis letras) + null
	char palo[7];//el palo mas largo es la espada(seis letras) + null
	int valor;

}CARTA;
//prototipos------------------------------------------------
void inicializarMazo (CARTA * mazo, char *numero[], char *palo[]);

void mezclar(CARTA *mazo);

void repartir(CARTA *mazo, char manoServer[][3][17], char manoCliente[][3][17]);

void enviarCartas(char manoCliente[][3][17]);

void enviarString(char *buf);

void recibirString(char *buf);

void inicializarGrilla(char grilla[][4][90], char *nombreServer, char *nombreCliente, int puntosMaximos);

void imprimirGrilla(char grilla[4][90]);

void imprimirMano(char manoServer[3][17]);

void ponerCartaServer(int numeroCarta, char manoserver[][3][17], char grilla[][4][90]);

void ponerCartaCliente(int numeroCarta, char manoCliente[][3][17], char grilla[][4][90]);

void enviarGrilla(char grilla[4][90]);

void enviarFlag(int flag);

int imprimirOpciones(char manoServer[3][17]);

void recibirOpcion(int *opcionClienteP);

int quienGano(CARTA *mazo, char comparar[2][17]);

void actualizarGrilla(char grilla[][4][90], int puntosServer, int puntosCliente, char *nombreServer, char *nombreCliente);


//funciones----------------------------------------------------
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
			if(strcmp(mazo[i].numero,"Cuatro") == 0)
				mazo[i].valor = 0;
			if(strcmp(mazo[i].numero,"Cinco") == 0)
				mazo[i].valor = 1;
			if(strcmp(mazo[i].numero,"Seis") == 0)
				mazo[i].valor = 2;
			if(strcmp(mazo[i].numero,"Siete") == 0)
			{
				if(strcmp(mazo[i].palo, "Basto") == 0 || strcmp(mazo[i].palo, "Copa") == 0)
					mazo[i].valor = 3;
				else if(strcmp(mazo[i].palo, "Oro") == 0)
					mazo[i].valor = 10;
				else if(strcmp(mazo[i].palo, "Espada") == 0)
					mazo[i].valor = 11;
			}
			if(strcmp(mazo[i].numero,"Diez") == 0)
				mazo[i].valor = 4;
			if(strcmp(mazo[i].numero,"Once") == 0)
				mazo[i].valor = 5;
			if(strcmp(mazo[i].numero,"Doce") == 0)
				mazo[i].valor = 6;
			if(strcmp(mazo[i].numero,"Uno") == 0)
			{
				if(strcmp(mazo[i].palo, "Oro") == 0 || strcmp(mazo[i].palo, "Copa") == 0)
					mazo[i].valor = 7;
				else if(strcmp(mazo[i].palo, "Basto") == 0)
					mazo[i].valor = 12;
				else if(strcmp(mazo[i].palo, "Espada") == 0)
					mazo[i].valor = 13;
			}
			if(strcmp(mazo[i].numero,"Dos") == 0)
				mazo[i].valor = 8;
			if(strcmp(mazo[i].numero,"Tres") == 0)
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

   	if ( ( numbytes = send(new_fd, buf, strlen(buf), 0) ) == -1 )//envio la mano del cliente
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
	int i = 0, j = 0;

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 90; j++)
			printf("%c", grilla[i][j]);
	}
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

	printf("\n");
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

int imprimirOpciones(char manoServer[3][17])
{
	//imprime las opciones disponibles y guarda la opcion elegida por el server
	int i = 0, salida = 0, flag = 0;

	fflush(stdin);

	printf("Presione:\n");
	for(i = 0; i < 3;i++)
	{
		if(strcmp(manoServer[i], "x") != 0)
			printf("%d para tirar %s\n", i+1, manoServer[i]);
		
	}
	while(flag == 0)
	{
		scanf("%d", &salida);//guardo la opcion elegida en salida
		//compruebo que la opcion sea correcta
		flag = 1;
		if(salida >0 && salida <4)
		{
			for(i = 0; i < 3;i++)
			{
				if(strcmp(manoServer[i], "x") == 0)
				{
					if(salida == i+1)
					{
						flag = 0;
						printf("Seleccione una opcion valida\n");
					}
				}
			}
		
		}
		else
		{
			flag =0;
			printf("Seleccione una opcion valida\n");
		}
	}

	salida--;//acomodo el indice

	return salida;
}

void recibirOpcion(int *opcionClienteP)
{
	int numbytes = 0;

	if ((numbytes = recv( new_fd , opcionClienteP, sizeof(int),  0 )) == -1  )
    {
        perror("recv");
        close(new_fd);
        close(sockfd);
        exit(1);
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

	if(puntosServer < 10)
		(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2]=puntosServer + 48;
	else
	{
		digito = puntosServer % 10;//agarro el primer digito
		(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2]=digito + 48;
		digito = puntosServer / 10;//agarro el segundo digito
		(*grilla)[2][61+strlen(nombreServer)+5+strlen(nombreCliente)/2 + 1]=digito + 48;
	}


}

int main( void )
{
	int numbytes, manonumero = 1;	// Escuchar sobre sock_fd, conexion del cliente sobre new_fd
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	int yes = 1, flag = 0, puntosServer = 0, puntosCliente = 0, puntosMaximos = 0, i = 0, aux = 0, opcionCliente = 0, opcionServer = 0,
	primera = 0, segunda = 0, tercera = 0;
    char buf[MAXDATASIZE], nombreServer[MAXDATASIZE], nombreCliente[MAXDATASIZE];
    CARTA mazo[40];//el mazo tiene 40 cartas
    char *numero[]={"Uno", "Dos", "Tres", "Cuatro", "Cinco", "Seis", "Siete", "Diez", "Once", "Doce"};
   	char *palo[]= {"Espada", "Basto", "Oro", "Copa"};
   	char *opciones[]={"Tirar carta", "Envido", "Real envido", "Falta envido", "Truco", "Retruco", "Quiero vale cuatro", "Quiero", "No quiero"};
   	char grilla[4][90];//grilla para imprimir por pantalla
   	char manoServer[3][17];//guardo la mano del cliente
   	char manoCliente[3][17];//guardo la mano del server
   	char comparar[2][17];//guardo las cartas jugadas
	
	if ( ( sockfd = socket( AF_INET , SOCK_STREAM , 0 ) ) == -1) 
	{
		perror("socket");
		exit(1);
	}
	
	if ( setsockopt( sockfd , SOL_SOCKET , SO_REUSEADDR , &yes , sizeof(int) ) == -1) 
	{
		perror("setsockopt");
		close(sockfd);
		exit(1);
	}
	
	my_addr.sin_family = AF_INET; 
	my_addr.sin_port = htons( MYPORT ); 
	my_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	memset(&(my_addr.sin_zero), '\0', 8);

	if ( bind( sockfd , (struct sockaddr *)&my_addr, sizeof( struct sockaddr ) ) == -1 ) 
	{
		perror("bind");
		close(sockfd);
		exit(1);
	}

	if ( listen( sockfd , BACKLOG ) == -1 ) 
	{
		perror("listen");
		close(sockfd);
		exit(1);
	}

	printf("Esperando conexion...\n");
	
	sin_size = sizeof( struct sockaddr_in );

	if ( ( new_fd = accept( sockfd , (struct sockaddr *)&their_addr, &sin_size)) == -1)
	{ 
		perror("accept");
		close(sockfd);
		exit(1);
	}

	//inicio de conexion-------------------------------------------------------------------
	
	printf("Conexion desde %s\n", inet_ntoa(their_addr.sin_addr));
	printf("Ingrese su nombre: ");
	scanf("%s", nombreServer);

	while(flag == 0)//para cubrir errores del usuario
	{
		printf("A cuantos puntos quiere jugar?\n");
		printf("Ingrese ""15"" o ""30"":");
		scanf("%d", &puntosMaximos);

		if(puntosMaximos == 15 || puntosMaximos == 30)
			flag = 1;
		else
			printf("La opcion ingresada es incorrecta\n");
	}
	flag = 0;//reinicio la flag

	enviarString(nombreServer);//envio el nombre del jugador server
	recibirString(nombreCliente);//recibo el nombre del jugador cliente

   	inicializarMazo(mazo, numero, palo);
   	inicializarGrilla(&grilla, nombreServer, nombreCliente, puntosMaximos);

	while(flag != 3)//flag = 3 significa que termino el juego
	{
		system("clear");//limpio la pantalla
		mezclar(mazo);
   		repartir(mazo, &manoServer, &manoCliente);
   		flag = 2;
   		enviarFlag(flag);
   		enviarCartas(&manoCliente);

   		primera = -1;
   		segunda = -1;
   		tercera = -1;
  		i = 1;//indica si estamos en primera, segunda o tercera

		if(manonumero%2 == 1)//es mano el server
		{
			while(primera == -1 || (primera == 0 && segunda == -1)|| (primera == 2 && segunda == -1)||
			(primera == 2 && segunda == 2 && tercera == -1))//tira carta el server
			{
				flag = 0;//0 significa que es el turno del server
				enviarFlag(flag);
				enviarGrilla(grilla);
				system("clear");
   				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				opcionServer = imprimirOpciones(manoServer);//el server selecciona la carta a tirar
				strcpy(comparar[0], manoServer[opcionServer]);//guardo esa carta
				ponerCartaServer(opcionServer, &manoServer, &grilla);
				imprimirGrilla(grilla);
				imprimirMano(manoServer);

				flag = 1;//1 significa que es el turno del cliente
				enviarFlag(flag);
				enviarGrilla(grilla);
				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				recibirOpcion(&opcionCliente);//recibo la carta a tirar del cliente
				strcpy(comparar[1], manoCliente[opcionCliente]);//guardo esa carta
				ponerCartaCliente(opcionCliente, &manoCliente, &grilla);
				
				if(i == 1)
					primera = quienGano(mazo, comparar);
				else if(i == 2)
					segunda = quienGano(mazo, comparar);
				else if(i == 3)
					tercera = quienGano(mazo, comparar);
				i++;
			}
			while((primera == 1 && segunda == -1)||(primera == 0 && segunda == 1 && tercera == -1))//tira carta el cliente
			{
				flag = 1;//1 significa que es el turno del cliente
				enviarFlag(flag);
				enviarGrilla(grilla);
				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				recibirOpcion(&opcionCliente);//recibo la carta a tirar del cliente
				strcpy(comparar[1], manoCliente[opcionCliente]);//guardo esa carta
				ponerCartaCliente(opcionCliente, &manoCliente, &grilla);

				flag = 0;//0 significa que es el turno del server
				enviarFlag(flag);
				enviarGrilla(grilla);
				system("clear");
   				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				opcionServer = imprimirOpciones(manoServer);//el server selecciona la carta a tirar
				strcpy(comparar[0], manoServer[opcionServer]);//guardo esa carta
				ponerCartaServer(opcionServer, &manoServer, &grilla);
				if(i == 1)
					primera = quienGano(mazo, comparar);
				else if(i == 2)
					segunda = quienGano(mazo, comparar);
				else if(i == 3)
					tercera = quienGano(mazo, comparar);
				i++;

			}
			while(primera == 1 && segunda == 0 && tercera == -1)
			{
				flag = 0;//0 significa que es el turno del server
				enviarFlag(flag);
				enviarGrilla(grilla);
				system("clear");
   				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				opcionServer = imprimirOpciones(manoServer);//el server selecciona la carta a tirar
				strcpy(comparar[0], manoServer[opcionServer]);//guardo esa carta
				ponerCartaServer(opcionServer, &manoServer, &grilla);
				imprimirGrilla(grilla);
				imprimirMano(manoServer);

				flag = 1;//1 significa que es el turno del cliente
				enviarFlag(flag);
				enviarGrilla(grilla);
				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				recibirOpcion(&opcionCliente);//recibo la carta a tirar del cliente
				strcpy(comparar[1], manoCliente[opcionCliente]);//guardo esa carta
				ponerCartaCliente(opcionCliente, &manoCliente, &grilla);
				
				if(i == 1)
					primera = quienGano(mazo, comparar);
				else if(i == 2)
					segunda = quienGano(mazo, comparar);
				else if(i == 3)
					tercera = quienGano(mazo, comparar);
				i++;
			}
				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);


		}
		else//es mano el cliente
		{
			flag = 1;//1 significa que es el turno del cliente
			enviarFlag(flag);
		}

		//subo puntos
		if((primera == 0 && segunda == 0)||(primera == 0 && segunda == 2)||(primera == 0 && segunda == 1 && tercera == 0)||
		(primera == 0 && segunda == 1 && tercera == 2)||(primera == 1 && segunda == 0 && tercera == 0)||(primera == 2 && segunda == 0)||
		(primera == 2 && segunda == 2 && tercera == 0)||(primera = 2 && segunda == 2 && tercera == 2 && (manonumero%2) == 1))//gana el server
		{
			puntosServer++;
			actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
			printf("%s gano la mano\n", nombreServer);
			sleep(2);
		}
		else //gana el cliente
		{
			puntosCliente++;
			actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
			printf("%s gano la mano\n", nombreCliente);
			sleep(2);
		}

		//veo si alguien gano
		if(puntosServer == puntosMaximos)
		{
			flag = 3;
			enviarFlag(flag);
			printf("%s ha ganado el partido", nombreServer);
			enviarString("Server ha ganado la partida");
		}
		else if(puntosCliente == puntosMaximos)
		{
			flag = 3;
			enviarFlag(flag);
			printf("%s ha ganado el partido", nombreCliente);
			enviarString("Cliente ha ganado la partida");
		}

		//PRUEBA----------------------------------------------------------------------------------------------------
		flag = 3;
		enviarFlag(flag);
		enviarGrilla(grilla);
		//----------------------------------------------------------------------------------------------------
		manonumero++;

	}

/*
   	enviarFlag(flag);
   	mezclar(mazo);
   	repartir(mazo, &manoServer, &manoCliente);
   	enviarCartas(&manoCliente);
   	enviarGrilla(grilla);

    imprimirGrilla(grilla);
    imprimirMano(manoServer);
	printf("Seleccione: 1-Tirar %s\n2-Tirar %s\n3-Tirar %s\n", manoServer[0], manoServer[1], manoServer[2]);
    
 	
    ponerCartaServer(0, &manoServer, &grilla);
    imprimirGrilla(grilla);
    imprimirMano(manoServer);
    enviarGrilla(grilla);
    flag = 1;
    enviarFlag(flag);
*/
    close(sockfd);
    close(new_fd);

	return 0;
}
