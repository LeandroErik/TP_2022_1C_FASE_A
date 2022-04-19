#include <kernel_utils.h>

int main(int argc, char *argv[])
{
    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    int socketKernel = iniciar_servidor_kernel(logger);
    int socketConsola = obtener_socket_consola(socketKernel, logger);
    char *mensaje;

    while (true)
    {
        cod_op codOp = obtener_codigo_operacion(socketConsola);
        switch (codOp)
        {
        case MENSAJE:
            mensaje = obtener_mensaje(socketConsola);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;

        case DESCONEXION:
            apagar_servidor_kernel(socketKernel, logger);
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operación desconocida.");
            break;
        }
    }
    log_destroy(logger);

    return EXIT_SUCCESS;
}

void conectar_cpu(void) {
    t_config *config = iniciar_config("kernel.config");

    char *ipCpu = config_get_string_value(config, "IP_CPU");
    char *puertoCpu = config_get_string_value(config, "PUERTO_CPU");

    int socketKernelCliente = crear_conexion(ipCpu, puertoCpu);
    printf("\n %d %s %s \n", socketKernelCliente, ipCpu, puertoCpu);

    enviar_mensaje("soy kernel, pa", socketKernelCliente);

    liberar_conexion(socketKernelCliente);
}