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

#define PROGRAM_NAME "qotd-tcp-client-Renero-Balgañon"

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
        aux = getservbyname("qotd", "udp");
        // Si el resultado es NULL imprimimos el mensaje de error
        if (!aux)
        {
            portError();
        }
        server_port = aux->s_port;
    }
    // Fin bloque datos de entrada

    //Bloque del socket
    int id_sock;
    //Creamos el socket y comprobamos los posibles errores
    id_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (id_sock == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    // Imprimo el id del socket para dar un feedback al usuario
    printf("Id de socket %d\n\n", id_sock);
    // Fin bloque del socket

    // // Bloque de bind
    int error;
    // struct sockaddr_in local_addr;
    // local_addr.sin_family = AF_INET;
    // local_addr.sin_port = 0;
    // local_addr.sin_addr.s_addr = INADDR_ANY;
    // // Hacemos el bind con el puerto y comprobamos los errores
    // error = bind(id_sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
    // if (error < 0)
    // {
    //     perror("bind()");
    //     // En el caso de que el bind falle el socket se queda encendido, por eso hay que cerrarlo
    //     error = close(id_sock);
    //     if (error < 0)
    //     {
    //         perror("close()");
    //         exit(EXIT_FAILURE);
    //     }
    //     exit(EXIT_FAILURE);
    // }
    // // Fin bloque de bind

    // Bloque de connect
    // Información del servidor
    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = server_port;
    remote_addr.sin_addr = server_ip;

    error = connect(id_sock, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
    if (error < 0)
    {
        perror("connect()");
        // En el caso de que el bind falle el socket se queda encendido, por eso hay que cerrarlo
        error = close(id_sock);
        if (error < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    // Fin bloque connect

    // // Bloque de sendto
    // // Datos que mandaremos al servidor
    // char data_out[] = "Some random data";
    // struct sockaddr_in remote_addr;
    // remote_addr.sin_family = AF_INET;
    // remote_addr.sin_port = server_port;
    // remote_addr.sin_addr = server_ip;
    // // Llamos a la directiva sendto para enviar los datos, en el caso de que falle cerramos el socket e imprimimos el error
    // error = sendto(id_sock, data_out, sizeof(data_out), 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
    // if (error < 0)
    // {
    //     perror("sendto()");
    //     error = close(id_sock);
    //     if (error < 0)
    //     {
    //         perror("close()");
    //         exit(EXIT_FAILURE);
    //     }
    //     exit(EXIT_FAILURE);
    // }
    // // Muestro por pantalla lo que he mandado, los elementos \033[1;32m y \033[0m son para imprimir con colores
    // printf("\033[1;32mCliente:\033[0m %s\n", data_out);
    // // Fin de sendto

    // Bloque de recv
    char data_in[512];
    // Recibo los datos solicitados al servidor comprobando posibles errores
    error = recv(id_sock, &data_in, 512, 0);
    if (error < 0)
    {
        perror("recv()");
        error = close(id_sock);
        if (error < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }
    // Muestro por consola lo que manda el servidor
    printf("\033[1;32mServidor:\033[0m %s\n", data_in);
    // Fin bloque de recv

    // close
    // Cerramos conexión con el servidor
    error = shutdown(id_sock, SHUT_RDWR);
    if (error < 0)
    {
        perror("shutdown()");
        error = close(id_sock);
        if (error < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    // recv para comprobar que el cliente se ha enterado.
    error = recv(id_sock, &data_in, 0, 0);
    if (error < 0)
    {
        perror("recv()");
        error = close(id_sock);
        if (error < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    // Cierro el socket
    error = close(id_sock);
    if (error < 0)
    {
        perror("close()");
        exit(EXIT_FAILURE);
    }
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
