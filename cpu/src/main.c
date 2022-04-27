#include <cpu_utils.h>
#include <cpu_config.h>
#include <pthread.h>

void conectar_memoria(void);

int recibir_mensaje_dispatch(t_log *logger)
{
  int socketCPUDispatch = iniciar_servidor_cpu(logger, CPU_CONFIG.PUERTO_ESCUCHA_DISPATCH);
  int socketKernelDispatch = esperar_cliente(socketCPUDispatch);

  char *mensaje;

  while (true)
  {
    cod_op_servidor codOp = obtener_codigo_operacion(socketKernelDispatch);

    switch (codOp)
    {
    case MENSAJE_CLIENTE:
      mensaje = obtener_mensaje(socketKernelDispatch);
      log_info(logger, "Recibí el mensaje: %s", mensaje);
      break;

    case DESCONEXION_CLIENTE:
      // apagar_servidor_cpu(socketCPUDispatch, logger);
      break;
    default:
      log_warning(logger, "Operación desconocida.");
      break;
    }
  }
}

int recibir_mensaje_interrupt(t_log *logger)
{
  int socketCPUInterrupt = iniciar_servidor_cpu(logger, CPU_CONFIG.PUERTO_ESCUCHA_INTERRUPT);
  int socketKernelInterrupt = esperar_cliente(socketCPUInterrupt);

  char *mensaje;

  while (true)
  {
    cod_op_servidor codOp = obtener_codigo_operacion(socketKernelInterrupt);

    switch (codOp)
    {
    case MENSAJE_CLIENTE:
      mensaje = obtener_mensaje(socketKernelInterrupt);
      log_info(logger, "Recibí el mensaje: %s", mensaje);
      break;

    case DESCONEXION_CLIENTE:
      // apagar_servidor_cpu(socketCPUInterrupt, logger);
      break;
    default:
      log_warning(logger, "Operación desconocida.");
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  obtener_configuraciones_cpu();

  logger = log_create("CPU.log", "CPU", true, LOG_LEVEL_DEBUG);

  // int socketCPU = iniciar_servidor_cpu(logger);
  // int socketKernel = obtener_socket_kernel(socketCPU, logger);

  pthread_t hiloEscuchaDispatch;
  pthread_t hiloEscuchaInterrupt;

  pthread_create(&hiloEscuchaDispatch, NULL, (void *)recibir_mensaje_dispatch, (void *)logger);
  pthread_create(&hiloEscuchaInterrupt, NULL, (void *)recibir_mensaje_interrupt, (void *)logger);

  pthread_join(hiloEscuchaDispatch, NULL);
  pthread_join(hiloEscuchaInterrupt, NULL);
}

void conectar_memoria(void)
{
  int socketCpuCliente = crear_conexion_con_memoria();

  enviar_mensaje("soy CPU, pa", socketCpuCliente);

  liberar_conexion_con_memoria(socketCpuCliente);
}