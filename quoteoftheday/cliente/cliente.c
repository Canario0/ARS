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
    if (argc <= 1)
    {
        noParamError();
    }

    if ((argc - 1) > 1 && (argc - 1) != 3)
    {
        paramError();
    }

    output(1, argv, argc - 1);

    if (argc - 1 == 3)
    {
        output(2, argv, argc - 1);
    }
    else
    {
        struct servent *aux;
        aux = getservbyname("qotd", "udp");
        if (!aux)
        {
            portError();
        }
        server_port = aux->s_port;
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
    local_addr.sin_port = 0;
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

    //Bloque de sendto
    char data_out[] = "Some random data";
    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = server_port;
    remote_addr.sin_addr = server_ip;
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
    printf("\033[1;32mCliente:\033[0m %s\n", data_out);
    //Fin de sendto

    //Bloque de recvfrom
    char data_in[512];
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
    printf("\033[1;32mServidor:\033[0m %s\n", data_in);
    //Fin bloque de recvfrom

    //close
    error = close(id_sock);
    if (error < 0)
    {
        perror("close()");
        exit(-1);
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
        ayuda();
    }
    else if (strcmp(argv[pos], "-p") == 0)
    {
        // printf("Esto es el puerto\n");
        int port;
        if (sscanf(argv[pos + 1], "%d", &port) != 1)
        {
            portError();
        }
        else
        {
            setPort(port);
        }
    }
    else
    {
        if (pos != 1)
        {
            paramError();
        }

        if (inet_aton(argv[pos], &server_ip) == 0)
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

void portError()
{
    printf("\nEl puerto no es válido o no se ha podido encontrar\n\n");
    exit(-1);
}

void setPort(const int port)
{
    server_port = htons(port);
}