// Practica tema 8, Renero Balgañón Pablo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include "ip-icmp-ping.h"

#define PROGRAM_NAME "miping-Renero-Balganon"
#define MODE "octet"

void output(int const pos, char const *argv[], const int total);
void paramError();
void noParamError();
void ayuda();
void ipError(const char *);
ECHORequest icmpRequest();
unsigned char *ackPackage(int);
unsigned char *dataPackage(int);

// Variables Globales
struct in_addr server_ip;
uint16_t server_port;
int request;
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
	if ((argc - 1) > 2 )
	{
		paramError();
	}

	output(1, argv, argc - 1);

	if(argc ==3){
		output(2, argv, argc - 1);
	}
//	// Obtenemos el número del puerto TFTP
//	struct servent *aux;
//	aux = getservbyname("tftp", "udp");
//	// Si el resultado es NULL imprimimos el mensaje de error
//	if (!aux)
//	{
//		perror("getservbyname()");
//		exit(EXIT_FAILURE);
//	}
//	server_port = aux->s_port;
//	// Fin bloque datos de entrada
//
//	//Bloque del socket
//	int id_sock;
//	//Creamos el socket y comprobamos los posibles errores
//	id_sock = socket(AF_INET, SOCK_DGRAM, 0);
//	if (id_sock == -1)
//	{
//		perror("socket()");
//		exit(EXIT_FAILURE);
//	}
//	// Fin bloque del socket
//
//	// Bloque de bind
//	int error;
//	struct sockaddr_in local_addr;
//	local_addr.sin_family = AF_INET;
//	local_addr.sin_port = 0;
//	local_addr.sin_addr.s_addr = INADDR_ANY;
//	// Hacemos el bind con el puerto y comprobamos los errores
//	error = bind(id_sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
//	if (error < 0)
//	{
//		perror("bind()");
//		// En el caso de que el bind falle el socket se queda encendido, por eso hay que cerrarlo
//		error = close(id_sock);
//		if (error < 0)
//		{
//			perror("close()");
//			exit(EXIT_FAILURE);
//		}
//		exit(EXIT_FAILURE);
//	}
//	// Fin bloque de bind
//
//	//Inicio de la comunicación
//	switch (request)
//	{
//		case 01:
//			readAction(id_sock);
//			break;
//		case 02:
//			writeAction(id_sock);
//			break;
//		default:
//			printf("Failure of the program");
//			exit(EXIT_FAILURE);
//	}
//	free(file_name);
//	// Fin de la comunicación
//
//	// Cierro el socket
//	error = close(id_sock);
//	if (error < 0)
//	{
//		perror("close()");
//		exit(EXIT_FAILURE);
//	}
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
	else if (strcmp(argv[pos], "-v") == 0)
	{
		if (pos != 2)
			paramError();

		vervose = 1;
	}
	else
	{
		//En el caso de que llegue aquí con una posición que no sea 1 se lanza un mensaje de error.
		if (pos != 1)
		{
			paramError();
		}
		//Traduce la ip y comprueba que es válida.
		if (inet_aton(argv[pos], &server_ip) == 0)
		{
			ipError(argv[pos]);
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
	printf("\n\033[1;33mUso:\033[0m\n\t./%s direccion.IP.servidor [-v] \n\n", PROGRAM_NAME);
	printf("\033[1;33m-v\033[0m\n\t   Parametro opcional que hara que el cliente informe de todos los pasos necesarios para enviar o recibir un fichero.\n\n");
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

/**
 * Función que crea tanto los paquetes de lectura como los de escritura, 
 * para saber qué código de operación poner se usa una variable global 
 * que contiene el código correspondiente y es asignada por el método
 * output().
 */
ECHORequest icmpRequest()
{
	ECHORequest request;
	request.icmpHeader.Type=8;
	request.icmpHeader.Code=0;
	request.icmpHeader.Checksum=0;
	request.ID=getpid();
	request.SeqNumber=getpid();
	int error;
	error = snprintf(request.payload, 64,"%s", "czhQcNOEMqbWMMrCadhKsUavCWatwGehkdYFllUYSXwBDYcKkXQKcLKJCNqZ");
	if (error < 0)
	{
		perror("ICMP request sprintf()");
		exit(EXIT_FAILURE);
	}
	return request;
}

/**
 * Función para crear los paquetes ack que se enviarán al servidor.
 * block_number número de bloque
 */
unsigned char *ackPackage(int block_number)
{
	package_size = 0;
	unsigned char *package;
	if ((package = (unsigned char *)calloc(4, sizeof(unsigned char))) == 0)
	{
		perror("Fallo al reservar memoria para el paquete ACK");
		exit(EXIT_FAILURE);
	}
	package[1] = 4;
	package_size = 2;
	//Posible mejora sacar ese código a una función
	package[2] = block_number / 256;
	package[3] = block_number % 256;
	package_size += 2;
	return package;
}

/**
 * Función para crear los paquetes de datos, antes tb escribía los datos en la misma función pero lo saque por errores.
 * Una mejora sería escribir tb los datos dentro.
 */
unsigned char *dataPackage(int block_number)
{
	package_size = 0;
	unsigned char *package;
	if ((package = (unsigned char *)calloc(516, sizeof(unsigned char))) == 0)
	{
		perror("Fallo al reservar memoria para el paquete de datos");
		exit(EXIT_FAILURE);
	}
	package[1] = 3;
	package_size = 2;
	//Posible mejora sacar ese código a una función
	package[2] = block_number / 256;
	package[3] = block_number % 256;
	package_size += 2;
	return package;
}
