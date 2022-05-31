#include <cpu_utils.h>

Logger *iniciar_logger_cpu()
{
  return log_create("CPU.log", "CPU", true, LOG_LEVEL_INFO);
}

int iniciar_servidor_cpu_dispatch()
{
  return iniciar_servidor(CPU_CONFIG.IP, CPU_CONFIG.PUERTO_KERNEL_DISPATCH);
}

int iniciar_servidor_cpu_interrupt()
{
  return iniciar_servidor(CPU_CONFIG.IP, CPU_CONFIG.PUERTO_KERNEL_INTERRUPT);
}

int conectar_con_memoria()
{
  return crear_conexion_con_servidor(CPU_CONFIG.IP, CPU_CONFIG.PUERTO_MEMORIA);
}

void mostrar_lineas_instrucciones(Logger *logger, Lista *listaInstrucciones)
{
  LineaInstruccion *lineaInstruccion;

  for (int i = 0; i < list_size(listaInstrucciones); i++)
  {
    lineaInstruccion = list_get(listaInstrucciones, i);
    log_info(logger, "Instrucción Nº %d: %s\t- Parámetro 1: %d\t- Parámetro 2: %d", i, lineaInstruccion->identificador, lineaInstruccion->parametros[0], lineaInstruccion->parametros[1]);
  }

  eliminar_linea_instruccion(lineaInstruccion);
  list_destroy(listaInstrucciones);
}

void mostrar_pcb(Logger *logger, Pcb *pcb)
{
  log_info(logger, "PCB Nº %d", pcb->pid);
  log_info(logger, "Tamaño del Proceso: %d", pcb->tamanio);
  log_info(logger, "Estimación de Ráfaga: %f", pcb->estimacionRafaga);
  log_info(logger, "Estado: %d", pcb->escenario->estado);
  log_info(logger, "Tiempo de Bloqueo de I/O: %d", pcb->escenario->tiempoBloqueadoIO);
  mostrar_lineas_instrucciones(logger, pcb->instrucciones);
}