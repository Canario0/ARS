// Practica tema 6, Renero Balgañón Pablo
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#define PROGRAM_NAME "qotd-tcp-server-Renero-Balgañon"
#define MAXLENGTH 512

void output(int const pos, char const *argv[], const int total);
void paramError();
void ayuda();
void portError();
void setPort(const int port);
void signal_handler(int signal);
void childAction(struct sockaddr_in remote_addr);

// Variables Globales
uint16_t local_port;
int id_sock;
int fork_id;

int main(int argc, char const *argv[])
{
    // Bloque datos de entrada
    // Compruebo que si hay parametros de entrada, no pueden ser más de dos
    if ((argc - 1) > 2)
    {
        paramError();
    }

    // Llamo a la función para procesar los datos o en el caso de no tener parámetros establezco el puerto por defecto de qotd
    if ((argc - 1) <= 2 && (argc - 1) > 0)
    {
        output(1, argv, argc - 1);
    }
    else
    {
        struct servent *aux;
        aux = getservbyname("qotd", "tcp");
        if (!aux)
        {
            portError();
        }
        local_port = aux->s_port;
    }
    // Fin bloque datos de entrada

    // Bloque del socket

    // Creamos el socket y comprobamos los posibles errores
    id_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (id_sock == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    // Imprimo el id del socket para dar un feedback al usuario
    printf("Id de socket %d\n\n", id_sock);
    // Fin bloque del socket

    // Bloque del signal
    if (signal(SIGINT, &signal_handler) == SIG_ERR)
    {
        perror("signal()");
    }
    // Fin bloque del signal

    // Bloque de bind
    int error;
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = local_port;
    local_addr.sin_addr.s_addr = INADDR_ANY;

    // Hacemos el bind con el puerto y comprobamos los errores
    error = bind(id_sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if (error < 0)
    {
        perror("bind()");
        // En el caso de que el bind falle el socket se queda encendido, por eso hay que cerrarlo
        error = close(id_sock);
        if (error < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }
    // Fin bloque de bind

    // Bloque de listen

    listen(id_sock, 5);

    // Bloque de escucha
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(remote_addr);
    int new_sock;
    while (1)
    {
        new_sock = accept(id_sock, (struct sockaddr *)&remote_addr, &len);
        if (new_sock < 0)
        {
            perror("accept()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        fork_id = fork();
        if (fork_id < 0)
        {
            perror("fork()");
            error = close(id_sock);
            if (error < 0)
            {
                perror("close()");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        if (fork_id == 0)
        {
            id_sock = new_sock;
            childAction(remote_addr);
            exit(0);
        }
    }
    // Fin bloque de escucha
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
        if (total != 2)
            paramError();
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
 * Función que imprime la ayuda desplegada con la opción -h
 */
void ayuda()
{
    printf("\nUso: ./%s [-p puerto-servidor]\n\n", PROGRAM_NAME);
    printf("Opciones:\n\t-p\n\t   Si no se proporciona un número de puesto se usará el puerto por defecto del servicio Quote of the Day.\n\n");
    exit(0);
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
    local_port = htons(port);
}

/**
 * Función para manejar ctrl+C
 */
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        if (close(id_sock) < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
	exit(0);
    }
}

void childAction(struct sockaddr_in remote_addr)
{
    // Buffer de salida
    char data_out[MAXLENGTH] = "";
    // llamamos al sistema para ejecutar el comando fortune y aprovechamos para concatenar todo con las redirecciones de bash
    if (system("/usr/bin/echo 'Quote Of The Day from vm2511:' > /tmp/tt.txt;/usr/games/fortune -s >> /tmp/tt.txt") == -1)
    {
        perror("system()");
        exit(EXIT_FAILURE);
    }
    // Mostramos la ip del cliente y los datos recibidos
    printf("\033[1;32mConexion from:\033[0m %s\n", inet_ntoa(remote_addr.sin_addr));
    fflush(stdout);
    // leemos el quote hasta llenar 482 caracteres o el fin de fichero
    FILE *fich = fopen("/tmp/tt.txt", "r");
    if (!fich)
    {
        perror("fopen()");
        if (close(id_sock) < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
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
            if (close(id_sock) < 0)
            {
                perror("close()");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        data_out[nc++] = aux;
        aux = fgetc(fich);
    } while (nc < MAXLENGTH - 1 && aux != EOF);

    // cerramos el fichero
    if (fclose(fich) == EOF)
    {
        perror("fclose()");
        if (close(id_sock) < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    //mandamos el quote
    if (send(id_sock, data_out, sizeof(data_out), 0) < 0)
    {
        perror("sendto()");
        if (close(id_sock) < 0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    // Cerramos el socket como el hijo sustituye el valor del id_sock no es necesarario pasarselo por parámetro
    // close
    if (close(id_sock) < 0)
    {
        perror("close()");
        exit(EXIT_FAILURE);
    }
}
