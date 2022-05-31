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

  pcb->pid = ++idProcesoGlobal;
  pcb->tamanio = tamanioProceso;
  pcb->contadorPrograma = 0;
  pcb->tablaPaginas = 0;
  pcb->estimacionRafaga = KERNEL_CONFIG.ESTIMACION_INICIAL;
  pcb->escenario = malloc(sizeof(Escenario));
  pcb->escenario->estado = EJECUTANDO;
  pcb->escenario->tiempoBloqueadoIO = 0;
  pcb->instrucciones = list_duplicate(listaInstrucciones);

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
  return crear_conexion_con_servidor(KERNEL_CONFIG.IP, KERNEL_CONFIG.PUERTO_CPU_DISPATCH);
}

int conectar_con_cpu_interrupt()
{
  return crear_conexion_con_servidor(KERNEL_CONFIG.IP, KERNEL_CONFIG.PUERTO_CPU_INTERRUPT);
}

int conectar_con_memoria()
{
  return crear_conexion_con_servidor(KERNEL_CONFIG.IP, KERNEL_CONFIG.PUERTO_MEMORIA);
}