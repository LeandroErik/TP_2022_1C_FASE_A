#include <kernel_utils.h>

int main(void)
{
  idProcesoGlobal = 0;

  Config *config = config_create("Kernel.config");
  Logger *logger = iniciar_logger_kernel();

  rellenar_configuracion_kernel(config);

  log_info(logger, "Iniciando Servidor Kernel...");
  int socketKernel = iniciar_servidor_kernel();

  if (socketKernel < 0)
  {
    log_error(logger, "Error intentando iniciar Servidor Kernel.");
    return EXIT_FAILURE;
  }

  log_info(logger, "Servidor Kernel iniciado correctamente.");

  Hilo hiloConsolas;
  Hilo hiloConexionInterrupt;
  Hilo hiloConexionMemoria;

  pthread_create(&hiloConsolas, NULL, (void *)esperar_consola, (void *)socketKernel);
  pthread_create(&hiloConexionInterrupt, NULL, (void *)manejar_conexion_cpu_interrupcion, NULL);
  pthread_create(&hiloConexionMemoria, NULL, (void *)manejar_conexion_memoria, NULL);

  pthread_join(hiloConsolas, NULL);
  pthread_join(hiloConexionInterrupt, NULL);
  pthread_join(hiloConexionMemoria, NULL);

  apagar_servidor(socketKernel);
  log_info(logger, "Servidor Kernel finalizado.");

  log_destroy(logger);
  config_destroy(config);

  return EXIT_SUCCESS;
}