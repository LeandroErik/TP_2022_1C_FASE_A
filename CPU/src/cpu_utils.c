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
  return crear_conexion_con_servidor(CPU_CONFIG.IP_MEMORIA, CPU_CONFIG.PUERTO_MEMORIA);
}

void mostrar_lineas_instrucciones(Logger *logger, Lista *listaInstrucciones)
{
  LineaInstruccion *lineaInstruccion;

  for (int i = 0; i < list_size(listaInstrucciones); i++)
  {
    lineaInstruccion = list_get(listaInstrucciones, i);
    log_info(logger, "Instrucción Nº %d: %s\t- Parámetro 1: %d\t- Parámetro 2: %d", i, lineaInstruccion->identificador, lineaInstruccion->parametros[0], lineaInstruccion->parametros[1]);
  }
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

Instruccion obtener_tipo_instruccion(char *instruccion)
{
  if (!strcmp(instruccion, "NO_OP"))
    return NOOP;
  else if (!strcmp(instruccion, "I/O"))
    return IO;
  else if (!strcmp(instruccion, "READ"))
    return READ;
  else if (!strcmp(instruccion, "COPY"))
    return COPY;
  else if (!strcmp(instruccion, "WRITE"))
    return WRITE;
  else if (!strcmp(instruccion, "EXIT"))
    return EXIT;
  else
    return DESCONOCIDA;
}

void ejecutar_noop()
{
  int tiempoEnMiliSegundos = CPU_CONFIG.RETARDO_NOOP;
  int tiempoEnMicroSegundos = tiempoEnMiliSegundos * 1000;
  usleep(tiempoEnMicroSegundos);
}

void ejecutar_io(Pcb *pcb, int tiempoBloqueadoIO, int socketKernel)
{
  pcb->escenario->estado = BLOQUEADO_IO;
  pcb->escenario->tiempoBloqueadoIO = tiempoBloqueadoIO;

  Paquete *paquete = malloc(sizeof(Paquete));
  paquete = crear_paquete(PCB);
  serializar_pcb(paquete, pcb);

  enviar_paquete_a_cliente(paquete, socketKernel);
  eliminar_paquete(paquete);
}

void ejecutar_exit(Pcb *pcb, int socketKernel)
{
  pcb->escenario->estado = TERMINADO;

  Paquete *paquete = malloc(sizeof(Paquete));
  paquete = crear_paquete(PCB);
  serializar_pcb(paquete, pcb);

  enviar_paquete_a_cliente(paquete, socketKernel);
  eliminar_paquete(paquete);
}

void atender_interrupcion(Pcb *pcb, int socketKernel)
{
  pcb->escenario->estado = INTERRUMPIDO;

  Paquete *paquete = malloc(sizeof(Paquete));
  paquete = crear_paquete(PCB);
  serializar_pcb(paquete, pcb);

  enviar_paquete_a_cliente(paquete, socketKernel);

  eliminar_paquete(paquete);

  seNecesitaAtenderInterrupcion = false;
}

void ejecutar_lista_instrucciones_del_pcb(Pcb *pcb, int socketKernel)
{
  Logger *logger = iniciar_logger_cpu();

  log_info(logger, "Ejecutando instrucciones del PCB Nº %d", pcb->pid);

  for (int i = pcb->contadorPrograma; i < list_size(pcb->instrucciones); i++)
  {
    printf("Contador de Programa: %d\n", pcb->contadorPrograma);

    LineaInstruccion *lineaInstruccion = list_get(pcb->instrucciones, pcb->contadorPrograma);
    Instruccion instruccion = obtener_tipo_instruccion(lineaInstruccion->identificador);

    if (seNecesitaAtenderInterrupcion)
    {
      log_info(logger, "Se necesita atender una interrupción");

      atender_interrupcion(pcb, socketKernel);

      log_info(logger, "Se termino de atender una interrupción (valor : %d)", seNecesitaAtenderInterrupcion);

      return;
    }
    pcb->contadorPrograma++;
    switch (instruccion)
    {
    case NOOP:
      log_info(logger, "Ejecutando NOOP");
      ejecutar_noop();
      break;
    case IO:
      log_info(logger, "Ejecutando IO");
      ejecutar_io(pcb, lineaInstruccion->parametros[0], socketKernel);
      break;
    case READ:
      break;
    case COPY:
      break;
    case WRITE:
      break;
    case EXIT:
      log_info(logger, "Ejecutando EXIT");
      ejecutar_exit(pcb, socketKernel);
      break;
    default:
      log_error(logger, "Instrucción desconocida: %s", lineaInstruccion->identificador);
      return;
    }

    if (instruccion == IO || instruccion == EXIT)
    {
      log_destroy(logger);
      return;
    }
  }
}