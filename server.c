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

}CARTA;
//prototipos------------------------------------------------
void inicializarMazo (CARTA * mazo, char *numero[], char *palo[]);

void mezclar(CARTA *mazo);

void repartir(CARTA *mazo, char manoServer[][3][17], char manoCliente[][3][17]);

void enviarCartas(char manoCliente[][3][17]);

void enviarString(char *buf);

void recibirString(char *buf);

void inicializarGrilla(char grilla[][4][80], char *nombreServer, char *nombreCliente);

void imprimirGrilla(char grilla[4][80]);

void imprimirMano(char manoServer[3][17]);


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

void inicializarGrilla(char grilla[][4][80], char *nombreServer, char *nombreCliente)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++)//inicializo todo con espacios
	{
		for(j = 0; j < 80; j++)
			(*grilla)[i][j]= ' ';
	}
	//hago la primera linea
	strcpy((*grilla)[0],"Primera          Segunda          Tercera          ");
	strcpy(&((*grilla)[0][strlen((*grilla)[0])]), nombreServer);
	strcpy(&((*grilla)[0][strlen((*grilla)[0])]), "     ");
	strcpy(&((*grilla)[0][strlen((*grilla)[0])]), nombreCliente);
	(*grilla)[0][strlen((*grilla)[0])]=' ';//reemplazo el null por un espacio

	//hago la segunda linea
	(*grilla)[1][3]='x';
	(*grilla)[1][20]='x';
	(*grilla)[1][37]='x';
	(*grilla)[1][51+strlen(nombreServer)/2]='0';
	(*grilla)[1][51+strlen(nombreServer)+5+strlen(nombreCliente)/2]='0';

	//la tercera linea son todos espacios

	//hago la cuarta linea
	(*grilla)[3][3]='x';
	(*grilla)[3][20]='x';
	(*grilla)[3][37]='x';

}

void imprimirGrilla(char grilla[4][80])
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++)//inicializo todo con espacios
	{
		for(j = 0; j < 80; j++)
			printf("%c", grilla[i][j]);
	}
}

void imprimirMano(char manoServer[3][17])
{
	printf("Su mano: %s\t%s\t%s\n", manoServer[0], manoServer[1], manoServer[2]);
}

int main( void )
{
	int numbytes, manonumero = 1;	// Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	int yes = 1, flag = 0, puntosServer = 0, puntosCliente = 0, puntosMaximos = 0, i = 0, aux = 0;
    char buf[MAXDATASIZE], nombreServer[MAXDATASIZE], nombreCliente[MAXDATASIZE];
    CARTA mazo[40];//el mazo tiene 40 cartas
    char *numero[]={"Uno", "Dos", "Tres", "Cuatro", "Cinco", "Seis", "Siete", "Diez", "Once", "Doce"};
   	char *palo[]= {"Espada", "Basto", "Oro", "Copa"};
   	char *opciones[]={"Tirar carta", "Envido", "Real envido", "Falta envido", "Truco", "Retruco", "Quiero vale cuatro", "Quiero", "No quiero"};
   	char grilla[4][80];
   	char manoServer[3][17];
   	char manoCliente[3][17];
	
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
   	inicializarGrilla(&grilla, nombreServer, nombreCliente);
/*
	while(flag == 0)//flag = 1 significa que termino el juego
	{
		mezclar(mazo);
   		repartir(mazo, &manoServer, &manoCliente);
   		enviarCartas(&manoCliente);
		if(manonumero%2 == 1)//es mano el server
		{
			
		}
		else//es mano el cliente
		{
			
		}

		if(puntosServer == puntosMaximos)
		{
			flag = 1;
			printf("%s ha ganado el partido", nombreServer);
			enviarString("Server ha ganado la partida");
		}
		else if(puntosCliente == puntosMaximos)
		{
			flag = 1;
			printf("%s ha ganado el partido", nombreCliente);
			enviarString("Cliente ha ganado la partida");
		}

		manonumero++;

	}
*/
   	mezclar(mazo);
   	repartir(mazo, &manoServer, &manoCliente);
   	enviarCartas(&manoCliente);

    printf("%s\n", manoServer[0]);
    printf("%s\n", manoServer[1]);
    printf("%s\n", manoServer[2]);
    printf("---------------------------\n");
    printf("%s\n", manoCliente[0]);
    printf("%s\n", manoCliente[1]);
    printf("%s\n", manoCliente[2]);

    sleep(3);

    system("clear");
    imprimirGrilla(grilla);
    imprimirMano(manoServer);

    close(sockfd);
    close(new_fd);

	return 0;
}
