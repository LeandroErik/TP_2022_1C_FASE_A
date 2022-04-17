#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <string.h>
#include <commons/log.h>
#include "utils.h"

int main(int argc, char *argv[])
{
    logger = log_create("log.log", "Kernel", 1, LOG_LEVEL_DEBUG);

    puts("Hello world!!");

    int socketKernel = iniciar_servidor();
    log_info(logger, "Servidor listo para recibir al cliente");

    int socketConsola = esperar_cliente(socketKernel);

    conectar_cpu();

    while (1)
    {
        int codOp = recibir_operacion(socketConsola);
        switch (codOp)
        {
        case MENSAJE:
            recibir_mensaje(socketConsola);
            break;

        case -1:
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operacion desconocida. No quieras meter la pata");
            break;
        }
    }
    log_destroy(logger);

    return 0;
}

void conectar_cpu(void) {
    t_config *config = iniciar_config("kernel.config");

    char *ipCpu = config_get_string_value(config, "IP_CPU");
    char *puertoCpu = config_get_string_value(config, "PUERTO_CPU");

    int socketKernel = crear_conexion(ipCpu, puertoCpu);

    //char *input = readline("> ");
    enviar_mensaje("soy kernel, pa", socketKernel);

    /*while (strcmp(input, ""))
    {
        enviar_mensaje(input, socketKernel);
        free(input);
        input = readline("> ");
    } */

    liberar_conexion(socketKernel);
}