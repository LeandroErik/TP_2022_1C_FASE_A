#include <kernel_utils.h>
#include <commons/config.h>

void conectar_cpu(void);
void conectar_memoria(void);

int main(int argc, char *argv[])
{
    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    conectar_cpu();
    int socketKernel = iniciar_servidor_kernel(logger);
    int socketConsola = obtener_socket_consola(socketKernel, logger);
    char *mensaje;

    // conectar_memoria();

    while (true)
    {
        cod_op_cliente codOp = obtener_codigo_operacion(socketConsola);
        switch (codOp)
        {
        case MENSAJE_CLIENTE:
            mensaje = obtener_mensaje(socketConsola);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;

        case DESCONEXION_CLIENTE:
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

void conectar_cpu(void)
{
    int socketKernelCliente = crear_conexion_con_cpu();

    enviar_mensaje("soy kernel, envio un mensaje al modulo CPU", socketKernelCliente);

    liberar_conexion_con_cpu(socketKernelCliente);
}

void conectar_memoria(void)
{
    int socketKernelCliente = crear_conexion_con_memoria();

    enviar_mensaje("soy kernel, envio un mensaje al modulo Memoria", socketKernelCliente);

    liberar_conexion_con_servidor(socketKernelCliente);
}