#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void convertir(int digito, char arrayaux[][20]);

void convertir(int digito, char arrayaux[][20])
{
	switch(digito)
	{
		case(1):strcpy(*arrayaux,"I");
		break;
		case(2):strcpy(*arrayaux,"II");
		break;
		case(3):strcpy(*arrayaux,"III");
		break;
		case(4):strcpy(*arrayaux,"IV");
		break;
		case(5):strcpy(*arrayaux,"V");
		break;
		case(6):strcpy(*arrayaux,"VI");
		break;
		case(7):strcpy(*arrayaux,"VII");
		break;
		case(8):strcpy(*arrayaux,"VIII");
		break;
		case(9):strcpy(*arrayaux,"IX");
		break;
		case(10):strcpy(*arrayaux,"X");
		break;
		case(20):strcpy(*arrayaux,"XX");
		break;
		case(30):strcpy(*arrayaux,"XXX");
		break;
		case(40):strcpy(*arrayaux,"XL");
		break;
		case(50):strcpy(*arrayaux,"L");
		break;
		case(60):strcpy(*arrayaux,"LX");
		break;
		case(70):strcpy(*arrayaux,"LXX");
		break;
		case(80):strcpy(*arrayaux,"LXXX");
		break;
		case(90):strcpy(*arrayaux,"XC");
		break;
		case(100):strcpy(*arrayaux,"C");
		break;
		case(200):strcpy(*arrayaux,"CC");
		break;
		case(300):strcpy(*arrayaux,"CCC");
		break;
		case(400):strcpy(*arrayaux,"CD");
		break;
		case(500):strcpy(*arrayaux,"D");
		break;
		case(600):strcpy(*arrayaux,"DC");
		break;
		case(700):strcpy(*arrayaux,"DCC");
		break;
		case(800):strcpy(*arrayaux,"DCCC");
		break;
		case(900):strcpy(*arrayaux,"CM");
		break;
		case(1000):strcpy(*arrayaux,"M");
		break;
		case(2000):strcpy(*arrayaux,"MM");
		break;
		case(3000):strcpy(*arrayaux,"MMM");
		break;
		default:
		break;
	}
}

int main()
{
	int digito[10], numero = 0, aux= 0, i = 0, j = 0;
	char buf[10], salida[20], arrayaux[20];

	while(1)
	{
		system("clear");
		printf("Ingrese el numero a convertir: ");

		fgets(buf, 10, stdin);
		buf[strcspn(buf, "\n")]='\0';

		numero = atoi(buf);
		if(numero <= 3999)
		{
			aux = numero;
			for(i = 0; i < strlen(buf);i++)
			{
				digito[i]= aux%10;
				for(j = 0;j < i;j++)
				digito[i] = digito[i]*10;
				aux = aux /10;
				//printf("digito[%d]= %d\n", i, digito[i]);
			}

			salida[0]= '\0';
			for(i = strlen(buf) - 1; i >= 0 ;i--)
			{
				if(digito[i] != 0)
				{
					convertir(digito[i], &arrayaux);
					strcpy(&(salida[strlen(salida)]), arrayaux);
				}
			}
			printf("El numero romano es: %s\n", salida);
		}
		else
			printf("El numero ingresado debe ser menor a 4000\n");
		fgets(buf, 10, stdin);
		buf[strcspn(buf, "\n")]='\0';
	}

	return 0;
}
