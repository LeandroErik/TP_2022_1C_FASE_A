#include <cpu_utils.h>
#include <cpu_config.h>
#include <pthread.h>

void conectar_memoria(void);

pcb des_pcb(t_list *lista)
{
  pcb p;
  int tamanio_lista = 0;
  p.pid = *(int *)list_get(lista, 0);
  p.tamanio = *(int *)list_get(lista, 1);
  p.proxima_instruccion = *(int *)list_get(lista, 2);
  p.tabla_de_paginas = *(int *)list_get(lista, 3);
  p.estimacion_rafaga = *(float *)list_get(lista, 4);
  p.lista_instrucciones = list_create();

  tamanio_lista = *(int *)list_get(lista, 5);

  for (int i = 0; i < tamanio_lista; i++)
  {
    char *instruccion = string_new();
    instruccion = (char *)list_get(lista, 6 + i);

    list_add(p.lista_instrucciones, instruccion);

    printf("\n elemento %i ,es : %s", i, list_get(p.lista_instrucciones, i));
  }

  return p;
}

int recibir_mensaje_dispatch(t_log *logger)
{
  int socketCPUDispatch = iniciar_servidor_cpu(logger, CPU_CONFIG.PUERTO_ESCUCHA_DISPATCH);
  int socketKernelDispatch = esperar_cliente(socketCPUDispatch);

  char *mensaje;
  pcb pc;
  t_paquete *paquete;
  t_list *lista;

  while (true)
  {
    cod_op codOp = recibir_operacion(socketKernelDispatch);

    switch (codOp)
    {
    case IMAGEN_PCB_P:
      lista = recibir_paquete(socketKernelDispatch);
      pc = des_pcb(lista);
      log_info(logger, "Tamanio de datos recibidos: %d", list_size(lista));
      log_info(logger, "proceso pid: %d", pc.pid);
      log_info(logger, "proceso tamanio: %d", pc.tamanio);
      log_info(logger, "proceso proxima_ins: %d", pc.proxima_instruccion);
      log_info(logger, "proceso tablapaginas: %d", pc.tabla_de_paginas);
      log_info(logger, "Persona estimacion: %f", pc.estimacion_rafaga);
      list_destroy_and_destroy_elements(lista, free);

      break;
    case MENSAJE_CLIENTE_P:
      mensaje = obtener_mensaje(socketKernelDispatch);
      log_info(logger, "Recibí el mensaje: %s", mensaje);
      break;

    case DESCONEXION_CLIENTE_P:
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

  recibir_mensaje_dispatch(logger);

  // int socketCPU = iniciar_servidor_cpu(logger);
  // int socketKernel = obtener_socket_kernel(socketCPU, logger);

  /* pthread_t hiloEscuchaDispatch;
   pthread_t hiloEscuchaInterrupt;

   pthread_create(&hiloEscuchaDispatch, NULL, (void *)recibir_mensaje_dispatch, (void *)logger);
   pthread_create(&hiloEscuchaInterrupt, NULL, (void *)recibir_mensaje_interrupt, (void *)logger);

   pthread_join(hiloEscuchaDispatch, NULL);
   pthread_join(hiloEscuchaInterrupt, NULL);
   */
}

void conectar_memoria(void)
{
  int socketCpuCliente = crear_conexion_con_memoria();

  enviar_mensaje("soy CPU, pa", socketCpuCliente);

  liberar_conexion_con_memoria(socketCpuCliente);
}