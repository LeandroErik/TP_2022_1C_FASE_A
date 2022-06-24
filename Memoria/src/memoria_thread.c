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
      enviar_estructuras_de_memoria_a_cpu(socketCliente, logger);
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

void enviar_estructuras_de_memoria_a_cpu(int socketCPU, Logger* logger)
{
  //TODO: Enviar estructuras basicas, crear el paquete
  log_info(logger, "Se envian a CPU las estructuras basicas de memoria");
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
    case PEDIDO_TABLA_SEGUNDO_NIVEL:
      atender_pedido_de_tabla_de_segundo_nivel(socketCPU, logger);
      break;
    case PEDIDO_MARCO:
      atender_pedido_de_marco(socketCPU, logger);
      break;
    case ESCRIBIR_EN_MEMORIA:
      atender_escritura_en_memoria(socketCPU, logger);
      break;
    case LEER_DE_MEMORIA:
      atender_lectura_de_memoria(socketCPU, logger);
      break;
    case COPIAR_EN_MEMORIA:
      atender_copiado_en_memoria(socketCPU, logger);
      break;
    case DESCONEXION:
      log_warning(logger, "Se desconecto CPU.");
      return;
    default:
      log_warning(logger, "Operacion desconocida...");
      break;
    }
  }

  log_destroy(logger);
}

void realizar_espera_de_memoria()
{
  int retardoMemoria = MEMORIA_CONFIG.RETARDO_MEMORIA * 1000;
  usleep(retardoMemoria);
}

//Funciones atender CPU
void atender_pedido_de_tabla_de_segundo_nivel(int socketCPU, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketCPU);
  int numeroTablaPrimerNivel = *(int *)list_get(lista, 0);
  int entradaATablaDePrimerNivel = *(int *)list_get(lista, 1);  

  char* numeroTablaSegundoNivel = "";
  //TODO: obtener numero tabla 2 nivel

  realizar_espera_de_memoria();
  enviar_mensaje_a_cliente(numeroTablaSegundoNivel, socketCPU);
  log_info(logger, "Se envia a CPU el numero de tabla de segundo nivel %s", numeroTablaSegundoNivel);
}

void atender_pedido_de_marco(int socketCPU, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketCPU);
  int numeroTablaSegundoNivel = *(int *)list_get(lista, 0);
  int entradaATablaDeSegundoNivel = *(int *)list_get(lista, 1); 

  char* numeroMarco = "";
  //TODO: Buscar el marco

  realizar_espera_de_memoria();
  enviar_mensaje_a_cliente(numeroMarco, socketCPU);
  log_info(logger, "Se envia a CPU el numero de marco %s", numeroMarco);
}

void atender_escritura_en_memoria(int socketCPU, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketCPU);
  int direccionFisicaAEscribir = *(int *)list_get(lista, 0);
  uint32_t numeroAEscribir = *(int *)list_get(lista, 1);

  //TODO: escribir en esa direccion
}

void atender_lectura_de_memoria(int socketCPU, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketCPU);
  int direccionFisicaALeer = *(int *)list_get(lista, 0);

  char* leido = "";
  //TODO: Leer

  realizar_espera_de_memoria();
  enviar_mensaje_a_cliente(leido, socketCPU);
  log_info(logger, "Se envia a CPU el numero leido %s", leido);
}

void atender_copiado_en_memoria(int socketCPU, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketCPU);
  int direccionFisicaDestino = *(int *)list_get(lista, 0);
  int direccionFisicaOrigen = *(int *)list_get(lista, 1);

  //TODO: copiar
}


//Funciones atender KERNEL
void atender_creacion_de_proceso(int socketKernel, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);
  int tamanio = *(int *)list_get(lista, 1);

  Proceso *nuevoProceso = crear_proceso(id, tamanio);
  char *numeroTablaPrimerNivel = string_itoa(nuevoProceso->tablaPrimerNivel->numeroTablaPrimerNivel);

  realizar_espera_de_memoria();

  enviar_mensaje_a_cliente(numeroTablaPrimerNivel, socketKernel);
  log_info(logger, "Se envia a kernel el numero de tabla de primer nivel %d", nuevoProceso->tablaPrimerNivel->numeroTablaPrimerNivel);
}

void atender_suspension_de_proceso(int socketKernel, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);

  suspender_proceso(id);

  realizar_espera_de_memoria();

  enviar_mensaje_a_cliente("Proceso suspendido", socketKernel);
  log_info(logger, "Se envia a kernel confirmacion de suspension del proceso %d", id);
}

void atender_finalizacion_de_proceso(int socketKernel, Logger *logger)
{
  t_list *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);

  finalizar_proceso(id);

  realizar_espera_de_memoria();

  enviar_mensaje_a_cliente("Proceso finalizado", socketKernel);
  log_info(logger, "Se envia a kernel confirmacion de finalizacion del proceso %d", id);
}
