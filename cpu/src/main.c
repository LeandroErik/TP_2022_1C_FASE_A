#include <cpu_utils.h>

int main(void)
{
  Config *config = config_create("CPU.config");
  Logger *logger = iniciar_logger_cpu();

  rellenar_configuracion_cpu(config);

  log_info(logger, "Iniciando Servidor CPU...");
  int socketCpuDispatch = iniciar_servidor_cpu_dispatch();
  int socketCpuInterrupt = iniciar_servidor_cpu_interrupt();

  if (socketCpuDispatch < 0 || socketCpuInterrupt < 0)
  {
    log_error(logger, "No se pudo iniciar el Servidor CPU.");
    log_destroy(logger);
    return EXIT_FAILURE;
  }

  Hilo hiloKernelDispatch;
  Hilo hiloKernelInterrupt;
  Hilo hiloConexionMemoria;

  pthread_create(&hiloKernelDispatch, NULL, (void *)esperar_kernel_dispatch, (void *)socketCpuDispatch);
  pthread_create(&hiloKernelInterrupt, NULL, (void *)esperar_kernel_interrupt, (void *)socketCpuInterrupt);
  pthread_create(&hiloConexionMemoria, NULL, (void *)manejar_conexion_memoria, NULL);

  pthread_join(hiloKernelDispatch, NULL);
  pthread_join(hiloKernelInterrupt, NULL);
  pthread_join(hiloConexionMemoria, NULL);

  log_destroy(logger);
  config_destroy(config);
}
