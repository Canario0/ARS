#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if ((argc - 1) % 2 != 0 | (argc > 5))
    {
        paramError();
    }

    int i;
    for (i = 1; i < argc; i += 2)
    {
        output(i, argv, argc);
    }

    return 0;
}

void output(int const pos, char const *argv[], const int total)
{
    if (strcmp(argv[pos], "-h") == 0)
    {
        if (pos != 1 | total != 1)
        {
            paramError();
        }
        printf("Esto es la ayuda\n");
        exit(0);
    }
    else if (strcmp(argv[pos], "-ip") == 0)
    {
        printf("Esto es la IP\n");
    }
    else if (strcmp(argv[pos], "-p") == 0)
    {
        printf("Esto es el puerto\n");
    }
    else
    {
        paramError();
    }
}
void paramError()
{
    printf("Número de parámetros erroneo\nPruebe './cliente -h' para más información.\n");
    exit(-1);
}

void noParamError()
{
    printf("Cliente: debe indicar almenos una direción IP\nPruebe './cliente -h' para más información.\n");
    exit(-1);
}