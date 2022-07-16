#include <kernel_utils.h>
#include <signal.h>

void interprete_de_seniales(int senial)
{
    switch (senial)
    {
    case SIGINT:
        // Termino la ejecucion del kernel
        pthread_cancel(hilo_planificador_largo_plazo);
        pthread_cancel(hilo_planificador_corto_plazo);
        pthread_cancel(hilo_dispositivo_io);
        pthread_cancel(hiloConsolas);
        pthread_cancel(hiloConexionMemoria);
    }
}

int main(void)
{
    signal(SIGINT, interprete_de_seniales);

    idProcesoGlobal = 0;
    cantidadProcesosEnMemoria = 0;

    Config *config = config_create("Kernel.config");
    logger = iniciar_logger_kernel();
    loggerPlanificacion = log_create("Kernel-Planificacion.log", "Kernel", 1, LOG_LEVEL_INFO);

    rellenar_configuracion_kernel(config);

    socketKernel = iniciar_servidor_kernel();

    log_info(logger, "Conectando con Servidor Memoria.");
    socketMemoria = conectar_con_memoria();
    log_info(logger, "Conectando con Servidor CPU-Dispatch.");
    socketDispatch = conectar_con_cpu_dispatch();
    log_info(logger, "Conectando con Servidor CPU-Interrupt.");
    socketInterrupt = conectar_con_cpu_interrupt();

    inicializar_semaforos();
    inicializar_colas_procesos();
    iniciar_planificadores();

    pthread_create(&hiloConsolas, NULL, (void *)esperar_consola, (void *)socketKernel);
    pthread_create(&hiloConexionMemoria, NULL, (void *)manejar_conexion_memoria, NULL);

    pthread_join(hiloConsolas, NULL);

    pthread_join(hiloConexionMemoria, NULL);

    liberar_estructuras();
    liberar_semaforos();
    liberar_conexiones();

    log_destroy(logger);
    log_destroy(loggerPlanificacion);

    config_destroy(config);

    return EXIT_SUCCESS;
}