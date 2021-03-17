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

int main( void )
{
	int manoNumero = 1;	// Escuchar sobre sock_fd, conexion del cliente sobre new_fd
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	int yes = 1, flag = 0, puntosServer = 0, puntosCliente = 0, puntosMaximos = 0, i = 0, opcionCliente = 0,
	primera = 0, segunda = 0, tercera = 0, envido = 0, truco = 0, seCantoTruco = 0, seCantoEnvido = 0, hayQueProcesarEnvido = 0;
    char  nombreServer[MAXDATASIZE], nombreCliente[MAXDATASIZE];
    CARTA mazo[40];//el mazo tiene 40 cartas
    char *numero[]={"1", "2", "3", "4", "5", "6", "7", "10", "11", "12"};
   	char *palo[]= {"Espada", "Basto", "Oro", "Copa"};
   	char grilla[4][90];//grilla para imprimir por pantalla
   	char manoServer[3][17];//guardo la mano del cliente
   	char manoCliente[3][17];//guardo la mano del server
   	char comparar[2][17];//guardo las cartas jugadas
	char manoServerAux[3][17];//utilizada en algunas funciones
	char manoClienteAux[3][17];//utilizada en algunas funciones
	
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

		for(i = 0;i < 3;i++)//copio las manos a variables auxiliares para no perder las cartas
		{
			strcpy(manoServerAux[i], manoServer[i]);
			strcpy(manoClienteAux[i], manoCliente[i]);
		}
		

   		flag = 2;
   		enviarFlag(flag);
   		enviarCartas(&manoCliente);
		//si no es la primer mano espero para que el server vea como termino la mano
		if(manoNumero != 1)
		sleep(4);

		//reinicio variables;
		//primera, segunda y tercera indican el estado de la mano
		//= -1 significa que no se jugo todavia
		//= 0 indica que gano el server
		//= 1 indica que gano el cliente
		//= 2 indica que es parda
   		primera = -1;
   		segunda = -1;
   		tercera = -1;
  		i = 1;//indica si estamos en primera, segunda o tercera
		envido = 0;//indica cuanto envido hay cantado
		truco = 1;//indica cuanto vale el truco
		seCantoEnvido = 0;//indica si hay que aceptar el envido
		seCantoTruco = 0;//indica si hay que aceptar el truco
		hayQueProcesarEnvido = 0;
		comparar[0][0]='t';//para algunos while
		comparar[1][0]='t';//para algunos while

		if(manoNumero%2 == 1)//es mano el server
		{
			while(primera == -1 || (primera == 0 && segunda == -1)|| (primera == 2 && segunda == -1)||
			(primera == 2 && segunda == 2 && tercera == -1))//tira carta el server primero
			{
				do
				{		
					flag = 0;//0 significa que es el turno del server
					enviarFlag(flag);
					enviarGrilla(grilla);
					system("clear");
   					imprimirGrilla(grilla);
   					imprimirMano(manoServer);

					imprimirOpciones(&manoServer, &comparar,&grilla,i ,&envido,
					&truco, &seCantoTruco, &seCantoEnvido, &hayQueProcesarEnvido);
					imprimirGrilla(grilla);
					imprimirMano(manoServer);
					
					if(hayQueProcesarEnvido == 1)//si se acepto el envido
					{
						procesarEnvido(manoClienteAux, manoServerAux, envido, &puntosServer, &puntosCliente, manoNumero, puntosMaximos);
						actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
						hayQueProcesarEnvido = 0;
					}
					
					flag = 1;//1 significa que es el turno del cliente
					enviarFlag(flag);
					enviarGrilla(grilla);
					enviarOpciones(&manoCliente, i, &envido, &truco, &seCantoTruco, &seCantoEnvido);
					system("clear");
					imprimirGrilla(grilla);
   					imprimirMano(manoServer);
					printf("Turno de %s\n", nombreCliente);
					recibirOpcionYActualizar(&opcionCliente, &manoCliente, &comparar, &grilla, i, &envido, &truco,
					&seCantoTruco, &seCantoEnvido, &hayQueProcesarEnvido);//recibo la carta a tirar del cliente
					
					if(hayQueProcesarEnvido == 1)//si se acepto el envido
					{
						procesarEnvido(manoClienteAux, manoServerAux, envido, &puntosServer, &puntosCliente, manoNumero, puntosMaximos);
						actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
						hayQueProcesarEnvido = 0;
					}
					
				}
				while(comparar[0][0]=='t'||comparar[1][0]=='t'||hayQueProcesarEnvido== 1);//fijarse si comparar tiene dos cartas y hayQueProcesarEnvido == 1

				if(i == 1)
					primera = quienGano(mazo, comparar);
				else if(i == 2)
					segunda = quienGano(mazo, comparar);
				else if(i == 3)
					tercera = quienGano(mazo, comparar);
				i++;
			}
			while((primera == 1 && segunda == -1)||(primera == 0 && segunda == 1 && tercera == -1))//tira carta el cliente primero
			{
				do
				{
					flag = 1;//1 significa que es el turno del cliente
					enviarFlag(flag);
					enviarGrilla(grilla);
					enviarOpciones(&manoCliente, i, &envido, &truco, &seCantoTruco, &seCantoEnvido);
					system("clear");
					imprimirGrilla(grilla);
   					imprimirMano(manoServer);
					printf("Turno de %s\n", nombreCliente);
					recibirOpcionYActualizar(&opcionCliente, &manoCliente, &comparar, &grilla, i, &envido, &truco,
					&seCantoTruco, &seCantoEnvido, &hayQueProcesarEnvido);

					flag = 0;//0 significa que es el turno del server
					enviarFlag(flag);
					enviarGrilla(grilla);
					system("clear");
   					imprimirGrilla(grilla);
   					imprimirMano(manoServer);
					imprimirOpciones(&manoServer, &comparar,&grilla,i ,&envido,
					&truco, &seCantoTruco, &seCantoEnvido, &hayQueProcesarEnvido);
				}
				while(comparar[0][0]=='t'||comparar[1][0]=='t');

				if(i == 1)
					primera = quienGano(mazo, comparar);
				else if(i == 2)
					segunda = quienGano(mazo, comparar);
				else if(i == 3)
					tercera = quienGano(mazo, comparar);
				i++;

			}
			while(primera == 1 && segunda == 0 && tercera == -1)//tira carta el server primero
			{
				do
				{
					flag = 0;//0 significa que es el turno del server
					enviarFlag(flag);
					enviarGrilla(grilla);
					system("clear");
   					imprimirGrilla(grilla);
   					imprimirMano(manoServer);
					imprimirOpciones(&manoServer, &comparar,&grilla,i ,&envido,
					&truco, &seCantoTruco, &seCantoEnvido, &hayQueProcesarEnvido);
					imprimirGrilla(grilla);
					imprimirMano(manoServer);

					flag = 1;//1 significa que es el turno del cliente
					enviarFlag(flag);
					enviarGrilla(grilla);
					enviarOpciones(&manoCliente, i, &envido, &truco, &seCantoTruco, &seCantoEnvido);
					system("clear");
					imprimirGrilla(grilla);
   					imprimirMano(manoServer);
					printf("Turno de %s\n", nombreCliente);
					recibirOpcionYActualizar(&opcionCliente, &manoCliente, &comparar, &grilla, i, &envido, &truco,
					&seCantoTruco, &seCantoEnvido, &hayQueProcesarEnvido);
				}
				while(comparar[0][0]=='t'||comparar[1][0]=='t');
				
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
		/*else//es mano el cliente
		{
			while(primera == -1 || (primera == 1 && segunda == -1)|| (primera == 2 && segunda == -1)||
			(primera == 2 && segunda == 2 && tercera == -1))//tira carta el cliente primero
			{
				flag = 1;//1 significa que es el turno del cliente
				enviarFlag(flag);
				enviarGrilla(grilla);

				if(i == 1)//si no esta en la primera envia el estado 
				enviarString(" ");
				else
				enviarEstado(opcionServer, manoServerAux, nombreServer);

				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				printf("Turno de %s\n", nombreCliente);
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
			while((primera == 0 && segunda == -1)||(primera == 1 && segunda == 0 && tercera == -1))//tira carta el server primero
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
				enviarEstado(opcionServer, manoServerAux, nombreServer);
				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				printf("Turno de %s\n", nombreCliente);
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
			while(primera == 0 && segunda == 1 && tercera == -1)//tira carta el cliente primero
			{
				flag = 1;//1 significa que es el turno del cliente
				enviarFlag(flag);
				enviarGrilla(grilla);
				enviarEstado(opcionServer, manoServerAux, nombreServer);
				system("clear");
				imprimirGrilla(grilla);
   				imprimirMano(manoServer);
				printf("Turno de %s\n", nombreCliente);
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
			system("clear");
			imprimirGrilla(grilla);
   			imprimirMano(manoServer);
		}
		*/
		//subo puntos
		if((primera == 0 && segunda == 0)||(primera == 0 && segunda == 2)||(primera == 0 && segunda == 1 && tercera == 0)||
		(primera == 0 && segunda == 1 && tercera == 2)||(primera == 1 && segunda == 0 && tercera == 0)||(primera == 2 && segunda == 0)||
		(primera == 2 && segunda == 2 && tercera == 0)||(primera = 2 && segunda == 2 && tercera == 2 && (manoNumero%2) == 1))//gana el server
		{
			puntosServer += truco;
			actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
			printf("%s gano la mano\n", nombreServer);
		}
		else //gana el cliente
		{
			puntosCliente += truco;
			actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
			printf("%s gano la mano\n", nombreCliente);
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

		system("clear");
		imprimirGrilla(grilla);
		limpiarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente, puntosMaximos);
		manoNumero++;

	}

    close(sockfd);
    close(new_fd);

	return 0;
}
