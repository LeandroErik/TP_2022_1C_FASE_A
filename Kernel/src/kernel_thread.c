#include <kernel_thread.h>

void esperar_consola(int socketKernel)
{
  while (true)
  {

    log_info(logger, "Esperando conexiones de Consola...");
    int socketConsola = esperar_cliente(socketKernel);

    if (socketConsola < 0)
    {
      log_warning(logger, "Consola desconectada.");

      return;
    }

    log_info(logger, "Conexión de Consola establecida.");

    Hilo hiloConsola;
    pthread_create(&hiloConsola, NULL, (void *)manejar_paquete_consola, (void *)socketConsola);
    list_add(hilosConsola, hiloConsola);
    pthread_join(hiloConsola, NULL);
    pthread_detach(hiloConsola);
  }
}

void manejar_paquete_consola(int socketConsola)
{
  while (true)
  {
    char *mensaje;
    switch (obtener_codigo_operacion(socketConsola))
    {
    case DESCONEXION:
      log_warning(logger, "Conexión de Consola terminada.");

      return;
    case MENSAJE:
      log_info(logger, "Mensaje recibido de Consola.");
      mensaje = obtener_mensaje_del_cliente(socketConsola);
      log_info(logger, "Mensaje: %s", mensaje);
      free(mensaje);
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

void enviar_interrupcion()
{

  log_info(logger, "Conectando con Servidor CPU via Interrupción en IP: %s, Puerto: %s", KERNEL_CONFIG.IP, KERNEL_CONFIG.PUERTO_CPU_INTERRUPT);

  int socketInterrupcion = conectar_con_cpu_interrupt();

  if (socketInterrupcion < 0)
  {
    log_error(logger, "Conexión rechazada. El Servidor CPU/Puerto Interrupción no está disponible.");

    return;
  }

  log_info(logger, "[INTERRUPCION] Enviando Interrupción al Servidor CPU...");

  Paquete *paquete = crear_paquete(INTERRUPCION);

  enviar_paquete_a_servidor(paquete, socketInterrupcion);

  eliminar_paquete(paquete);

  log_info(logger, "Saliendo del Puerto Interrupt...");

  liberar_conexion_con_servidor(socketInterrupcion);
}

void manejar_conexion_memoria()
{

  if (socketMemoria < 0)
  {
    log_warning(logger, "Conexión rechazada. El Servidor Memoria no está disponible.");

    return;
  }

  log_info(logger, "Enviando Mensaje de inicio al Servidor Memoria...");
  enviar_mensaje_a_servidor("Kernel", socketMemoria);
  log_info(logger, "Mensaje de inicio enviado.");
}