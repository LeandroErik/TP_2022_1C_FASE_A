#include <cpu_utils.h>

bool seNecesitaAtenderInterrupcion = false;

int main(int argc, char *argv[])
{
  Logger *logger = iniciar_logger_cpu();
  if (argc < 2)
  {
    log_error(logger, "Falta poner config.");
    return EXIT_FAILURE;
  }
  char *parametro = argv[1];
  Config *config = config_create(parametro);
  if (config == NULL)
  {
    log_error(logger, "No existe config %s", parametro);
    return EXIT_FAILURE;
  }
  tlb = list_create();
  pidAnterior = -1;

  rellenar_configuracion_cpu(config);

  log_info(logger, "Iniciando Servidor CPU...");
  int socketCpuDispatch = iniciar_servidor_cpu_dispatch();
  int socketCpuInterrupt = iniciar_servidor_cpu_interrupt();

  if (socketCpuDispatch < 0 || socketCpuInterrupt < 0)
  {
    log_error(logger, "Error intentando iniciar Servidor CPU.");
    log_destroy(logger);
    return EXIT_FAILURE;
  }

  Hilo hiloKernelDispatch;
  Hilo hiloKernelInterrupt;

  pthread_create(&hiloKernelDispatch, NULL, (void *)esperar_kernel_dispatch, (void *)socketCpuDispatch);
  pthread_create(&hiloKernelInterrupt, NULL, (void *)esperar_kernel_interrupt, (void *)socketCpuInterrupt);

  pthread_join(hiloKernelDispatch, NULL);
  pthread_join(hiloKernelInterrupt, NULL);

  list_destroy_and_destroy_elements(tlb, &free);

  log_destroy(logger);
  config_destroy(config);
}
