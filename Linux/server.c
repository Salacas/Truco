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
#include <stdio_ext.h>
#include "server.h"

int main( void )
{
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	int manoNumero = 1, yes = 1, flag = 0, puntosServer = 0, puntosCliente = 0, puntosMaximos = 0, i = 0, opcionCliente = -1,
	primera = 0, segunda = 0, tercera = 0, envido = 0, truco = 0, seCantoTruco = 0, seCantoEnvido = 0, hayQueProcesarEnvido = 0,
	flag1 = 0, quienTieneElQuiero = -1, opcionServer = -1, auxFlag = -1;
    char  nombreServer[12], nombreCliente[12], buf[MAXDATASIZE];
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
	clean_stdin();
    fgets(nombreServer, 12, stdin);
    nombreServer[strcspn(nombreServer, "\n")] = '\0';//le saco el \n

	while(flag == 0)//para cubrir errores del usuario
	{
		printf("A cuantos puntos quiere jugar?\n");
		printf("Ingrese ""15"" o ""30"": ");
		clean_stdin();
		fgets(buf, 4, stdin);
		puntosMaximos = atoi(buf);
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
		auxFlag = flag;//para saber quien fue el ultimo en jugar
   		flag = 2;
   		enviarFlag(flag);

		//si no es la primer mano espero para que el server vea como termino la mano
		//ademas envio la grilla para que el cliente vea como termino la mano
		if (manoNumero != 1)
		{
			enviarGrilla(grilla);
			if(auxFlag == 0)//si el server fue el ultimo en tirar
			enviarEstado(manoServerAux, &opcionServer, nombreServer);
			else if(auxFlag == 1)//si fue el cliente
			enviarString("Nada");
			limpiarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente, puntosMaximos);
			sleep(4);
		}
		//mezclo y reparto las nuevas cartas
		mezclar(mazo);
   		repartir(mazo, &manoServer, &manoCliente);

		for(i = 0;i < 3;i++)//copio las manos a variables auxiliares para no perder las cartas
		{
			strcpy(manoServerAux[i], manoServer[i]);
			strcpy(manoClienteAux[i], manoCliente[i]);
		}
		enviarCartas(&manoCliente);
		system("clear");//limpio la pantalla
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
		opcionCliente = -1;//utilizada en imprimirEstado
		opcionServer = -1;//utilizada en enviarEstado
		hayQueProcesarEnvido = 0;
		comparar[0][0]='t';//para algunos while
		comparar[1][0]='t';//para algunos while
		flag1 = 0;//indica que no se acepto el truco, alguno se fue al mazo o alguien gano el partido con el envido
		quienTieneElQuiero = -1;//-1 significa que nadie lo tiene, 0 que lo tiene el server, 1 que lo tiene el cliente

		if(manoNumero%2 == 1)//ES MANO EL SERVER
		{
			while((primera == -1 || (primera == 0 && segunda == -1)|| (primera == 2 && segunda == -1)||
			(primera == 2 && segunda == 2 && tercera == -1))&& flag1 == 0)//tira carta el server primero
			{
				do
				{	
					//si no se quiso el envido0
					if(envido > 0 && envido < 100 && seCantoEnvido == 0)
					{//es para evitar bugs al no aceptar el envido
						envido = 100;//le pongo 100 para no pasar de vuelta por este if
						continue;
					}
					else if(truco>1 && seCantoTruco == 0 && comparar[0][0]!='t');
					else
					{
						turnoDelServer(&grilla, manoServerAux,manoClienteAux,&opcionServer, &opcionCliente,nombreServer,
						nombreCliente, &manoServer,&comparar, i, &envido, &truco, &seCantoTruco,&seCantoEnvido, 
						&hayQueProcesarEnvido, &puntosCliente, &puntosServer, &flag, &flag1, &quienTieneElQuiero, manoNumero );
					}
					if(hayQueProcesarEnvido == 1)//si se acepto el envido
					{
						procesarEnvido(manoClienteAux, manoServerAux, envido, &puntosServer, &puntosCliente,
						manoNumero, puntosMaximos, &grilla, manoServer, nombreServer,nombreCliente, &opcionServer, &opcionCliente);
						if(puntosServer == puntosMaximos || puntosCliente == puntosMaximos)flag1=1;//si se gano el partido con el envido
						hayQueProcesarEnvido = 0;
					}
					if(envido > 0 && envido < 100 && seCantoEnvido == 0 && comparar[0][0]=='t' )
					{
						envido = 100;
						continue;//salteo el turno del cliente
					}

					if(truco>1 && seCantoTruco == 0 && comparar[0][0]=='t')continue;
					if(flag1 == 1)//si no se acepto el truco, alguien se fue al mazo o alguien gano el partido con el envido
					continue;

					turnoDelCliente(&grilla, manoServerAux, manoClienteAux, &opcionServer, &opcionCliente, nombreServer,
 					nombreCliente, &manoCliente,i, &envido, &truco, &seCantoTruco, &seCantoEnvido,
 					&quienTieneElQuiero, manoNumero, &comparar, manoServer,
 					&hayQueProcesarEnvido, &puntosServer, puntosCliente, &flag,&flag1);

					if(hayQueProcesarEnvido == 1)
					{
						procesarEnvido(manoClienteAux, manoServerAux, envido, &puntosServer, &puntosCliente,
						manoNumero, puntosMaximos, &grilla, manoServer, nombreServer,nombreCliente, &opcionServer, &opcionCliente);
						if(puntosServer == puntosMaximos || puntosCliente == puntosMaximos) flag1 = 1;
						hayQueProcesarEnvido = 0;
					}
				}
				while((comparar[0][0]=='t'||comparar[1][0]=='t')&&(flag1 == 0));
				
				if(flag1==0)quienGano(mazo, comparar, &primera, &segunda, &tercera, i);
				comparar[0][0]='t';
				comparar[1][0]='t';
				i++;
			}
			while(((primera == 1 && segunda == -1)||(primera == 0 && segunda == 1 && tercera == -1))&& flag1 == 0)//tira carta el cliente primero
			{
				do
				{
					if(truco>1 && seCantoTruco == 0 && comparar[1][0]!='t');
					else
					{
						turnoDelCliente(&grilla, manoServerAux, manoClienteAux,&opcionServer, &opcionCliente, nombreServer,
 						nombreCliente, &manoCliente,i, &envido, &truco, &seCantoTruco, &seCantoEnvido,
 						&quienTieneElQuiero, manoNumero, &comparar, manoServer,
 						&hayQueProcesarEnvido, &puntosServer, puntosCliente, &flag,&flag1);
					}

					if(truco>1 && seCantoTruco == 0 && comparar[1][0]=='t')continue;
					if(flag1 == 1)continue;

					turnoDelServer(&grilla, manoServerAux,manoClienteAux,&opcionServer, &opcionCliente,nombreServer,
					nombreCliente, &manoServer,&comparar, i, &envido, &truco, &seCantoTruco,&seCantoEnvido, 
					&hayQueProcesarEnvido, &puntosCliente, &puntosServer, &flag,&flag1, &quienTieneElQuiero, manoNumero );
				}
				while((comparar[0][0]=='t'||comparar[1][0]=='t')&&(flag1 == 0));

				if(flag1==0)quienGano(mazo, comparar, &primera, &segunda, &tercera, i);
				comparar[0][0]='t';
				comparar[1][0]='t';
				i++;
			}
			while((primera == 1 && segunda == 0 && tercera == -1)&& flag1 == 0)//tira carta el server primero
			{
				do
				{
					if(truco>1 && seCantoTruco == 0 && comparar[0][0]!='t');
					else{
						turnoDelServer(&grilla, manoServerAux,manoClienteAux,&opcionServer, &opcionCliente,nombreServer,
						nombreCliente, &manoServer,&comparar, i, &envido, &truco, &seCantoTruco,&seCantoEnvido, 
						&hayQueProcesarEnvido, &puntosCliente, &puntosServer, &flag,&flag1, &quienTieneElQuiero, manoNumero );
					}

					if(truco>1 && seCantoTruco == 0 && comparar[0][0]=='t')continue;
					if(flag1 == 1)continue;
					
					turnoDelCliente(&grilla, manoServerAux, manoClienteAux,&opcionServer, &opcionCliente, nombreServer,
 					nombreCliente, &manoCliente,i, &envido, &truco, &seCantoTruco, &seCantoEnvido,
 					&quienTieneElQuiero, manoNumero, &comparar, manoServer,
 					&hayQueProcesarEnvido, &puntosServer, puntosCliente,&flag, &flag1);

				}
				while((comparar[0][0]=='t'||comparar[1][0]=='t')&&(flag1 == 0));
				
				if(flag1==0)quienGano(mazo, comparar, &primera, &segunda, &tercera, i);
				comparar[0][0]='t';
				comparar[1][0]='t';
				i++;
			}
		}
		else//ES MANO EL CLIENTE
		{
			while((primera == -1 || (primera == 1 && segunda == -1)|| (primera == 2 && segunda == -1)||
			(primera == 2 && segunda == 2 && tercera == -1))&& flag1 == 0)//tira carta el cliente primero
			{
				do
				{
					//si no se quiso el envido
					if(envido > 0 && envido < 100 && seCantoEnvido == 0)
					{//es para evitar bugs al no aceptar el envido
						envido = 100;//le pongo 100 para no pasar de vuelta por este if
						continue;
					}
					else if(truco>1 && seCantoTruco == 0 && comparar[1][0]!='t');
					else
					{
						turnoDelCliente(&grilla, manoServerAux, manoClienteAux, &opcionServer, &opcionCliente, nombreServer,
 						nombreCliente, &manoCliente,i, &envido, &truco, &seCantoTruco, &seCantoEnvido,
 						&quienTieneElQuiero, manoNumero, &comparar, manoServer,
 						&hayQueProcesarEnvido, &puntosServer, puntosCliente, &flag,&flag1);
					}

					if(hayQueProcesarEnvido == 1)//si se acepto el envido
					{
						procesarEnvido(manoClienteAux, manoServerAux, envido, &puntosServer, &puntosCliente,
						manoNumero, puntosMaximos, &grilla, manoServer, nombreServer,nombreCliente, &opcionServer, &opcionCliente);
						if(puntosServer == puntosMaximos || puntosCliente == puntosMaximos) flag1 = 1;
						hayQueProcesarEnvido = 0;
					}
					if(envido > 0 && envido < 100 && seCantoEnvido == 0 && comparar[1][0]=='t')
					{
						envido = 100;
						continue;
					}
					
					if(truco>1 && seCantoTruco == 0 && comparar[1][0]=='t')continue;
					if(flag1 == 1)continue;
					
					turnoDelServer(&grilla, manoServerAux,manoClienteAux,&opcionServer, &opcionCliente,nombreServer,
					nombreCliente, &manoServer,&comparar, i, &envido, &truco, &seCantoTruco,&seCantoEnvido, 
					&hayQueProcesarEnvido, &puntosCliente, &puntosServer, &flag,&flag1, &quienTieneElQuiero, manoNumero );

					if(hayQueProcesarEnvido == 1)
					{
						procesarEnvido(manoClienteAux, manoServerAux, envido, &puntosServer, &puntosCliente,
						manoNumero, puntosMaximos, &grilla, manoServer, nombreServer,nombreCliente, &opcionServer, &opcionCliente);
						if(puntosServer == puntosMaximos || puntosCliente == puntosMaximos) flag1 = 1;
						hayQueProcesarEnvido = 0;
					}
				}
				while((comparar[0][0]=='t'||comparar[1][0]=='t')&&(flag1== 0));

				if(flag1==0)quienGano(mazo, comparar, &primera, &segunda, &tercera, i);
				comparar[0][0]='t';
				comparar[1][0]='t';
				i++;
			}
			while(((primera == 0 && segunda == -1)||(primera == 1 && segunda == 0 && tercera == -1))&& flag1 == 0)//tira carta el server primero
			{
				do
				{
					if(truco>1 && seCantoTruco == 0 && comparar[0][0]!='t');
					else{
						turnoDelServer(&grilla, manoServerAux,manoClienteAux,&opcionServer, &opcionCliente,nombreServer,
						nombreCliente, &manoServer,&comparar, i, &envido, &truco, &seCantoTruco,&seCantoEnvido, 
						&hayQueProcesarEnvido, &puntosCliente, &puntosServer, &flag,&flag1, &quienTieneElQuiero, manoNumero );
					}

					if(truco>1 && seCantoTruco == 0 && comparar[0][0]=='t')continue;
					if(flag1 == 1)continue;

					turnoDelCliente(&grilla, manoServerAux, manoClienteAux, &opcionServer, &opcionCliente, nombreServer,
 					nombreCliente, &manoCliente,i, &envido, &truco, &seCantoTruco, &seCantoEnvido,
 					&quienTieneElQuiero, manoNumero, &comparar, manoServer,
 					&hayQueProcesarEnvido, &puntosServer, puntosCliente,&flag, &flag1);

				} 
				while ((comparar[0][0]=='t'||comparar[1][0]=='t')&&(flag1 == 0));
				
				if(flag1==0)quienGano(mazo, comparar, &primera, &segunda, &tercera, i);
				comparar[0][0]='t';
				comparar[1][0]='t';
				i++;
			}
			while((primera == 0 && segunda == 1 && tercera == -1)&& flag1 == 0)//tira carta el cliente primero
			{
				do
				{
					if(truco>1 && seCantoTruco == 0 && comparar[1][0]!='t');
					else{
						turnoDelCliente(&grilla, manoServerAux,manoClienteAux, &opcionServer, &opcionCliente, nombreServer,
 						nombreCliente, &manoCliente,i, &envido, &truco, &seCantoTruco, &seCantoEnvido,
 						&quienTieneElQuiero, manoNumero, &comparar, manoServer,
 						&hayQueProcesarEnvido, &puntosServer, puntosCliente,&flag, &flag1);
					}

				    if(truco>1 && seCantoTruco == 0 && comparar[1][0]=='t')continue;
					if(flag1 == 1)continue;

					turnoDelServer(&grilla, manoServerAux,manoClienteAux,&opcionServer, &opcionCliente,nombreServer,
					nombreCliente, &manoServer,&comparar, i, &envido, &truco, &seCantoTruco,&seCantoEnvido, 
					&hayQueProcesarEnvido, &puntosCliente, &puntosServer, &flag,&flag1, &quienTieneElQuiero, manoNumero );
				} 
				while ((comparar[0][0]=='t'||comparar[1][0]=='t')&&(flag1 == 0));
				
				if(flag1==0)quienGano(mazo, comparar, &primera, &segunda, &tercera, i);
				comparar[0][0]='t';
				comparar[1][0]='t';
				i++;
			}
		}

		//subo puntos
		if(flag1==0)//no en el caso de no querer truco, irse al mazo o ganar el partido con el envido
		{
			if((primera == 0 && segunda == 0)||(primera == 0 && segunda == 2)||(primera == 0 && segunda == 1 && tercera == 0)||
			(primera == 0 && segunda == 1 && tercera == 2)||(primera == 1 && segunda == 0 && tercera == 0)||(primera == 2 && segunda == 0)||
			(primera == 2 && segunda == 2 && tercera == 0)||(primera = 2 && segunda == 2 && tercera == 2 && (manoNumero%2) == 1))//gana el server
			{
				puntosServer += truco;
				actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);

			}
			else //gana el cliente
			{
				puntosCliente += truco;
				actualizarGrilla(&grilla, puntosServer, puntosCliente, nombreServer, nombreCliente);
			}
		}

		//veo si alguien gano
		if(puntosServer >= puntosMaximos)
		{
			auxFlag = flag;
			flag = 3;
			enviarFlag(flag);
			enviarGrilla(grilla);
			system("clear");
			imprimirGrilla(grilla);
			imprimirMano(manoServer);
			if(auxFlag == 1)//si el ultimo en tirar fue el cliente
			{
				imprimirEstado(manoClienteAux, &opcionCliente, nombreCliente);
				enviarString("Nada");
			}
			else if(auxFlag == 0)
			enviarEstado(manoServerAux, &opcionServer, nombreServer);

			printf("%s ha ganado el partido\n", nombreServer);
			sprintf(buf, "%s ha ganado el partido", nombreServer);
			enviarString(buf);
		}
		else if(puntosCliente >= puntosMaximos)
		{
			auxFlag = flag;
			flag = 3;
			enviarFlag(flag);
			enviarGrilla(grilla);
			system("clear");
			imprimirGrilla(grilla);
			imprimirMano(manoServer);
			if(auxFlag == 1)//si el ultimo en tirar fue el cliente
			{
				imprimirEstado(manoClienteAux, &opcionCliente, nombreCliente);
				enviarString("Nada");
			}
			else if(auxFlag == 0)
			enviarEstado(manoServerAux, &opcionServer, nombreServer);

			printf("%s ha ganado el partido\n", nombreCliente);
			sprintf(buf, "%s ha ganado el partido", nombreCliente);
			enviarString(buf);
		}
		else//si nadie gano
		{
			system("clear");
			imprimirGrilla(grilla);
			imprimirMano(manoServer);
			if(flag == 1)//si el ultimo en tirar fue el cliente
			imprimirEstado(manoClienteAux, &opcionCliente, nombreCliente);
			printf("Finalizo la mano\n");
			manoNumero++;
		}

	}

    close(sockfd);
    close(new_fd);

	return 0;
}
