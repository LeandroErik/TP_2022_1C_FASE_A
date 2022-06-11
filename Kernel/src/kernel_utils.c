#include <kernel_utils.h>

Logger *iniciar_logger_kernel()
{
  return log_create("Kernel.log", "Kernel", 1, LOG_LEVEL_INFO);
}

int iniciar_servidor_kernel()
{
  return iniciar_servidor(KERNEL_CONFIG.IP, KERNEL_CONFIG.PUERTO_KERNEL);
}

int rellenar_lista_instrucciones(Lista *listaInstrucciones, int socketCliente)
{
  Lista *listaPlana = obtener_paquete_como_lista(socketCliente);

  int tamanioProceso = *(int *)list_get(listaPlana, 0);
  deserializar_lista_de_instrucciones(listaInstrucciones, listaPlana, 1, 1);

  list_destroy(listaPlana);

  return tamanioProceso;
}

Pcb *crear_pcb(Lista *listaInstrucciones, int tamanioProceso)
{
  Pcb *pcb = malloc(sizeof(Pcb));
  pthread_mutex_lock(&mutexNumeroProceso);
  pcb->pid = idProcesoGlobal++;
  pthread_mutex_unlock(&mutexNumeroProceso);
  pcb->tamanio = tamanioProceso;
  pcb->contadorPrograma = 0;
  pcb->tablaPaginas = 0;
  pcb->estimacionRafaga = KERNEL_CONFIG.ESTIMACION_INICIAL;
  pcb->escenario = malloc(sizeof(Escenario));
  pcb->escenario->estado = LISTO;
  pcb->escenario->tiempoBloqueadoIO = 0;
  pcb->instrucciones = list_duplicate(listaInstrucciones);
  pcb->tiempoRafagaRealAnterior = KERNEL_CONFIG.ESTIMACION_INICIAL / 1000;
  pcb->tiempoInicioEjecucion = (int)NULL;

  list_destroy(listaInstrucciones);

  return pcb;
}

Pcb *generar_pcb(int socketCliente)
{
  Lista *listaInstrucciones = list_create();
  int tamanioProceso = rellenar_lista_instrucciones(listaInstrucciones, socketCliente);

  return crear_pcb(listaInstrucciones, tamanioProceso);
}

int conectar_con_cpu_dispatch()
{
  Logger *logger = iniciar_logger_kernel();

  log_info(logger, "Conectando con Servidor CPU via Dispatch en IP: %s y Puerto: %s", KERNEL_CONFIG.IP_CPU, KERNEL_CONFIG.PUERTO_CPU_DISPATCH);

  int socketDispatch = crear_conexion_con_servidor(KERNEL_CONFIG.IP_CPU, KERNEL_CONFIG.PUERTO_CPU_DISPATCH);

  if (socketDispatch < 0)
  {
    log_error(logger, "Conexión rechazada. El Servidor CPU/Puerto Dispatch no está disponible. %d", socketDispatch);
    log_destroy(logger);
    return DESCONEXION;
  }

  log_info(logger, "Conexión con Dispatch establecida.");

  return socketDispatch;
}

int conectar_con_cpu_interrupt()
{
  return crear_conexion_con_servidor(KERNEL_CONFIG.IP_CPU, KERNEL_CONFIG.PUERTO_CPU_INTERRUPT);
}

int conectar_con_memoria()
{
  return crear_conexion_con_servidor(KERNEL_CONFIG.IP_MEMORIA, KERNEL_CONFIG.PUERTO_MEMORIA);
}