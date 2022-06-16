#include <cpu_thread.h>

void esperar_kernel_dispatch(int socketCpu)
{
  while (true)
  {
    Logger *logger = iniciar_logger_cpu();
    log_info(logger, "Esperando a Kernel que se conecte al puerto Dispatch...");
    int socketKernel = esperar_cliente(socketCpu);

    if (socketKernel < 0)
    {
      log_warning(logger, "Kernel desconectado.");
      log_destroy(logger);
      return;
    }

    log_info(logger, "Conexión con Kernel en puerto Dispatch establecida.");
    log_destroy(logger);

    manejar_paquete_kernel_dispatch(socketKernel);
  }
}

void esperar_kernel_interrupt(int socketCpu)
{
  while (true)
  {
    Logger *logger = iniciar_logger_cpu();
    log_info(logger, "Esperando a Kernel que se conecte al puerto Interrupt...");
    int socketKernel = esperar_cliente(socketCpu);

    if (socketKernel < 0)
    {
      log_warning(logger, "Kernel desconectado.");
      log_destroy(logger);
      return;
    }

    log_info(logger, "Conexión con Kernel en puerto Interrupt establecida.");
    log_destroy(logger);

    manejar_paquete_kernel_interrupt(socketKernel);
  }
}

void manejar_paquete_kernel_dispatch(int socketKernel)
{
  while (true)
  {
    Logger *logger = iniciar_logger_cpu();
    Pcb *pcb;

    switch (obtener_codigo_operacion(socketKernel))
    {
    case DESCONEXION:
      log_warning(logger, "Conexión con Kernel en puerto Dispatch terminada.");
      log_destroy(logger);
      return;

    case PCB:
      log_info(logger, "PCB recibido de Kernel.");
      pcb = deserializar_pcb(socketKernel);
      mostrar_pcb(logger, pcb);
      ejecutar_lista_instrucciones_del_pcb(pcb, socketKernel);
      break;

    default:
      break;
    }

    log_destroy(logger);
  }
}

void manejar_paquete_kernel_interrupt(int socketKernel)
{
  while (true)
  {
    Logger *logger = iniciar_logger_cpu();

    switch (obtener_codigo_operacion(socketKernel))
    {
    case DESCONEXION:
      log_warning(logger, "Conexión con Kernel en puerto Interrupt terminada.");
      log_destroy(logger);
      return;
    /// TODO: revisar nombre del enum.
    case MENSAJE:
      log_info(logger, "Interrupción recibida de Kernel.");
      log_info(logger, "Interrupción: %s", obtener_mensaje_del_cliente(socketKernel));
      seNecesitaAtenderInterrupcion = true;

      break;

    default:
      break;
    }

    log_destroy(logger);
  }
}

void manejar_conexion_memoria(int socketCpu)
{
  Logger *logger = iniciar_logger_cpu();

  log_info(logger, "Conectando con Servidor Memoria...");
  int socketMemoria = conectar_con_memoria();

  if (socketMemoria < 0)
  {
    log_warning(logger, "Conexión rechazada. El Servidor Memoria no está disponible.");
    log_destroy(logger);
    return;
  }

  log_info(logger, "Conexión con Memoria establecida.");

  log_info(logger, "Enviando Mensaje de inicio al Servidor Memoria...");
  enviar_mensaje_a_servidor("CPU", socketMemoria);
  log_info(logger, "Mensaje de inicio enviado.");

  log_info(logger, "Saliendo del Servidor Memoria...");
  liberar_conexion_con_servidor(socketMemoria);
  log_destroy(logger);
}