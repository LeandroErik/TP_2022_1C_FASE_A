#include <cpu_thread.h>

void esperar_kernel_dispatch(int socketCpu)
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

  realizar_handshake_con_memoria(socketMemoria);

  bool desconecto = false;

  while (true)
  {
    log_info(logger, "Esperando a Kernel que se conecte al puerto Dispatch...");
    int socketKernel = esperar_cliente(socketCpu);

    if (socketKernel < 0)
    {
      log_warning(logger, "Kernel desconectado.");
      log_destroy(logger);
      return;
    }

    log_info(logger, "Conexión con Kernel en puerto Dispatch establecida.");

    desconecto = manejar_paquete_kernel_dispatch(socketKernel);

    if (desconecto)
      log_destroy(logger);
      return;
  }

}

void esperar_kernel_interrupt(int socketCpu)
{
  bool desconecto = false;

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

    desconecto = manejar_paquete_kernel_interrupt(socketKernel);

    if (desconecto)
      return;
  }
}

bool manejar_paquete_kernel_dispatch(int socketKernel)
{
  while (true)
  {
    Logger *logger = iniciar_logger_cpu();
    Pcb *pcb;

    switch (obtener_codigo_operacion(socketKernel))
    {
    case DESCONEXION:
      log_warning(logger, "Conexión con Kernel en puerto Dispatch terminada.");
      liberar_conexion_con_servidor(socketKernel);
      log_destroy(logger);
      return true;

    case PCB:
      log_info(logger, "PCB recibido de Kernel.");
      pcb = deserializar_pcb(socketKernel);
      ejecutar_lista_instrucciones_del_pcb(pcb, socketKernel);
      break;

    default:
      break;
    }

    log_destroy(logger);
  }
}

bool manejar_paquete_kernel_interrupt(int socketKernel)
{
  while (true)
  {
    Logger *logger = iniciar_logger_cpu();

    switch (obtener_codigo_operacion(socketKernel))
    {
    case DESCONEXION:
      log_warning(logger, "Conexión con Kernel en puerto Interrupt terminada.");
      liberar_conexion_con_servidor(socketKernel);
      log_destroy(logger);
      return true;

    case INTERRUPCION:
      log_info(logger, "Interrupción recibida de Kernel.");

      seNecesitaAtenderInterrupcion = true;

      break;

    default:
      break;
    }

    log_destroy(logger);
  }
}

void realizar_handshake_con_memoria(int socketMemoria)
{
  Logger *logger = iniciar_logger_cpu();

  log_info(logger, "Enviando Mensaje de inicio al Servidor Memoria...");
  enviar_mensaje_a_servidor("CPU", socketMemoria);
  log_info(logger, "Mensaje de inicio enviado.");

  CodigoOperacion codigoOperacion = obtener_codigo_operacion(socketMemoria);

  switch (codigoOperacion)
  {
  case ESTRUCTURAS_MEMORIA:
    cargar_estructura_memoria(socketMemoria);
    break;
  default:
    break;
  }

  log_destroy(logger);
}

void cargar_estructura_memoria(int socketMemoria)
{
  Logger *logger = iniciar_logger_cpu();

  log_info(logger, "Cargando estructura de memoria...");

  Lista *lista_plana = obtener_paquete_como_lista(socketMemoria);

  ESTRUCTURA_MEMORIA.SOCKET_MEMORIA = socketMemoria;
  ESTRUCTURA_MEMORIA.ENTRADAS_POR_TABLA = *(int *)list_get(lista_plana, 0);
  ESTRUCTURA_MEMORIA.TAMANIO_PAGINA = *(int *)list_get(lista_plana, 1);

  log_info(logger, "Estructura de memoria cargada.");

  list_destroy_and_destroy_elements(lista_plana, &free);
  log_destroy(logger);
}