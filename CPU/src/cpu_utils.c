#include <cpu_utils.h>

typedef struct EntradaTlb
{
  int numeroPagina;
  int numeroMarco;
} EntradaTlb;

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

void ejecutar_read(int direccionFisica)
{
  Logger *logger = iniciar_logger_cpu();
  Paquete *paquete = crear_paquete(LEER_DE_MEMORIA);
  agregar_a_paquete(paquete, &direccionFisica, sizeof(int));
  enviar_paquete_a_servidor(paquete, ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);

  char *valor = obtener_mensaje_del_servidor(ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  log_info(logger, "Se recibio el valor %s de memoria", valor);

  free(valor);
  eliminar_paquete(paquete);
  log_destroy(logger);
}

void ejecutar_write(Pcb *proceso, int direccionFisica, int valor)
{
  Paquete *paquete = crear_paquete(ESCRIBIR_EN_MEMORIA);
  agregar_a_paquete(paquete, &direccionFisica, sizeof(int));
  agregar_a_paquete(paquete, &valor, sizeof(int));
  enviar_paquete_a_servidor(paquete, ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  eliminar_paquete(paquete);
}

void ejecutar_copy(Pcb *proceso, int direccionFisicoDestino, int direccionLogicaOrigen)
{
  int direccionFisicaOrigen = llamar_mmu(proceso, direccionLogicaOrigen);

  Paquete *paquete = crear_paquete(COPIAR_EN_MEMORIA);
  agregar_a_paquete(paquete, &direccionFisicoDestino, sizeof(int));
  agregar_a_paquete(paquete, &direccionFisicaOrigen, sizeof(int));
  enviar_paquete_a_servidor(paquete, ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  eliminar_paquete(paquete);
}

void atender_interrupcion(Pcb *pcb, int socketKernel)
{
  pcb->escenario->estado = INTERRUPCION_EXTERNA;

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

    int direccionFisica;

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
      log_info(logger, "Ejecutando READ");
      direccionFisica = llamar_mmu(pcb, lineaInstruccion->parametros[0]);
      ejecutar_read(direccionFisica);
      break;
    case COPY:
      log_info(logger, "Ejecutando COPY");
      direccionFisica = llamar_mmu(pcb, lineaInstruccion->parametros[0]);
      ejecutar_copy(pcb, direccionFisica, lineaInstruccion->parametros[1]);
      break;
    case WRITE:
      log_info(logger, "Ejecutando WRITE");
      direccionFisica = llamar_mmu(pcb, lineaInstruccion->parametros[0]);
      ejecutar_write(pcb, direccionFisica, lineaInstruccion->parametros[1]);
      break;
    case EXIT:
      log_info(logger, "Ejecutando EXIT");
      ejecutar_exit(pcb, socketKernel);
      limpiar_tlb();
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

bool esta_en_tlb(int numeroPagina)
{

  bool es_numero_pagina(void *_entradaTLB)
  {
    EntradaTlb *entradaTLB = (EntradaTlb *)_entradaTLB;
    return entradaTLB->numeroPagina == numeroPagina;
  }

  return list_any_satisfy(tlb, &es_numero_pagina);
}

int devolver_marco_por_tlb(int numeroPagina)
{
  bool es_numero_pagina(void *_entradaTLB)
  {
    EntradaTlb *entradaTLB = (EntradaTlb *)_entradaTLB;
    return entradaTLB->numeroPagina == numeroPagina;
  }

  EntradaTlb *entradaTlb = (EntradaTlb *)list_find(tlb, &es_numero_pagina);

  return entradaTlb->numeroMarco;
}

void agregar_a_tlb(int numeroPagina, int numeroMarco)
{
  if (list_size(tlb) <= CPU_CONFIG.ENTRADAS_TLB)
  {
    EntradaTlb *entradaTLB = (EntradaTlb *)malloc(sizeof(EntradaTlb));

    entradaTLB->numeroPagina = numeroPagina;
    entradaTLB->numeroMarco = numeroMarco;

    list_add(tlb, entradaTLB);
  }
  else
  {
    // TODO: Implementar método de sustitución (FIFO | LRU).
    // 1. Elegir víctima.
    // 2. Borrar víctima de la lista.
    // 3. Llamar agregar_a_tlb().
  }
}

void limpiar_tlb()
{
  list_clean_and_destroy_elements(tlb, &free);
}

void mostrar_tlb()
{
  Logger *logger = iniciar_logger_cpu();

  log_info(logger, "TLB:");
  for (int i = 0; i < list_size(tlb); i++)
  {
    EntradaTlb *entradaTLB = (EntradaTlb *)list_get(tlb, i);
    log_info(logger, "Entrada %d TLB:\n\t- Número de página: %d\n\t- Número de marco: %d\n\n", i, entradaTLB->numeroPagina, entradaTLB->numeroMarco);
  }

  log_destroy(logger);
}

int llamar_mmu(Pcb *proceso, int direccionLogica)
{
  Logger *logger = iniciar_logger_cpu();
  int numeroPagina = floor((float)direccionLogica / ESTRUCTURA_MEMORIA.TAMANIO_PAGINA);
  int desplazamiento = direccionLogica - numeroPagina * ESTRUCTURA_MEMORIA.TAMANIO_PAGINA;
  int numeroMarco;

  if (esta_en_tlb(numeroPagina))
    numeroMarco = devolver_marco_por_tlb(numeroPagina);

  else
  {
    int numeroTablaPrimerNivel = proceso->tablaPaginas;
    int entradaTablaPrimerNivel = floor((float)numeroPagina / ESTRUCTURA_MEMORIA.ENTRADAS_POR_TABLA);
    int entradaTablaSegundoNivel = numeroPagina % ESTRUCTURA_MEMORIA.ENTRADAS_POR_TABLA;
    int numeroTablaSegundoNivel = pedir_tabla_segundo_nivel(numeroTablaPrimerNivel, entradaTablaPrimerNivel);
    log_info(logger, "numero de tabla de segundo nivel recibido: %d", numeroTablaSegundoNivel);
    numeroMarco = pedir_marco(numeroTablaSegundoNivel, entradaTablaSegundoNivel);
    log_info(logger, "numero de marco recibido: %d", numeroMarco);
    agregar_a_tlb(numeroPagina, numeroMarco);
    mostrar_tlb();
    log_destroy(logger);
  }

  return numeroMarco * ESTRUCTURA_MEMORIA.TAMANIO_PAGINA + desplazamiento;
}

int pedir_tabla_segundo_nivel(int numeroTablaPrimerNivel, int entradaTablaPrimerNivel)
{
  Paquete *paquete = crear_paquete(PEDIDO_TABLA_SEGUNDO_NIVEL);
  agregar_a_paquete(paquete, &numeroTablaPrimerNivel, sizeof(int));
  agregar_a_paquete(paquete, &entradaTablaPrimerNivel, sizeof(int));

  enviar_paquete_a_cliente(paquete, ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  eliminar_paquete(paquete);

  char *mensaje = obtener_mensaje_del_servidor(ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  int numeroTablaSegundoNivel = atoi(mensaje);
  free(mensaje);

  return numeroTablaSegundoNivel;
}

int pedir_marco(int numeroTablaSegundoNivel, int entradaTablaSegundoNivel)
{
  Paquete *paquete = crear_paquete(PEDIDO_MARCO);
  agregar_a_paquete(paquete, &numeroTablaSegundoNivel, sizeof(int));
  agregar_a_paquete(paquete, &entradaTablaSegundoNivel, sizeof(int));

  enviar_paquete_a_cliente(paquete, ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  eliminar_paquete(paquete);

  char *mensaje = obtener_mensaje_del_servidor(ESTRUCTURA_MEMORIA.SOCKET_MEMORIA);
  int numeroMarco = atoi(mensaje);
  free(mensaje);

  return numeroMarco;
}