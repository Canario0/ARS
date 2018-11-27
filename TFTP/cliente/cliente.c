// Practica tema 5, Renero Balgañón Pablo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/unistd.h>

#define PROGRAM_NAME "tftp-Renero-Balganon"
#define MODE "octet"

void output(int const pos, char const *argv[], const int total);
void paramError();
void noParamError();
void ayuda();
void ipError(const char *);
char *readWriteRequest();
char *ackPackage(int);
char *dataPackage(int, char*);

// Variables Globales
struct in_addr server_ip;
uint16_t server_port;
int request;
char *file_name;
int vervose = 0;
int package_size = 0;

int main(int argc, char const *argv[])
{
	// Bloque datos de entrada
	// Compruebo que hay parametros de entrada, si no los hay se imprime un mensaje avisando
	if (argc <= 1)
	{
		noParamError();
	}

	// Compruebo que si existen más de un parámetro de entrada tienen e ser tres o cuatro en total
	if ((argc - 1) > 1 && !((argc - 1) >= 3 && (argc - 1) <= 4))
	{
		paramError();
	}

	// Llamo a la función que parsea los datos con el primer elemento
	int i;
	for (i = 1; i < argc; i++)
	{
		output(i, argv, argc - 1);
	}
	// Fin bloque datos de entrada
	char *dataout;
	dataout = readWriteRequest();
	printf("Número de bytes del paquete: %d\n", package_size);
	printf("esto es mi paquete to rechulon: %s\n", dataout);
	dataout = ackPackage(1);
	printf("Número de bytes del paquete: %d\n", package_size);
	printf("esto es mi paquete to rechulon: %s\n", dataout);
	dataout = dataPackage(1,"Mando algunos datos, y otros no los mando");
	printf("Número de bytes del paquete: %d\n", package_size);
	printf("esto es mi paquete to rechulon: %s\n", dataout);

	free(dataout);
	free(file_name);
	return 0;
}

/**
 * Función que parsea los argumentos de entrada
 * pos posición a parsear
 * argv argumentos de entrada
 * total total de argumentos
 */
void output(int const pos, char const *argv[], const int total)
{
	//opción para mostrar la ayuda
	if (strcmp(argv[pos], "-h") == 0)
	{
		//En el caso de que la opción -h no esté en la pos 1 o tenga más amentos detras se lanza un error de entrada
		if (pos != 1 || total != 1)
		{
			paramError();
		}
		ayuda();
	}
	else if (strcmp(argv[pos], "-r") == 0 || strcmp(argv[pos], "-w") == 0)
	{
		if (pos != 2)
			paramError();

		if (strcmp(argv[pos], "-r") == 0)
		{
			request = 01;
		}
		else
		{
			request = 02;
		}
		printf("Modo %s\n", argv[pos]);
	}
	else if (strcmp(argv[pos], "-v") == 0)
	{
		if (pos != 4)
			paramError();

		vervose = 1;
	}
	else
	{
		//En el caso de que llegue aquí con una posición que no sea 1 se lanza un mensaje de error.
		if (pos != 1 && pos != 3)
		{
			paramError();
		}
		if (pos == 1)
		{
			//Traduce la ip y comprueba que es válida.
			if (inet_aton(argv[pos], &server_ip) == 0)
			{
				ipError(argv[pos]);
			}
			printf("La ip es: %d\n", server_ip.s_addr);
		}
		else
		{
			if (strcmp(argv[pos - 1], "-r") == 0 && strcmp(argv[pos - 1], "-w") == 0)
				paramError();

			if ((file_name = (char *)calloc(100, sizeof(char))) == 0)
			{
				perror("Fallo al reservar memoria para el nombre del fichero");
				exit(EXIT_FAILURE);
			}
			strncpy(file_name, argv[pos], 100);
			printf("Nombre del archivo %s \n", file_name);
		}
	}
}

/**
 * Función que imprime un mensaje de error en los parámetros
 */
