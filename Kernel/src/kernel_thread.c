#include <kernel_thread.h>

void esperar_consola(int socketKernel)
{
  while (true)
  {
    Logger *logger = iniciar_logger_kernel();
    log_info(logger, "Esperando conexiones de Consola...");
    int socketConsola = esperar_cliente(socketKernel);

    if (socketConsola < 0)
    {
      log_warning(logger, "Consola desconectada.");
      log_destroy(logger);
      return;
    }

    log_info(logger, "Conexión de Consola establecida.");
    log_destroy(logger);

    Hilo hiloConsola;
    pthread_create(&hiloConsola, NULL, (void *)manejar_paquete_consola, (void *)socketConsola);
    pthread_join(hiloConsola, NULL);
  }
}

void manejar_paquete_consola(int socketConsola)
{
  while (true)
  {
    Logger *logger = iniciar_logger_kernel();

    switch (obtener_codigo_operacion(socketConsola))
    {
    case DESCONEXION:
      log_warning(logger, "Conexión de Consola terminada.");
      log_destroy(logger);
      return;
    case MENSAJE:
      log_info(logger, "Mensaje recibido de Consola.");
      log_info(logger, "Mensaje: %s", obtener_mensaje_del_cliente(socketConsola));
      break;
    case LINEAS_CODIGO:
      log_info(logger, "Lineas de Código recibidas de Consola.");
      agregar_proceso_nuevo(generar_pcb(socketConsola));
      break;
    default:
      break;
    }
  }
}

void manejar_conexion_cpu_interrupcion()
{
  Logger *logger = iniciar_logger_kernel();

  log_info(logger, "Conectando con Servidor CPU via Interrupción en IP: %s, Puerto: %s", KERNEL_CONFIG.IP, KERNEL_CONFIG.PUERTO_CPU_INTERRUPT);
  int socketInterrupcion = conectar_con_cpu_interrupt();

  if (socketInterrupcion < 0)
  {
    log_error(logger, "Conexión rechazada. El Servidor CPU/Puerto Interrupción no está disponible.");
    log_destroy(logger);
    return;
  }

  log_info(logger, "Conexión con Interrupción establecida.");

  log_info(logger, "Enviando Interrupción al Servidor CPU...");
  enviar_mensaje_a_servidor("Interrupción externa", socketInterrupcion);

  log_info(logger, "Saliendo del Puerto Interrupt...");
  liberar_conexion_con_servidor(socketInterrupcion);
  log_destroy(logger);
}

void manejar_conexion_memoria()
{
  Logger *logger = iniciar_logger_kernel();

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
  enviar_mensaje_a_servidor("Kernel", socketMemoria);
  log_info(logger, "Mensaje de inicio enviado.");

  log_info(logger, "Saliendo del Servidor Memoria...");
  liberar_conexion_con_servidor(socketMemoria);
  log_destroy(logger);
}