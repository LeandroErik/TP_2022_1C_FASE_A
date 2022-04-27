#include <kernel_utils.h>
#include <commons/config.h>
#include <pthread.h>

void conectar_cpu_dispatch(char *mensaje);
void conectar_cpu_interrupt(char *mensaje);

int main(int argc, char *argv[])
{
    id_proceso_total = 0;
    cargar_configuracion();

    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    pthread_t hiloConexionDispatch;
    pthread_t hiloConexionInterrupt;

    pthread_create(&hiloConexionDispatch, NULL, (void *)conectar_cpu_dispatch, (void *)"Soy Kernel a CPU Dispatch");
    pthread_join(hiloConexionDispatch, NULL);

    pthread_create(&hiloConexionInterrupt, NULL, (void *)conectar_cpu_interrupt, (void *)"Soy Kernel a CPU Interrupt");
    pthread_join(hiloConexionInterrupt, NULL);

    int socketKernel = iniciar_servidor_kernel(logger);
    pthread_t hiloEscucha;

    pthread_create(&hiloEscucha, NULL, (void *)iniciar_escucha, (void *)socketKernel);
    pthread_join(hiloEscucha, NULL);

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
