#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "qotd-udp-client-Renero-Balgañon"

void output(int const pos, char const *argv[], const int total);
void paramError();
void noParamError();

int main(int argc, char const *argv[])
{
    if (argc <= 1)
    {
        noParamError();
    }

    if (strcmp(argv[1], "-h") == 0)
    {
        output(1, argv, argc - 1);
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
        printf("\nUso: ./%s direccion.IP.servidor [-p puerto-servidor]\n\n", PROGRAM_NAME);
        printf("Opciones:\n\t-p\n\t   Si no se proporciona un número de puesto se usará el puerto por defecto del servicio Quote of the Day.\n\n");
        exit(0);
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
        printf("Check IP\n");
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