#include <kernel_utils.h>
#include <commons/config.h>
#include <pthread.h>

void conectar_memoria(void);
void conectar_cpu_dispatch(char *mensaje);
void conectar_cpu_interrupt(char *mensaje);

int main(int argc, char *argv[])
{
    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    // conectar_cpu_dispatch();
    // conectar_cpu_interrupt();
    pthread_t hiloConexionDispatch;
    pthread_t hiloConexionInterrupt;

    pthread_create(&hiloConexionDispatch, NULL, (void *)conectar_cpu_dispatch, (void *)"Soy Kernel a CPU Dispatch");
    pthread_join(hiloConexionDispatch, NULL);

    pthread_create(&hiloConexionInterrupt, NULL, (void *)conectar_cpu_interrupt, (void *)"Soy Kernel a CPU Interrupt");
    pthread_join(hiloConexionInterrupt, NULL);
    /* int socketKernel = iniciar_servidor_kernel(logger);
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
    } */
    log_destroy(logger);

    return EXIT_SUCCESS;
}

void conectar_cpu_dispatch(char *mensaje)
{
    int socketKernelClienteDispatch = crear_conexion_con_cpu_dispatch();

    enviar_mensaje(mensaje, socketKernelClienteDispatch);
    liberar_conexion_con_servidor(socketKernelClienteDispatch);
}

void conectar_cpu_interrupt(char *mensaje)
{
    int socketKernelClienteInterrupt = crear_conexion_con_cpu_interrupt();

    enviar_mensaje(mensaje, socketKernelClienteInterrupt);
    liberar_conexion_con_servidor(socketKernelClienteInterrupt);
}

void conectar_memoria(void)
{
    int socketKernelCliente = crear_conexion_con_memoria();

    enviar_mensaje("soy kernel, envio un mensaje al modulo Memoria", socketKernelCliente);

    liberar_conexion_con_servidor(socketKernelCliente);
}