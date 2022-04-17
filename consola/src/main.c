#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

int main(int argc, char *argv[])
{

    char *ipKernel = "127.0.0.1";
    char *puertoKernel = "5000";

    int socketConsola = crear_conexion(ipKernel, puertoKernel);

    char *input = readline("> ");

    while (strcmp(input, ""))
    {
        enviar_mensaje(input, socketConsola);
        free(input);
        input = readline("> ");
    }

    liberar_conexion(socketConsola);

    return 0;
}
