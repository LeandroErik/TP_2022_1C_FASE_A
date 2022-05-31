#include <memoria_utils.h>
#include <commons/config.h>

bool es_kernel(int socketCliente)
{
  char *mensaje = obtener_mensaje(socketCliente);
  return !strcmp("Kernel", mensaje);
}

int recibir_mensaje_cliente(int socketCliente)
{
  t_log *logger = log_create("Memoria.log", "Memoria", true, LOG_LEVEL_DEBUG);
  bool es_cliente_kernel;

  cod_op_cliente codOp = obtener_codigo_operacion(socketCliente);
  switch (codOp)
  {
  case MENSAJE_CLIENTE:
    es_cliente_kernel = es_kernel(socketCliente);
    if (es_cliente_kernel)
      log_info(logger, "Recibi un mensaje de Kernel");
    else
      log_info(logger, "Recibi un mensaje de CPU");
    break;
  default:
    log_warning(logger, "Operación desconocida.");
    break;
  }
}

int main(int argc, char *argv[])
{
  logger = log_create("Memoria.log", "Memoria", true, LOG_LEVEL_DEBUG);

  int socketKernel = iniciar_servidor_memoria(logger);

  while (true)
  {
    int socketCliente = esperar_cliente(socketKernel);
    pthread_t hiloCliente;

    pthread_create(&hiloCliente, NULL, (void *)recibir_mensaje_cliente, (void *)socketCliente);
  }
  log_destroy(logger);

  return EXIT_SUCCESS;
}