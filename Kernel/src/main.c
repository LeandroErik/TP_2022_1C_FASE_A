#include <kernel_utils.h>

int main(void)
{
    idProcesoGlobal = 0;
    cantidadProcesosEnMemoria = 0;

    Config *config = config_create("Kernel.config");
    logger = iniciar_logger_kernel();
    loggerPlanificacion = log_create("Kernel-Planificacion.log", "Kernel", 1, LOG_LEVEL_INFO);

    rellenar_configuracion_kernel(config);

    int socketKernel = iniciar_servidor_kernel();

    log_info(logger, "Conectando con Servidor Memoria.");
    socketMemoria = conectar_con_memoria();
    log_info(logger, "Conectando con Servidor CPU-Dispatch.");
    socketDispatch = conectar_con_cpu_dispatch();

    inicializar_semaforos();
    inicializar_colas_procesos();
    iniciar_planificadores();

    Hilo hiloConsolas;
    Hilo hiloConexionMemoria;

    pthread_create(&hiloConsolas, NULL, (void *)esperar_consola, (void *)socketKernel);
    pthread_create(&hiloConexionMemoria, NULL, (void *)manejar_conexion_memoria, NULL);

    pthread_join(hiloConsolas, NULL);
    pthread_join(hiloConexionMemoria, NULL);

    liberar_estructuras();
    liberar_semaforos();

    apagar_servidor(socketKernel);
    log_info(logger, "Servidor Kernel finalizado.");

    liberar_conexion_con_servidor(socketMemoria);
    log_info(logger, "Saliendo del Servidor Memoria...");

    liberar_conexion_con_servidor(socketDispatch);
    log_info(logger, "Saliendo del Servidor CPU-Dispatch...");

    log_destroy(logger);
    config_destroy(config);

    return EXIT_SUCCESS;
}