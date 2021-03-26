#define PORT 3490 			// puerto al que vamos a conectar
#define MAXDATASIZE 100 	// máximo número de bytes de arrays

//variables globales
int sockfd;

//prototipos-----------------------------
void recibirCartas(char manoCliente[][3][17] );

void enviarString(char *buf);

void recibirString(char *buf);

void imprimirMano(char manoCliente[3][17]);

void recibirGrilla(char grillaP[][4][90]);

void imprimirGrilla(char grilla[4][90]);

void revertirGrilla(char grillaP[][4][90]);

void recibirFlag(int *flagp);

void imprimirOpciones(char manoClienteP[][3][17]);