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

void escuchar_kernel(int socketKernel)
{
  Logger *logger = iniciar_logger_memoria();

  while (1)
  {
    CodigoOperacion codOp = obtener_codigo_operacion(socketKernel);

    switch (codOp)
    {
    case PROCESO_NUEVO:
      log_info(logger, "Se recibio un pedido de creacion de proceso de Kernel");
      atender_creacion_de_proceso(socketKernel, logger);
      break;

    case SUSPENDER_PROCESO:
      log_info(logger, "Se recibio un pedido de suspension de proceso de Kernel");
      atender_suspension_de_proceso(socketKernel, logger);
      break;
    case FINALIZAR_PROCESO:
      log_info(logger, "Se recibio un pedido de finalizacion de proceso de Kernel");
      atender_finalizacion_de_proceso(socketKernel, logger);
      break;

    case DESCONEXION:
      log_warning(logger, "Se desconecto kernel.");
      log_destroy(logger);
      return;

    default:
      log_warning(logger, "Operacion desconocida...");
      break;
    }
  }
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
      log_info(logger, "Se recibio un pedido de numero de tabla de segundo nivel de CPU");
      atender_pedido_de_tabla_de_segundo_nivel(socketCPU, logger);
      break;
    case PEDIDO_MARCO:
      log_info(logger, "Se recibio un pedido de numero de marco de CPU");
      atender_pedido_de_marco(socketCPU, logger);
      break;
    case ESCRIBIR_EN_MEMORIA:
      log_info(logger, "Se recibio un pedido de escritura en memoria de CPU");
      atender_escritura_en_memoria(socketCPU, logger);
      break;
    case LEER_DE_MEMORIA:
      log_info(logger, "Se recibio un pedido de lectura en memoria de CPU");
      atender_lectura_de_memoria(socketCPU, logger);
      break;
    case COPIAR_EN_MEMORIA:
      log_info(logger, "Se recibio un pedido de copiado en memoria de CPU");
      atender_copiado_en_memoria(socketCPU, logger);
      break;
    case DESCONEXION:
      log_warning(logger, "Se desconecto CPU.");
      log_destroy(logger);
      return;
    default:
      log_warning(logger, "Operacion desconocida...");
      break;
    }
  }
}

void realizar_espera_de_memoria()
{
  int retardoMemoria = MEMORIA_CONFIG.RETARDO_MEMORIA * 1000;
  usleep(retardoMemoria);
}

// Funciones atender CPU
void enviar_estructuras_de_memoria_a_cpu(int socketCPU, Logger *logger)
{
  Paquete *paquete = crear_paquete(ESTRUCTURAS_MEMORIA);
  agregar_a_paquete(paquete, &MEMORIA_CONFIG.ENTRADAS_POR_TABLA, sizeof(int));
  agregar_a_paquete(paquete, &MEMORIA_CONFIG.TAM_PAGINA, sizeof(int));
  enviar_paquete_a_cliente(paquete, socketCPU);
  eliminar_paquete(paquete);
}

void atender_pedido_de_tabla_de_segundo_nivel(int socketCPU, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketCPU);
  int numeroTablaPrimerNivel = *(int *)list_get(lista, 0);
  int entradaATablaDePrimerNivel = *(int *)list_get(lista, 1);
  list_destroy_and_destroy_elements(lista, &free);

  int numeroTablaSegundoNivel = obtener_numero_tabla_segundo_nivel(numeroTablaPrimerNivel, entradaATablaDePrimerNivel);

  realizar_espera_de_memoria();

  char *numeroComoString = string_itoa(numeroTablaSegundoNivel);
  enviar_mensaje_a_cliente(numeroComoString, socketCPU);
  log_info(logger, "Se envia a CPU el numero de tabla de segundo nivel %d", numeroTablaSegundoNivel);
  free(numeroComoString);
}

void atender_pedido_de_marco(int socketCPU, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketCPU);
  int numeroTablaSegundoNivel = *(int *)list_get(lista, 0);
  int entradaATablaDeSegundoNivel = *(int *)list_get(lista, 1);
  list_destroy_and_destroy_elements(lista, &free);

  int numeroMarco = obtener_numero_marco(numeroTablaSegundoNivel, entradaATablaDeSegundoNivel);

  realizar_espera_de_memoria();

  char *numeroComoString = string_itoa(numeroMarco);
  enviar_mensaje_a_cliente(numeroComoString, socketCPU);
  log_info(logger, "Se envia a CPU el numero de marco %d", numeroMarco);
  free(numeroComoString);
}

void atender_escritura_en_memoria(int socketCPU, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketCPU);
  int direccionFisicaAEscribir = *(int *)list_get(lista, 0);
  uint32_t numeroAEscribir = *(uint32_t *)list_get(lista, 1);
  list_destroy_and_destroy_elements(lista, &free);

  escribir_entero_en_memoria(numeroAEscribir, direccionFisicaAEscribir);

  enviar_mensaje_a_cliente("ok", socketCPU);
}

void atender_lectura_de_memoria(int socketCPU, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketCPU);
  int direccionFisicaALeer = *(int *)list_get(lista, 0);
  log_info(logger, "direccion fisica a LEER : %d", direccionFisicaALeer);
  list_destroy_and_destroy_elements(lista, &free);

  int leido = leer_entero_de_memoria(direccionFisicaALeer);

  realizar_espera_de_memoria();

  char *leidoComoString = string_itoa(leido);
  enviar_mensaje_a_cliente(leidoComoString, socketCPU);
  log_info(logger, "Se envia a CPU el numero leido %d", leido);
  free(leidoComoString);
}

void atender_copiado_en_memoria(int socketCPU, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketCPU);
  int direccionFisicaDestino = *(int *)list_get(lista, 0);
  int direccionFisicaOrigen = *(int *)list_get(lista, 1);
  list_destroy_and_destroy_elements(lista, &free);

  copiar_entero_en_memoria(direccionFisicaDestino, direccionFisicaOrigen);

  enviar_mensaje_a_cliente("ok", socketCPU);
}

// Funciones atender KERNEL
void atender_creacion_de_proceso(int socketKernel, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);
  int tamanio = *(int *)list_get(lista, 1);
  list_destroy_and_destroy_elements(lista, &free);

  Proceso *nuevoProceso = crear_proceso(id, tamanio);
  char *numeroTablaPrimerNivel = string_itoa(nuevoProceso->tablaPrimerNivel->numeroTablaPrimerNivel);

  realizar_espera_de_memoria();

  enviar_mensaje_a_cliente(numeroTablaPrimerNivel, socketKernel);
  log_info(logger, "Se envia a kernel el numero de tabla de primer nivel %d", nuevoProceso->tablaPrimerNivel->numeroTablaPrimerNivel);
  free(numeroTablaPrimerNivel);
}

void atender_suspension_de_proceso(int socketKernel, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);
  list_destroy_and_destroy_elements(lista, &free);

  suspender_proceso(id);

  realizar_espera_de_memoria();

  enviar_mensaje_a_cliente("Proceso suspendido", socketKernel);
  log_info(logger, "Se envia a kernel confirmacion de suspension del proceso %d", id);
}

void atender_finalizacion_de_proceso(int socketKernel, Logger *logger)
{
  Lista *lista = obtener_paquete_como_lista(socketKernel);
  int id = *(int *)list_get(lista, 0);
  list_destroy_and_destroy_elements(lista, &free);

  finalizar_proceso(id);

  realizar_espera_de_memoria();

  enviar_mensaje_a_cliente("Proceso finalizado", socketKernel);
  log_info(logger, "Se envia a kernel confirmacion de finalizacion del proceso %d", id);
}
