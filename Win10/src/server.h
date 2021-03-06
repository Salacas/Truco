
//defines
#define MYPORT 	3490		// Puerto al que conectar�n los usuarios
#define BACKLOG 0			// Cu�ntas conexiones pendientes se mantienen en cola
#define MAXDATASIZE 100     // m�ximo n�mero de bytes que se pueden leer de una vez

//variables globales
int sockfd, new_fd;	// Escuchar sobre sock_fd, conexion del cliente sobre new_fd
char mensajeServer[MAXDATASIZE], mensajeCliente[MAXDATASIZE];// variables usadas por enviarEstado e imprimirEstado
//struct
typedef struct carta
{
	char numero[3];//el numero mas largo tiene 2 caracteres + null
	char palo[7];//el palo mas largo es la espada(seis letras) + null
	int valor;

}CARTA;

//prototipos
void inicializarMazo(CARTA* mazo, char* numero[], char* palo[]);

void mezclar(CARTA* mazo);

void repartir(CARTA* mazo, char manoServer[][3][17], char manoCliente[][3][17]);

void enviarCartas(char manoCliente[][3][17]);

void enviarString(char* buf);

void recibirString(char* buf);

void inicializarGrilla(char grilla[][4][90], char* nombreServer, char* nombreCliente, int puntosMaximos);

void imprimirGrilla(char grilla[4][90]);

void imprimirMano(char manoServer[3][17]);

void ponerCartaServer(int numeroCarta, char manoserver[][3][17], char grilla[][4][90]);

void ponerCartaCliente(int numeroCarta, char manoCliente[][3][17], char grilla[][4][90]);

void enviarGrilla(char grilla[4][90]);

void enviarFlag(int flag);

int imprimirOpciones(char manoServerP[][3][17], char compararP[][2][17], char grillaP[][4][90], int j, int* envidoP, int* trucoP,
	int* seCantoTrucoP, int* seCantoEnvidoP, int* hayQueProcesarEnvidoP, int* puntosClienteP, int puntosServer, char* nombreServer,
	char* nombreCliente, int* flag1P, int* quienTieneElQuieroP, int manoNumero);

void quienGano(CARTA* mazo, char comparar[2][17], int* primeraP, int* segundaP, int* terceraP, int h);

void actualizarGrilla(char grilla[][4][90], int puntosServer, int puntosCliente, char* nombreServer, char* nombreCliente);

void limpiarGrilla(char grilla[][4][90], int puntosServer, int puntosCliente, char* nombreServer, char* nombreCliente, int puntosMaximos);

void enviarOpciones(char manoClienteP[][3][17], int j, int* envidoP, int* trucoP,
	int* seCantoTrucoP, int* seCantoEnvidoP, int* quienTieneElQuieroP, int manoNumero, char comparar[2][17]);

void recibirOpcionYActualizar(int* opcionClienteP, char manoClienteP[][3][17], char compararP[][2][17],
	char grillaP[][4][90], int j, int* envidoP, int* trucoP, int* seCantoTrucoP, int* seCantoEnvidoP,
	int* hayQueProcesarEnvidoP, int* puntosServerP, int puntosCliente, char* nombreServer, char* nombreCliente,
	int* flag1P, int* quienTieneElQuieroP);

int calcularEnvido(char manoAux[3][17]);

void procesarEnvido(char manoClienteAux[3][17], char manoServerAux[3][17], int envido, int* puntosServerP,
	int* puntosClienteP, int manoNumero, int puntosMaximos, char grillaP[][4][90], char manoServer[3][17], char* nombreServer, char* nombreCliente,
	int* opcionServerP, int* opcionClienteP);

void imprimirEstado(char manoClienteAux[3][17], int* opcionClienteP, char* nombreCliente);

void enviarEstado(char manoServerAux[3][17], int* opcionServerP, char* nombreServer);

void turnoDelServer(char grillaP[][4][90], char manoServerAux[3][17], char manoClienteAux[3][17], int* opcionServerP, int* opcionClienteP,
	char* nombreServer, char* nombreCliente, char manoServerP[][3][17], char compararP[][2][17], int i, int* envidoP, int* trucoP, int* seCantoTrucoP,
	int* seCantoEnvidoP, int* hayQueProcesarEnvidoP, int* puntosClienteP, int* puntosServerP, int* flagP, int* flag1P, int* quienTieneElQuieroP, int manoNumero);

void turnoDelCliente(char grillaP[][4][90], char manoServerAux[3][17], char manoClienteAux[3][17], int* opcionServerP, int* opcionClienteP, char* nombreServer,
	char* nombreCliente, char manoClienteP[][3][17], int i, int* envidoP, int* trucoP, int* seCantoTrucoP, int* seCantoEnvidoP,
	int* quienTieneElQuieroP, int manoNumero, char compararP[][2][17], char manoServer[3][17],
	int* hayQueProcesarEnvidoP, int* puntosServerP, int puntosCliente, int* flagP, int* flag1P);

void clean_stdin(void);
