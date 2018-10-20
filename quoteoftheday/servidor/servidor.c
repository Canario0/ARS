// Practica tema 5, Renero Balgañón Pablo
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PROGRAM_NAME "qotd-udp-server-Renero-Balgañon"
#define MAXLENGTH 100

void output(int const pos, char const *argv[], const int total);
void paramError();
void ayuda();
void ipError(const char *in);
void portError();
void setPort(const int port);

// Variables Globales
uint16_t local_port;

int main(int argc, char const *argv[])
{
    // Bloque datos de entrada
    if ((argc - 1) > 2)
    {
        paramError();
    }

    if ((argc - 1) <= 2 && (argc - 1) > 0)
    {
        output(1, argv, argc - 1);
    }
    else
    {
        struct servent *aux;
        aux = getservbyname("qotd", "udp");
        if (!aux)
        {
            portError();
        }
        local_port = aux->s_port;
    }
    //Fin bloque datos de entrada

    //Bloque del socket
    int id_sock;
    id_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (id_sock == -1)
    {
        perror("socket()");
        exit(-1);
    }
    printf("Id de socket %d\n\n", id_sock);
    //Fin bloque del socket

    //Bloque de bind
    int error;
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = local_port;
    local_addr.sin_addr.s_addr = INADDR_ANY;

    error = bind(id_sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if (error < 0)
    {
        perror("bind()");
        error = close(id_sock);
        if (error < 0)
        {
            perror("close()");
            exit(-1);
        }
        exit(-1);
    }
    //Fin bloque de bind

    //Bloque de escucha
    char data_out[512];
    // while (1)
    // {
        strcpy(data_out, "Quote Of The Day from vm2511:\n");
        printf("%s\n", data_out);
        char buffQuote[MAXLENGTH];
        system("/usr/games/fortune -s > /tmp/tt.txt");
        FILE *fich = fopen("/tmp/tt.txt", "r");
        int nc = 0;
        char aux;
        aux = fgetc(fich);
        do
        {
            buffQuote[nc++] = aux;
            aux = fgetc(fich);
        } while (nc < MAXLENGTH - 1 && aux != EOF);
        fclose(fich);
        strcat(data_out, buffQuote);
        printf("%s\n", data_out);
    // }

    //Fin bloque de escucha
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
        // printf("Esto es el puerto\n");
        int port;
        if (total != 2)
            paramError();
        if (sscanf(argv[pos + 1], "%d", &port) != 1)
        {
            portError();
        }
        else
        {
            setPort(port);
        }
    }
}
void paramError()
{
    printf("Número de parámetros erroneo\nPruebe './%s -h' para más información.\n", PROGRAM_NAME);
    exit(-1);
}

void ayuda()
{
    printf("\nUso: ./%s [-p puerto-servidor]\n\n", PROGRAM_NAME);
    printf("Opciones:\n\t-p\n\t   Si no se proporciona un número de puesto se usará el puerto por defecto del servicio Quote of the Day.\n\n");
    exit(0);
}

void ipError(const char *in)
{
    printf("\nLa ip: '%s' no es válidad\n\n", in);
    exit(-1);
}

void portError()
{
    printf("\nEl puerto no es válido o no se ha podido encontrar\n\n");
    exit(-1);
}

void setPort(const int port)
{
    local_port = htons(port);
}