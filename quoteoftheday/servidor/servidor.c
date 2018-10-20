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
#define MAXLENGTH 482

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

    char data_out[512] = "";
    char data_in[512] = "";
    static char buffQuote[MAXLENGTH] = "";
    struct sockaddr_in remote_addr;
    while (1)
    {
        if (!strcpy(data_out, "Quote Of The Day from vm2511:\n"))
        {
            perror("strcpy()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(-1);
            }
            exit(EXIT_FAILURE);
        }

        if (system("/usr/games/fortune -s > /tmp/tt.txt") == -1)
        {
            perror("system()");
            exit(EXIT_FAILURE);
        }

        socklen_t len = sizeof(remote_addr);
        error = recvfrom(id_sock, &data_in, 512, 0, (struct sockaddr *)&remote_addr, &len);
        if (error < 0)
        {
            perror("recvfrom()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(-1);
            }
            exit(-1);
        }

        printf("Mensaje de la ip; %s\n", inet_ntoa(remote_addr.sin_addr));
        printf("\033[1;32mMensaje:\033[0m %s\n", data_in);

        FILE *fich = fopen("/tmp/tt.txt", "r");
        if (!fich)
        {
            perror("fopen()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(-1);
            }
            exit(EXIT_FAILURE);
        }
        int nc = 0;
        char aux;
        aux = fgetc(fich);
        do
        {
            if (ferror(fich) != 0)
            {
                perror("fgetc()");
                error = close(id_sock);
                if (error < 0)
                {
                    perror("close()");
                    exit(-1);
                }
                exit(EXIT_FAILURE);
            }
            buffQuote[nc++] = aux;
            aux = fgetc(fich);
        } while (nc < MAXLENGTH - 1 && aux != EOF);

        if (fclose(fich) == EOF)
        {
            perror("fclose()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(-1);
            }
            exit(EXIT_FAILURE);
        }
        if (!strcat(data_out, buffQuote))
        {
            perror("strcpy()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(-1);
            }
            exit(EXIT_FAILURE);
        }

        error = sendto(id_sock, data_out, sizeof(data_out), 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
        if (error < 0)
        {
            perror("sendto()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(-1);
            }
            exit(-1);
        }
    }

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