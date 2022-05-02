#include <cpu_utils.h>
#include <cpu_config.h>
#include <pthread.h>

void conectar_memoria(void);
pcb deserializar_pcb(t_list *);
void imprimir_pcb(pcb *, t_log *);

int recibir_mensaje_dispatch(t_log *logger)
{
  int socketCPUDispatch = iniciar_servidor_cpu(logger, CPU_CONFIG.PUERTO_ESCUCHA_DISPATCH);
  int socketKernelDispatch = esperar_cliente(socketCPUDispatch);

  char *mensaje;
  pcb proceso;
  t_list *paqueteLista;

  while (true)
  {
    cod_op codOp = recibir_operacion(socketKernelDispatch);

    switch (codOp)
    {
    case IMAGEN_PCB_P:
      paqueteLista = recibir_paquete(socketKernelDispatch);
      proceso = deserializar_pcb(paqueteLista);

      imprimir_pcb(&proceso, logger);
      list_destroy_and_destroy_elements(paqueteLista, free);

      break;
    case MENSAJE_CLIENTE_P:
      mensaje = obtener_mensaje(socketKernelDispatch);
      log_info(logger, "Recibí el mensaje: %s", mensaje);
      break;

    case DESCONEXION_CLIENTE_P:

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

pcb deserializar_pcb(t_list *listaRecibida)
{
  pcb proceso;
  proceso.pid = *(int *)list_get(listaRecibida, 0);
  proceso.tamanio = *(int *)list_get(listaRecibida, 1);
  proceso.proxima_instruccion = *(int *)list_get(listaRecibida, 2);
  proceso.tabla_de_paginas = *(int *)list_get(listaRecibida, 3);
  proceso.estimacion_rafaga = *(float *)list_get(listaRecibida, 4);
  proceso.lista_instrucciones = list_create();

  int tamanio_lista = *(int *)list_get(listaRecibida, 5);

  for (int i = 0; i < tamanio_lista; i++)
  {
    char *instruccion = string_new();
    instruccion = (char *)list_get(listaRecibida, 6 + i);
    list_add(proceso.lista_instrucciones, instruccion);
  }
  return proceso;
}

void imprimir_pcb(pcb *proceso, t_log *logger)
{
  log_info(logger, "proceso pid: %d", proceso->pid);
  log_info(logger, "proceso tamanio: %d", proceso->tamanio);
  log_info(logger, "proceso proxima instruccion: %d", proceso->proxima_instruccion);
  log_info(logger, "proceso tabla de paginas: %d", proceso->tabla_de_paginas);
  log_info(logger, "proceso estimacion: %f", proceso->estimacion_rafaga);

  for (int i = 0; i < list_size(proceso->lista_instrucciones); i++)
  {
    log_info(logger, "instruccion %i ,es : %s", i, list_get(proceso->lista_instrucciones, i));
  }
}