void paramError()
{
	printf("\nNumero de parametros erroneo\nPruebe './%s -h' para mas informacion\n\n", PROGRAM_NAME);
	exit(EXIT_FAILURE);
}
/**
 * Función que imprime un mensaje de error cuando no hay parámetros de entrada
 */
void noParamError()
{
	printf("Cliente: debe indicar almenos una direción IP\nPruebe './%s -h' para más información.\n", PROGRAM_NAME);
	exit(EXIT_FAILURE);
}

/**
 * Función que imprime la ayuda desplegada con la opción -h
 */
void ayuda()
{
	printf("\n\033[1;33mUso:\033[0m\n\t./%s direccion.IP.servidor {-r|-w} nombre.archivo [-v] \n\n", PROGRAM_NAME);
	printf("\033[1;33mOpciones:\n\t-r\033[0m\n\t   Indica que se desea leer un fichero del servidor.\n\n");
	printf("\t\033[1;33m-w\033[0m\n\t   Indica que se desea escribir un fichero del servidor.\n\n");
	printf("\t\033[1;33m-v\033[0m\n\t   Parametro opcional que hara que el cliente informe de todos los pasos necesarios para enviar o recibir un fichero.\n\n");
	exit(0);
}

/**
 * Función que imprime un mensaje de error si la ip no es válida
 */
void ipError(const char *in)
{
	printf("\nLa ip: '%s' no es válidad\n\n", in);
	exit(EXIT_FAILURE);
}

char *readWriteRequest()
{
	package_size = 0;
	char *package;
	if ((package = (char *)malloc(512 * sizeof(char))) == 0)
	{
		perror("Fallo al reservar memoria para el paquete RRQ o WRQ");
		exit(EXIT_FAILURE);
	}
	if (sprintf(package, "%02d", request) < 0)
	{
		perror("opencode sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size = 2;
	int aux_size;
	aux_size = sprintf(package + 2, "%s", file_name);
	if (aux_size < 0)
	{
		perror("Nombre del archivo sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size += aux_size;
	if (sprintf(package + package_size, "%d", 0) < 0)
	{
		perror("EOS sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size++;
	aux_size = sprintf(package + package_size, "%s", MODE);
	if (aux_size < 0)
	{
		perror("Modo sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size += aux_size;
	if (sprintf(package + package_size, "%d", 0) < 0)
	{
		perror("EOS sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size++;
	return package;
}

char * ackPackage(int block_number){
	package_size = 0;
	char *package;
	if ((package = (char *)malloc(512 * sizeof(char))) == 0)
	{
		perror("Fallo al reservar memoria para el paquete ACK");
		exit(EXIT_FAILURE);
	}
	if (sprintf(package, "%02d", 04) < 0)
	{
		perror("opencode sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size=2;
	int aux_size;
	aux_size = sprintf(package + 2, "%02d", block_number);
	if (aux_size < 0)
	{
		perror("Numero de bloque sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size+= aux_size;
	return package;


}


char * dataPackage(int block_number, char* data){
	package_size = 0;
	char *package;
	if ((package = (char *)malloc(516 * sizeof(char))) == 0)
	{
		perror("Fallo al reservar memoria para el paquete de datos");
		exit(EXIT_FAILURE);
	}
	if (sprintf(package, "%02d", 03) < 0)
	{
		perror("opencode sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size=2;
	int aux_size;
	aux_size = sprintf(package + 2, "%02d", block_number);
	if (aux_size < 0)
	{
		perror("Numero de bloque sprintf()");
		exit(EXIT_FAILURE);
	}
	package_size+= aux_size;
	aux_size = sprintf(package +package_size , "%s", data);
	if (aux_size < 0)
	{
		perror("Datos sprintf()");
		exit(EXIT_FAILURE);
	}
	printf("Esto es lo que escribo %d", aux_size);
	package_size+= aux_size;
	return package;


}
