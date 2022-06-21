#include <memoria_thread.h>
#include <memoria_utils.h>

void manejar_paquetes_clientes(int socketCliente)
{
  Logger *logger = iniciar_logger_memoria();
  bool esKernel;

  switch (obtener_codigo_operacion(socketCliente))
  {
  case DESCONEXION:
    log_warning(logger, "Se desconecto un cliente.");
    return;

  case MENSAJE:
    esKernel = es_kernel(socketCliente);
    if (esKernel)
    {
      log_info(logger, "Se conecto Kernel.");
      escuchar_kernel(socketCliente);
    }
    else
    {
      log_info(logger, "Se conecto CPU.");
      escuchar_cpu(socketCliente);
    }
    break;
  default:
    log_info(logger, "Cliente desconocido.");
    break;
  }

  log_destroy(logger);
}

bool es_kernel(int socketCliente)
{
  char *mensaje = obtener_mensaje_del_cliente(socketCliente);
  bool esKernel = strcmp(mensaje, "Kernel") == 0;
  free(mensaje);

  return esKernel;
}

void escuchar_kernel(int socketKernel)
{
  Logger *logger = iniciar_logger_memoria();

  while (1)
  {
    CodigoOperacion codOp = obtener_codigo_operacion(socketKernel);

    switch (codOp)
    {
    case PROCESO_NUEVO:
      atender_creacion_de_proceso(socketKernel, logger);
      break;

    case SUSPENDER_PROCESO:
      atender_suspension_de_proceso(socketKernel, logger);
      break;
    case FINALIZAR_PROCESO:
      atender_finalizacion_de_proceso(socketKernel, logger);
      break;

    case DESCONEXION:
      log_warning(logger, "Se desconecto kernel.");
      return;

    default:
      log_warning(logger, "Operacion desconocida...");
      break;
    }
  }

  log_destroy(logger);
}

void escuchar_cpu(int socketCPU)
{
  Logger *logger = iniciar_logger_memoria();

  while (1)
  {
    CodigoOperacion codOp = obtener_codigo_operacion(socketCPU);

    switch (codOp)
    {
    case DESCONEXION:
      log_warning(logger, "Se desconecto CPU.");
      return;
    default:
      log_warning(logger, "Operacion desconocida...");
      break;
    }
    // TODO: agregar los otros case
    // https://github.com/sisoputnfrba/tp-2022-1c-FASE_A/issues/25
  }

  log_destroy(logger);
}

void realizar_espera_de_memoria()
{
  int retardoMemoria = MEMORIA_CONFIG.RETARDO_MEMORIA * 1000;
  usleep(retardoMemoria);
}

void atender_creacion_de_proceso(int socketKernel, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);
  int tamanio = *(int *)list_get(lista, 1);

  Proceso *nuevoProceso = crear_proceso(id, tamanio);
  char *nroTablaPrimerNivel = string_itoa(nuevoProceso->tablaPrimerNivel->nroTablaPrimerNivel);

  realizar_espera_de_memoria();

  enviar_mensaje_a_servidor(nroTablaPrimerNivel, socketKernel);
  log_info(logger, "Se envia a kernel el numero de tabla de primer nivel %d", nuevoProceso->tablaPrimerNivel->nroTablaPrimerNivel);
}

void atender_suspension_de_proceso(int socketKernel, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);

  suspender_proceso(id);

  realizar_espera_de_memoria();

  enviar_mensaje_a_servidor("Proceso suspendido", socketKernel);
  log_info(logger, "Se envia a kernel confirmacion de suspension del proceso %d", id);
}

void atender_finalizacion_de_proceso(int socketKernel, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);

  finalizar_proceso(id);

  realizar_espera_de_memoria();

  enviar_mensaje_a_servidor("Proceso finalizado", socketKernel);
  log_info(logger, "Se envia a kernel confirmacion de finalizacion del proceso %d", id);
}
