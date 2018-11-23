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

    // Compruebo que si existen más de un parámetro de entrada tienen que ser tres en total
    if ((argc - 1) > 1 && (argc - 1) != 3)
    {
        paramError();
    }

    // Llamo a la función que parsea los datos con el primer elemento
    output(1, argv, argc - 1);

    if (argc - 1 == 3)
    {
        // En el caso de que existan 3 elementos llamo a la función con el valor 2 en la variable pos
        output(2, argv, argc - 1);
    }
    else
    {
        // En el caso de que no tenga 3 parámetros quiere decir que el puerto que vamos usar es el por defecto del protocolo qotd
        struct servent *aux;
        aux = getservbyname("tftp", "udp");
        // Si el resultado es NULL imprimimos el mensaje de error
        if (!aux)
        {
            portError();
        }
	printf("El puerto de TFTP es: %u\n", ntohs(aux->s_port));
        server_port = aux->s_port;
    }
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
        //En el caso de que la opción -h no esté en la pos 1 o tenga más argumentos detras se lanza un error de entrada
        if (pos != 1 || total != 1)
        {
            paramError();
        }
        ayuda();
    }
    else if (strcmp(argv[pos], "-p") == 0)
    {
        //En el caso de que el puerto no vaya seguido de un elemento o el elemento no sea un número se lanza un error
        int port;
        if (sscanf(argv[pos + 1], "%d", &port) != 1)
        {
            portError();
        }
        else
        {
            //Función que establece el puerto
            setPort(port);
        }
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
    printf("Número de parámetros erroneo\nPruebe './%s -h' para más información.\n", PROGRAM_NAME);
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
    printf("\nUso: ./%s direccion.IP.servidor [-p puerto-servidor]\n\n", PROGRAM_NAME);
    printf("Opciones:\n\t-p\n\t   Si no se proporciona un número de puesto se usará el puerto por defecto del servicio Quote of the Day.\n\n");
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
