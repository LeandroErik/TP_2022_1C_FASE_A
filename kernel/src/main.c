#include <kernel_utils.h>
#include <commons/config.h>
#include <pthread.h>

void conectar_cpu_dispatch(char *mensaje);
void conectar_cpu_interrupt(char *mensaje);
void escuchar_a_cpu_dispatch();

int main(int argc, char *argv[])
{
    id_proceso_total = 0;

    cargar_configuracion();

    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    pthread_t hiloConexionDispatch;
    // pthread_t hiloConexionInterrupt;

    pthread_create(&hiloConexionDispatch, NULL, (void *)escuchar_a_cpu_dispatch, NULL);

    pthread_join(hiloConexionDispatch, NULL);

    // pthread_create(&hiloConexionInterrupt, NULL, (void *)conectar_cpu_interrupt, (void *)"Soy Kernel a CPU Interrupt");
    // pthread_join(hiloConexionInterrupt, NULL);

    int socketKernel = iniciar_servidor_kernel(logger);
    socketKernelClienteDispatch = crear_conexion_con_cpu_dispatch();

    inicializar_semaforos();
    inicializar_colas_procesos();
    iniciar_planificadores();
    pthread_t hiloEscucha;

    pthread_create(&hiloEscucha, NULL, (void *)iniciar_escucha, (void *)socketKernel);
    pthread_join(hiloEscucha, NULL);

    log_destroy(logger);

    return EXIT_SUCCESS;
}

void escuchar_a_cpu_dispatch()
{

    int socketCliente = esperar_cliente(socketKernelClienteDispatch);
    log_info(logger, "Se conectó el CPU Dispatch...");

    char *mensaje;

    while (true)
    {
        cod_op codOp = obtener_codigo_operacion(socketCliente);

        switch (codOp)
        {
        case MENSAJE_CLIENTE_P:
            mensaje = obtener_mensaje(socketCliente);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;

        case DESCONEXION_CLIENTE_P:
            log_info(logger, "Se desconectó el CPU Dispatch... %d", codOp);

            break;

        default:
            log_warning(logger, "Operación desconocida.");
            break;
        }
    }
    // recibir_mensajes(socketKernelClienteDispatch);
}

void conectar_cpu_interrupt(char *mensaje)
{
    int socketKernelClienteInterrupt = crear_conexion_con_cpu_interrupt();

    enviar_mensaje(mensaje, socketKernelClienteInterrupt);
    liberar_conexion_con_servidor(socketKernelClienteInterrupt);
}

void agregar_lista_a_paquete(t_paquete *paquete, pcb *proceso)
{
    int tamanio_lista = list_size(proceso->lista_instrucciones);
    agregar_a_paquete(paquete, &(tamanio_lista), sizeof(int));

    for (int i = 0; i < tamanio_lista; i++)
    {
        char *instruccion = malloc(strlen(list_get(proceso->lista_instrucciones, i)) + 1);

        instruccion = list_get(proceso->lista_instrucciones, i);

        agregar_a_paquete(paquete, instruccion, strlen(instruccion) + 1);
    }
}
