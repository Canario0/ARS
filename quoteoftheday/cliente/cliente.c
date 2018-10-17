#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PROGRAM_NAME "qotd-udp-client-Renero-Balgañon"

void output(int const pos, char const *argv[], const int total);
void paramError();
void noParamError();
void ayuda();
void ipError(const char *in);

// Variables Globales
struct in_addr *server_ip;

int main(int argc, char const *argv[])
{
    // Bloque datos de entrada
    if (argc <= 1)
    {
        noParamError();
    }

    if ((argc - 1) > 1 && (argc - 1) != 3)
    {
        paramError();
    }

    output(1, argv, argc);
    if (argc - 1 == 3)
    {
        output(2, argv, argc);
    }
    //Fin bloque datos de entrada

    return 0;
}

void output(int const pos, char const *argv[], const int total)
{
    if (strcmp(argv[pos], "-h") == 0)
    {
        if (pos != 1 || total != 1)
        {
            paramError();
        }
        ayuda();
    }
    else if (strcmp(argv[pos], "-p") == 0)
    {
        printf("Esto es el puerto\n");
    }
    else
    {
        if (pos != 1)
        {
            paramError();
        }

        if (inet_aton(argv[pos], server_ip) == 0)
        {
            ipError(argv[pos]);
        }
    }
}
void paramError()
{
    printf("Número de parámetros erroneo\nPruebe './%s -h' para más información.\n", PROGRAM_NAME);
    exit(-1);
}

void noParamError()
{
    printf("Cliente: debe indicar almenos una direción IP\nPruebe './%s -h' para más información.\n", PROGRAM_NAME);
    exit(-1);
}

void ayuda()
{
    printf("\nUso: ./%s direccion.IP.servidor [-p puerto-servidor]\n\n", PROGRAM_NAME);
    printf("Opciones:\n\t-p\n\t   Si no se proporciona un número de puesto se usará el puerto por defecto del servicio Quote of the Day.\n\n");
    exit(0);
}

void ipError(const char *in)
{
    printf("\nLa ip: '%s' no es válidad\n\n", in);
    exit(-1);
}