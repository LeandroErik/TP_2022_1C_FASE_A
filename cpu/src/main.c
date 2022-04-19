#include <cpu_utils.h>

int main(int argc, char *argv[])
{
  logger = log_create("CPU.log", "CPU", true, LOG_LEVEL_DEBUG);

  int socketCPU = iniciar_servidor_cpu(logger);
  int socketKernel = obtener_socket_kernel(socketCPU, logger);
  char *mensaje;

  while (true)
  {
    cod_op_servidor codOp = obtener_codigo_operacion(socketKernel);

    switch (codOp)
    {
    case MENSAJE_CLIENTE:
      mensaje = obtener_mensaje(socketKernel);
      log_info(logger, "Recibí el mensaje: %s", mensaje);
      break;

    case DESCONEXION_CLIENTE:
      apagar_servidor_cpu(socketCPU, logger);
      return EXIT_FAILURE;
    default:
      log_warning(logger, "Operación desconocida.");
      break;
    }
  }
  log_destroy(logger);
}