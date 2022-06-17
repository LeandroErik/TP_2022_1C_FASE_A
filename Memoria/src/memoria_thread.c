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

void escuchar_kernel(int socketCliente)
{
  Logger *log = iniciar_logger_memoria();

  while (1)
  {
    CodigoOperacion codOp = obtener_codigo_operacion(socketCliente);

    t_list *lista;
    Proceso *nuevoProceso;
    int id, tamanio;
    char *nroTablaPrimerNivel;

    switch (codOp)
    {
    case PROCESO_NUEVO:
      lista = obtener_paquete_como_lista(socketCliente);
      id = *(int *)list_get(lista, 0);
      tamanio = *(int *)list_get(lista, 1);

      nuevoProceso = crear_proceso(id, tamanio);
      nroTablaPrimerNivel = string_itoa(nuevoProceso->tablaPrimerNivel->nroTablaPrimerNivel);
      enviar_mensaje_a_servidor(nroTablaPrimerNivel, socketCliente);
      log_info(log, "Se creo el Proceso %d tamanio %d, tabla de primer nivel numero: %d", nuevoProceso->idProceso, nuevoProceso->tamanio, nuevoProceso->tablaPrimerNivel->nroTablaPrimerNivel);
      break;

    case SUSPENDER_PROCESO:
      suspender_proceso(atoi(obtener_mensaje_del_cliente(socketCliente)));

      break;
    case FINALIZAR_PROCESO:
      finalizar_proceso(atoi(obtener_mensaje_del_cliente(socketCliente)));
      break;

    case DESCONEXION:
      log_warning(log, "Se desconecto kernel.");
      return;
    default:
      log_warning(log, "Operacion desconocida...");
      break;
    }
  }

  log_destroy(log);
}

void escuchar_cpu(int socketCliente)
{

  while (1)
  {
  }
}