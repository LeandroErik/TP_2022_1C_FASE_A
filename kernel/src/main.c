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

    int socketKernelClienteDispatch = crear_conexion_con_cpu_dispatch();
    t_paquete *p = crear_paquete(IMAGEN_PCB);
    pcb proceso;

    proceso.lista_instrucciones = list_create();
    list_add(proceso.lista_instrucciones, "Instruccion 1");
    list_add(proceso.lista_instrucciones, "Instruccion 2");

    proceso.pid = 10;
    proceso.tamanio = 50;
    proceso.proxima_instruccion = 2;
    proceso.tabla_de_paginas = 11;
    proceso.estimacion_rafaga = 2.2;

    agregar_a_paquete(p, &(proceso.pid), sizeof(int));
    agregar_a_paquete(p, &(proceso.tamanio), sizeof(int));
    agregar_a_paquete(p, &(proceso.proxima_instruccion), sizeof(int));
    agregar_a_paquete(p, &(proceso.tabla_de_paginas), sizeof(int));
    agregar_a_paquete(p, &(proceso.estimacion_rafaga), sizeof(float));

    int tamanio_lista = list_size(proceso.lista_instrucciones);
    printf("TAM LISTA --> %i", tamanio_lista);

    agregar_a_paquete(p, &(tamanio_lista), sizeof(uint32_t));
    for (int i = 0; i < tamanio_lista; i++)
    {

        char *instruccion = malloc(strlen("Instruccion 1") + 1);

        instruccion = list_get(proceso.lista_instrucciones, i);
        log_info(logger, "Las intreccion %s", instruccion);
        agregar_a_paquete(p, instruccion, strlen(instruccion) + 1);
    }

    enviar_paquete(p, socketKernelClienteDispatch);

    log_info(logger, "Mensaje y paquete enviado");

    // free(persona.nombre);
    eliminar_paquete(p);
    liberar_conexion_con_servidor(socketKernelClienteDispatch);

    /*pthread_t hiloConexionDispatch;
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
    */
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
