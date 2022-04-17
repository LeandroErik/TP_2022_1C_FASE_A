#include <stdlib.h>
#include <stdio.h>
#include "consola_utils.h"

int main(int argc, char *argv[])
{
    t_config *config = iniciar_config("consola.config");

    char *ipKernel = config_get_string_value(config, "IP_KERNEL");
    ;
    char *puertoKernel = config_get_string_value(config, "PUERTO_KERNEL");
    ;

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
