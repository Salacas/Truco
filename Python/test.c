#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int calcularEnvido ( char manoAux[3][17]);
int calcularEnvido(char manoAux[3][17])
{
	//calcula y devuelve el envido de la manoAux
	int i = 0, envido = 0, numeros[3], alternativas[] = { 0,0,0 };//inicializo alternativas en 0
	char* palos[3];

	for (i = 0;i < 3; i++)
	{
		numeros[i] = atoi(strtok(manoAux[i], " "));//guardo los numeros de las cartas
		strtok(NULL, " ");
		palos[i] = strtok(NULL, " ");//guardo los palos de las cartas
	}

	if ((numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12) && (numeros[1] == 10 || numeros[1] == 11 || numeros[1] == 12) &&
		(numeros[2] == 10 || numeros[2] == 11 || numeros[2] == 12))//si todas las cartas de la mano son 10,11 o 12
	{
		//si al menos 2 cartas comparte palo
		if (strcmp(palos[0], palos[1]) == 0 || strcmp(palos[1], palos[2]) == 0 || strcmp(palos[0], palos[2]) == 0)
			envido = 20;
		else
			envido = 0;

	}
	else//si existe al menos una carta que no sea ni 10 ni 11 ni 12
	{
		//comparo los palos de dos cartas y si hay envido lo cargo en alternativa[0]
		if (strcmp(palos[0], palos[1]) == 0)
		{
			//si las dos cartas son 10,11 o 12
			if ((numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12) && (numeros[1] == 10 || numeros[1] == 11 || numeros[1] == 12))
				alternativas[0] = 20;
			//si una de las dos cartas es un 10,11 o 12
			else if ((numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12 || numeros[1] == 10 || numeros[1] == 11 || numeros[1] == 12))
			{
				alternativas[0] = 20;
				if (numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12)
					alternativas[0] += numeros[1];
				else
					alternativas[0] += numeros[0];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
				alternativas[0] = 20 + numeros[0] + numeros[1];

		}
		//comparo los palos de otras dos cartas y si hay envido lo cargo en alternativa[1]
		if (strcmp(palos[1], palos[2]) == 0)
		{
			//si las dos cartas son 10,11 o 12
			if ((numeros[1] == 10 || numeros[1] == 11 || numeros[1] == 12) && (numeros[2] == 10 || numeros[2] == 11 || numeros[2] == 12))
				alternativas[1] = 20;
			//si una de las dos cartas es un 10,11 o 12
			else if ((numeros[1] == 10 || numeros[1] == 11 || numeros[1] == 12 || numeros[2] == 10 || numeros[2] == 11 || numeros[2] == 12))
			{
				alternativas[1] = 20;
				if (numeros[1] == 10 || numeros[1] == 11 || numeros[1] == 12)
					alternativas[1] += numeros[2];
				else
					alternativas[1] += numeros[1];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
				alternativas[1] = 20 + numeros[1] + numeros[2];

		}
		//comparo los palos de otras dos cartas y si hay envido lo cargo en alternativa[2]
		if (strcmp(palos[0], palos[2]) == 0)
		{
			//si las dos cartas son 10,11 o 12
			if ((numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12) && (numeros[2] == 10 || numeros[2] == 11 || numeros[2] == 12))
				alternativas[2] = 20;
			//si una de las dos cartas es un 10,11 o 12
			else if ((numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12 || numeros[2] == 10 || numeros[2] == 11 || numeros[2] == 12))
			{
				alternativas[2] = 20;
				if (numeros[0] == 10 || numeros[0] == 11 || numeros[0] == 12)
					alternativas[2] += numeros[2];
				else
					alternativas[2] += numeros[0];
			}
			//si ninguna de las cartas es un 10,11 o 12
			else
				alternativas[2] = 20 + numeros[0] + numeros[2];

		}
		//si no hay al menos 2 cartas con el mismo palo
		if (alternativas[0] == 0 && alternativas[1] == 0 && alternativas[2] == 0)
		{
			for (i = 0;i < 3;i++)
			{
				if (numeros[i] < 10)
					alternativas[i] = numeros[i];
			}
		}

		//comparo las alternativas y me quedo con la mas alta

		for (i = 0;i < 3;i++)
		{
			if (envido < alternativas[i])
				envido = alternativas[i];
		}
	}

	return envido;
}

int main()
{
    char mano[3][17];
    strcpy(mano[0],"10 de Espada");
    strcpy(mano[1], "10 de Basto");
    strcpy(mano[2], "12 de Oro");
    printf("%d", calcularEnvido(mano));

    return 0;
}