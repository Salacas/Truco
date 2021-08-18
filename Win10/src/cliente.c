#define _CRT_SECURE_NO_WARNINGS//windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS//windows

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ws2tcpip.h>//windows
#include <winsock.h>//windows
#include <windows.h>//windows
#include "client.h"
#pragma comment(lib, "wsock32.lib")//windows
#pragma comment(lib, "Ws2_32.lib")//windows

int main(int argc, char* argv[])
{
    struct in_addr addr;
    int  manonumero = 0, flag = 0, flagAux = 0;
    char  nombreServer[12], nombreCliente[12], buf[MAXDATASIZE];
    struct sockaddr_in their_addr;
    char manoCliente[3][17];
    char grilla[4][90];

    WSADATA wsaData;// windows

    //windows
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    printf("IP: ");
    scanf("%s", buf);
    clean_stdin();

    if (inet_pton(AF_INET, buf, &addr) == 0)
    {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = addr;

    memset(&(their_addr.sin_zero), 0, 8);

    if (connect(sockfd, (struct sockaddr*)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        closesocket(sockfd);
        exit(1);
    }

    //inicio de conexion--------------------------------------------------------------------
    printf("Conexion exitosa con el servidor\n");
    do {
        printf("Ingrese su nombre: ");
        fgets(buf, MAXDATASIZE, stdin);
        buf[strcspn(buf, "\n")] = '\0';//le saco el \n
        flagAux = 0;
        if (strlen(buf) > 11) {
            printf("El nombre debe contener 11 caracteres como maximo\n");
            flagAux = 1;
        }
    } while (flagAux == 1);
    strcpy(nombreCliente, buf);

    recibirString(nombreServer);//recibo el nombre del jugador server
    enviarString(nombreCliente);//envio el nombre del jugador cliente

    while (flag != 3)//flag = 3 significa que termino el juego
    {
        recibirFlag(&flag);

        if (flag == 3)//finaliza el partido
        {
            system("cls");
            recibirGrilla(&grilla);
            imprimirGrilla(grilla);
            imprimirMano(manoCliente);
            recibirString(buf);//recibo el mensaje del server
            if (strcmp(buf, "Nada") != 0)//lo imprimo si tengo que hacerlo
                printf("%s\n\n", buf);
            recibirString(buf);//quien gano
            printf("%s\n", buf);
        }
        else if (flag == 2)//empieza una mano
        {
            manonumero++;
            if (manonumero != 1)
            {
                system("cls");
                recibirGrilla(&grilla);
                imprimirGrilla(grilla);
                imprimirMano(manoCliente);
                recibirString(buf);//recibo el mensaje del server
                if (strcmp(buf, "Nada") != 0)//lo imprimo si tengo que hacerlo
                    printf("%s\n\n", buf);
                printf("Finalizo la mano\n");
                Sleep(4000);
            }
            recibirCartas(&manoCliente);
        }
        else if (flag == 0)//turno del server
        {
            system("cls");
            recibirGrilla(&grilla);
            imprimirGrilla(grilla);
            imprimirMano(manoCliente);
            recibirString(buf);//recibo el mensaje del server
            if (strcmp(buf, "Nada") != 0)//lo imprimo si tengo que hacerlo
                printf("%s\n\n", buf);
            printf("Turno de %s\n", nombreServer);
        }
        else if (flag == 1)//turno del cliente
        {
            system("cls");
            recibirGrilla(&grilla);
            imprimirGrilla(grilla);
            imprimirMano(manoCliente);
            recibirString(buf);
            if (strcmp(buf, "Nada") != 0)
                printf("%s\n\n", buf);
            imprimirOpciones(&manoCliente);
        }
    }

    //windows
    WSACleanup();
    closesocket(sockfd);
    system("pause");

    return 0;
}
