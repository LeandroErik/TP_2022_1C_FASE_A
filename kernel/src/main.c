#include <kernel_utils.h>
#include <commons/config.h>
#include <pthread.h>

void conectar_cpu_dispatch(char *mensaje);
void conectar_cpu_interrupt(char *mensaje);
void crear_pcb(pcb *);
void enviar_pcb(pcb *, int, t_log *);
void agregar_lista_a_paquete(t_paquete *, pcb *);

int main(int argc, char *argv[])
{
    id_proceso_total = 0;

    cargar_configuracion();

    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    // int socketKernelClienteDispatch = crear_conexion_con_cpu_dispatch();

    // pcb proceso;
    // crear_pcb(&proceso);
    // enviar_pcb(&proceso, socketKernelClienteDispatch, logger);
    // log_info(logger, "PCB enviado");

    // liberar_conexion_con_servidor(socketKernelClienteDispatch);

    // pthread_t hiloConexionDispatch;
    // pthread_t hiloConexionInterrupt;

    // pthread_create(&hiloConexionDispatch, NULL, (void *)conectar_cpu_dispatch, (void *)"Soy Kernel a CPU Dispatch");
    // pthread_join(hiloConexionDispatch, NULL);

    // pthread_create(&hiloConexionInterrupt, NULL, (void *)conectar_cpu_interrupt, (void *)"Soy Kernel a CPU Interrupt");
    // pthread_join(hiloConexionInterrupt, NULL);
    inicializar_semaforos();
    inicializar_colas_procesos();

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

void enviar_pcb(pcb *proceso, int socketCPU, t_log *logger)
{
    t_paquete *paquete = crear_paquete(IMAGEN_PCB_P);

    agregar_a_paquete(paquete, &(proceso->pid), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->tamanio), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->proxima_instruccion), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->tabla_de_paginas), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->estimacion_rafaga), sizeof(float));

    agregar_lista_a_paquete(paquete, proceso);

    enviar_paquete(paquete, socketCPU);

    eliminar_paquete(paquete);
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
