#include <kernel_utils.h>
#include <commons/config.h>
#include <pthread.h>

void conectar_cpu(void);
void conectar_memoria(void);
int recibir_mensajes(int);

int main(int argc, char *argv[])
{
    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    conectar_memoria();

    int socketKernel = iniciar_servidor_kernel(logger);

    while (1)
    {
        int socketCliente = esperar_cliente(socketKernel);
        pthread_t hiloCliente;

        pthread_create(&hiloCliente, NULL, (void *)recibir_mensajes, (void *)socketCliente);
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

    char *nombre = strdup("Kernel");
    enviar_mensaje(nombre, socketKernelCliente);

    // liberar_conexion_con_servidor(socketKernelCliente);
}

int recibir_mensajes(int socketCliente)
{

    t_log *logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    char *mensaje;
    while (true)
    {
        cod_op_cliente codOp = obtener_codigo_operacion(socketCliente);
        switch (codOp)
        {
        case MENSAJE_CLIENTE:
            mensaje = obtener_mensaje(socketCliente);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;
        case DESCONEXION_CLIENTE:
            apagar_servidor_kernel(socketCliente, logger);
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operación desconocida.");
            break;
        }
    }
    log_destroy(logger);
}
