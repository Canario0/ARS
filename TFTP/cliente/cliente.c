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

#define PROGRAM_NAME "qotd-udp-client-Renero-Balgañon"

void output(int const pos, char const *argv[], const int total);
void paramError();
void noParamError();
void ayuda();
void ipError(const char *in);
void portError();
void setPort(const int port);

// Variables Globales
struct in_addr server_ip;
uint16_t server_port;

int main(int argc, char const *argv[])
{
    // Bloque datos de entrada
    // Compruebo que hay parametros de entrada, si no los hay se imprime un mensaje avisando
    if (argc <= 1)
    {
        noParamError();
    }

    // Compruebo que si existen más de un parámetro de entrada tienen e ser tres o cuatro en total
    if ((argc - 1) > 1 && !((argc - 1) >=3 && (argc-1)<=4))
    {
        paramError();
    }

    // Llamo a la función que parsea los datos con el primer elemento
    int i =1;
    for(i, i<argc;i++)    
    output(i, argv, argc - 1);
    // Fin bloque datos de entrada
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
	    if(pos != 2)
		    paramError();
	    printf("Modo %s\n", argv[pos]);

    }
    else if (strcmp(argv[pos], "-v") == 0)
    {
	    if(pos!=4)
		    paramError();
	    printf("Modo vervoso");

    }
    else
    {
        //En el caso de que llegue aquí con una posición que no sea 1 se lanza un mensaje de error.
        if (pos != 1 && pos != 3 )
        {
            paramError();
        }
	if (pos == 1){
           //Traduce la ip y comprueba que es válida.
           if (inet_aton(argv[pos], &server_ip) == 0)
           {
               ipError(argv[pos]);
           }
	   printf("La ip es: %d\n", server_ip);

         } else {
	      if (strcmp(argv[pos-1], "-r") == 0 && strcmp(argv[pos-1], "-w") == 0)
		      paramError();
	      printf("Nombre del archivo %s \n" , argv[pos]);
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

/**
 * Función que imprime un mensaje de error si el puerto no es válido
 */
void portError()
{
    printf("\nEl puerto no es válido o no se ha podido encontrar\n\n");
    exit(EXIT_FAILURE);
}

/**
 * Función que traduce un puerto de número al valor de internet
 */
void setPort(const int port)
{
    server_port = htons(port);
}
