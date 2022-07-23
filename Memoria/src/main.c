#include <memoria_utils.h>
#include <main.h>

int main(int argc, char *argv[])
{
  Logger *logger = iniciar_logger_memoria();
  // if (argc < 2)
  // {
  //   log_error(logger, "Falta poner config.");
  //   return EXIT_FAILURE;
  // }
  // char *parametro = argv[1];
  Config *config = config_create("tlb.config");

  rellenar_config_memoria(config);

  log_info(logger, "Iniciando Servidor Memoria...");
  int socketMemoria = iniciar_servidor_memoria();

  if (socketMemoria < 0)
  {
    log_error(logger, "Error intentando iniciar Servidor Memoria.");
    return EXIT_FAILURE;
  }

  log_info(logger, "Servidor Memoria iniciado correctamente.");

  iniciar_estructuras_memoria();

  log_info(logger, "Esperando a los clientes CPU y Kernel");

  // Hilos
  Hilo hiloCliente1, hiloCliente2;
  int socketCliente = esperar_cliente(socketMemoria);
  pthread_create(&hiloCliente1, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);

  socketCliente = esperar_cliente(socketMemoria);
  pthread_create(&hiloCliente2, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);

  pthread_join(hiloCliente1, NULL);
  pthread_join(hiloCliente2, NULL);

  destruir_hilos(hiloCliente1, hiloCliente2);
  liberar_memoria();

  log_info(logger, "Page Faults totales: %d", contadorPageFaults);
  log_info(logger, "Accesos a discos totales: %d", contadorAccesosADisco);

  log_destroy(logger);
  config_destroy(config);

  return EXIT_SUCCESS;
}
