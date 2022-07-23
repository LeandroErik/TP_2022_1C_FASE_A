#include <memoria_utils.h>
#include <main.h>
#include <swap.h>

Logger *iniciar_logger_memoria()
{
  return log_create("Memoria.log", "Memoria", true, LOG_LEVEL_INFO);
}

int iniciar_servidor_memoria()
{
  return iniciar_servidor(MEMORIA_CONFIG.IP_MEMORIA, MEMORIA_CONFIG.PUERTO_MEMORIA);
}

// Estructuras
void iniciar_estructuras_memoria()
{
  Logger *logger = iniciar_logger_memoria();

  tablasDePrimerNivel = 0;
  tablasDeSegundoNivel = 0;
  contadorPageFaults = 0;
  contadorAccesosADisco = 0;

  memoriaPrincipal = (void *)malloc(MEMORIA_CONFIG.TAM_MEMORIA);
  memset(memoriaPrincipal, '0', MEMORIA_CONFIG.TAM_MEMORIA);

  iniciar_marcos();
  procesos = list_create();

  log_info(logger, "Estructuras de memoria inicializadas");
  log_destroy(logger);
}

void iniciar_semaforos()
{
  // sem_init(&semaforoMarcos, 0, 0);
  pthread_mutex_init(&semaforoProcesos, NULL);
}

void iniciar_marcos()
{
  int cantidadMarcos = MEMORIA_CONFIG.TAM_MEMORIA / MEMORIA_CONFIG.TAM_PAGINA;
  marcos = list_create();
  for (int numeroDeMarco = 0; numeroDeMarco < cantidadMarcos; numeroDeMarco++)
  {
    Marco *marco = malloc(sizeof(Marco));
    marco->numeroMarco = numeroDeMarco;
    marco->idProceso = -1;
    marco->paginaActual = NULL;
    list_add(marcos, marco);
  }
}

Proceso *crear_proceso(int id, int tamanio)
{
  Logger *logger = iniciar_logger_memoria();

  Proceso *proceso = malloc(sizeof(Proceso));
  proceso->idProceso = id;
  proceso->tamanio = tamanio;
  proceso->tablaPrimerNivel = crear_tabla_primer_nivel();
  proceso->paginasAsignadas = list_create();
  proceso->posicionDelPunteroDeSustitucion = 0;

  log_info(logger, "Se creo el Proceso: %d, tamanio: %d", proceso->idProceso, proceso->tamanio);

  proceso->archivoSwap = crear_archivo_swap(id);
  agregar_proceso_a_lista_de_procesos(proceso);

  log_destroy(logger);

  return proceso;
}

void agregar_proceso_a_lista_de_procesos(Proceso *proceso)
{
  pthread_mutex_lock(&semaforoProcesos);
  list_add(procesos, proceso);
  pthread_mutex_unlock(&semaforoProcesos);
}

TablaPrimerNivel *crear_tabla_primer_nivel()
{
  TablaPrimerNivel *tabla = malloc(sizeof(TablaPrimerNivel));
  tabla->entradas = list_create();
  tabla->numeroTablaPrimerNivel = tablasDePrimerNivel;
  tablasDePrimerNivel++;

  int entradasPorTabla = MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  for (int numeroTablaSegundoNivel = 0; numeroTablaSegundoNivel < entradasPorTabla; numeroTablaSegundoNivel++)
  {
    TablaSegundoNivel *tablaSegundoNivel = crear_tabla_segundo_nivel(numeroTablaSegundoNivel * entradasPorTabla);
    list_add(tabla->entradas, tablaSegundoNivel);
  }

  return tabla;
}

TablaSegundoNivel *crear_tabla_segundo_nivel(int nroPrimerPaginaDeTabla)
{
  TablaSegundoNivel *tabla = malloc(sizeof(TablaSegundoNivel));
  tabla->entradas = list_create();
  tabla->numeroTablaSegundoNivel = tablasDeSegundoNivel;
  tablasDeSegundoNivel++;

  int entradasPorTabla = MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  for (int i = 0; i < entradasPorTabla; i++)
  {
    Pagina *pag = malloc(sizeof(Pagina));
    pag->numeroPagina = i + nroPrimerPaginaDeTabla;
    pag->paginaVacia = true;
    pag->modificado = false;
    pag->uso = false;
    pag->marcoAsignado = NULL;
    list_add(tabla->entradas, pag);
  }
  return tabla;
}
// Fin funciones de estructuras

// Funciones de escritura, lectura y copiado en memoria fisica
Marco *marco_de_direccion_fisica(int direccionFisica)
{
  int numeroDeMarco = direccionFisica / MEMORIA_CONFIG.TAM_PAGINA;
  return list_get(marcos, numeroDeMarco);
}

void escribir_entero_en_memoria(uint32_t valorAEscribir, int direccionFisica)
{
  Logger *logger = iniciar_logger_memoria();

  void *direccionInicioEscritura = memoriaPrincipal + direccionFisica;
  memcpy(direccionInicioEscritura, &valorAEscribir, sizeof(valorAEscribir));

  Marco *marcoEscrito = marco_de_direccion_fisica(direccionFisica);
  marcoEscrito->paginaActual->modificado = true;
  marcoEscrito->paginaActual->paginaVacia = false;
  marcoEscrito->paginaActual->uso = true;

  log_info(logger, "Valor escrito %d, en la posicion de memoria fisica %d.", valorAEscribir, direccionFisica);
  log_destroy(logger);
}

uint32_t leer_entero_de_memoria(int direccionFisica)
{
  Logger *logger = iniciar_logger_memoria();

  uint32_t leido;
  void *direccionInicioLectura = memoriaPrincipal + direccionFisica;
  memcpy(&leido, direccionInicioLectura, sizeof(leido));

  Marco *marcoLeido = marco_de_direccion_fisica(direccionFisica);
  if (marcoLeido == NULL)
  {
    log_error(logger, "ERROR EN EL MARCO LEIDO");
  }
  marcoLeido->paginaActual->uso = true;

  log_info(logger, "Valor leido %d, de la posicion de memoria fisica %d.", leido, direccionFisica);
  log_destroy(logger);

  return leido;
}

void copiar_entero_en_memoria(int direccionFisicaDestino, int direccionFisicaOrigen)
{
  Logger *logger = iniciar_logger_memoria();

  log_info(logger, "Iniciando Copia:");

  uint32_t leido = leer_entero_de_memoria(direccionFisicaOrigen);
  escribir_entero_en_memoria(leido, direccionFisicaDestino);

  log_info(logger, "Copia finalizada.");
  log_destroy(logger);
}
// Fin funciones de escritura, lectura y copia en memoria

Proceso *buscar_proceso_por_id(int idProceso)
{
  pthread_mutex_lock(&semaforoProcesos);

  bool es_proceso(void *_proceso)
  {
    Proceso *proceso = (Proceso *)_proceso;
    return proceso->idProceso == idProceso;
  }

  Proceso *proceso = list_find(procesos, &es_proceso);
  pthread_mutex_unlock(&semaforoProcesos);

  return proceso;
}

Marco *primer_marco_libre() // First fit
{
  bool marco_libre(void *_marco)
  {
    Marco *marco = (Marco *)_marco;
    return marco->paginaActual == NULL;
  }

  return list_find(marcos, &marco_libre);
}

bool tiene_marcos_por_asignar(Proceso *proceso)
{
  int cantidadDePaginasAsignadas = list_size(proceso->paginasAsignadas);

  return cantidadDePaginasAsignadas < MEMORIA_CONFIG.MARCOS_POR_PROCESO;
}

void agregar_pagina_a_paginas_asignadas_del_proceso(Proceso *proceso, Pagina *pagina)
{
  int posicionAAgregar = proceso->posicionDelPunteroDeSustitucion - 1;

  if (posicionAAgregar >= 0)
    list_add_in_index(proceso->paginasAsignadas, posicionAAgregar, pagina);
  else
    list_add(proceso->paginasAsignadas, pagina);
}

void asignar_pagina_del_proceso_al_marco(Proceso *proceso, Pagina *pagina, Marco *marco)
{
  Logger *logger = iniciar_logger_memoria();

  if (!pagina->paginaVacia) // Para saber si hay que traer info de swap o no
  {
    escribir_datos_de_pagina_en_memoria(proceso, pagina->numeroPagina, marco->numeroMarco);
  }

  agregar_pagina_a_paginas_asignadas_del_proceso(proceso, pagina);
  pagina->marcoAsignado = marco;
  pagina->uso = true;
  marco->paginaActual = pagina;
  marco->idProceso = proceso->idProceso;

  log_info(logger, "Pagina %d del proceso %d, asignada al Marco %d", pagina->numeroPagina, proceso->idProceso, marco->numeroMarco);
  log_destroy(logger);
}

bool hay_que_sustituir_pagina_del_proceso(Proceso *proceso, Marco *marcoLibre)
{
  return !tiene_marcos_por_asignar(proceso) || marcoLibre == NULL;
}

Marco *asignar_pagina_a_marco_libre(Proceso *proceso, Pagina *pagina)
{
  Logger *logger = iniciar_logger_memoria();

  Marco *marco = primer_marco_libre();
  if (hay_que_sustituir_pagina_del_proceso(proceso, marco))
  {
    log_info(logger, "No se pudo asignar la pagina %d del proceso %d, se debe realizar una sustitucion", pagina->numeroPagina, proceso->idProceso);
    marco = marco_del_proceso_sustituido(proceso);
  }
  asignar_pagina_del_proceso_al_marco(proceso, pagina, marco);

  log_destroy(logger);
  return marco;
}

// Algoritmos de sustitucion
Marco *marco_del_proceso_sustituido(Proceso *proceso)
{
  Logger *logger = iniciar_logger_memoria();

  Marco *marcoSustituido;
  bool esClock = !strcmp(MEMORIA_CONFIG.ALGORITMO_REEMPLAZO, "CLOCK");
  if (esClock)
  {
    log_info(logger, "Iniciando algoritmo de sustitucion Clock");
    marcoSustituido = correr_clock(proceso, logger);
  }
  else
  {
    log_info(logger, "Iniciando algoritmo de sustitucion Clock-M");
    marcoSustituido = correr_clock_modificado(proceso, logger);
  }

  log_destroy(logger);
  return marcoSustituido;
}

Marco *correr_clock(Proceso *proceso, Logger *logger)
{
  Marco *marcoSustituido;
  bool eligioVictima = false;
  int numeroDePaginasAsignadas = list_size(proceso->paginasAsignadas);
  while (!eligioVictima)
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, proceso->posicionDelPunteroDeSustitucion);
    if (!pagina->uso)
    {
      eligioVictima = true;
      log_info(logger, "Victima elegida pagina %d del proceso %d", pagina->numeroPagina, proceso->idProceso);
      marcoSustituido = desalojar_pagina(proceso, pagina);
    }
    else
    {
      pagina->uso = false;
    }

    proceso->posicionDelPunteroDeSustitucion++;
    if (proceso->posicionDelPunteroDeSustitucion == numeroDePaginasAsignadas)
    {
      proceso->posicionDelPunteroDeSustitucion = 0;
    }
  }

  return marcoSustituido;
}

Marco *correr_clock_modificado(Proceso *proceso, Logger *logger)
{
  Marco *marcoSustituido;
  bool eligioVictima = false;
  int numeroDePaginasAsignadas = list_size(proceso->paginasAsignadas);
  int posicionInicialDelPuntero = proceso->posicionDelPunteroDeSustitucion;

  int contadorDeVueltas = 0;
  while (!eligioVictima)
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, proceso->posicionDelPunteroDeSustitucion);
    if (!pagina->uso)
    {
      if (!pagina->modificado || contadorDeVueltas == 3 || contadorDeVueltas == 1)
      {
        eligioVictima = true;
        log_info(logger, "Victima elegida pagina %d del proceso %d", pagina->numeroPagina, proceso->idProceso);
        marcoSustituido = desalojar_pagina(proceso, pagina);
      }
    }
    else if (contadorDeVueltas == 1)
    {
      pagina->uso = false;
    }

    proceso->posicionDelPunteroDeSustitucion++;
    if (proceso->posicionDelPunteroDeSustitucion == numeroDePaginasAsignadas)
    {
      proceso->posicionDelPunteroDeSustitucion = 0;
    }
    if (proceso->posicionDelPunteroDeSustitucion == posicionInicialDelPuntero)
    {
      contadorDeVueltas++;
    }
  }
  return marcoSustituido;
}
//

Marco *desalojar_pagina(Proceso *proceso, Pagina *pagina)
{
  Marco *marcoDesasignado = pagina->marcoAsignado;
  if (pagina->modificado)
  {
    escribir_en_swap(pagina, proceso);
  }
  desasignar_pagina(proceso, pagina);
  return marcoDesasignado;
}

void desasignar_marco(Marco *marco) // TODO: Ver si necesita sincronizarse
{
  marco->paginaActual = NULL;
  marco->idProceso = -1;
}

void desasignar_pagina(Proceso *proceso, Pagina *pagina)
{
  desasignar_marco(pagina->marcoAsignado);
  pagina->marcoAsignado = NULL;
  pagina->modificado = false;
  sacar_pagina_de_paginas_asignadas(proceso, pagina);
}

void sacar_pagina_de_paginas_asignadas(Proceso *proceso, Pagina *pagina)
{
  bool es_pagina(void *_paginaLista)
  {
    Pagina *paginaLista = (Pagina *)_paginaLista;
    return paginaLista == pagina;
  }

  list_remove_by_condition(proceso->paginasAsignadas, &es_pagina);
}

void suspender_proceso(int idProcesoASuspender)
{
  Logger *logger = iniciar_logger_memoria();

  Proceso *proceso = buscar_proceso_por_id(idProcesoASuspender);
  log_info(logger, "Iniciando suspension del proceso %d", proceso->idProceso);

  while (!list_is_empty(proceso->paginasAsignadas))
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, 0);
    desalojar_pagina(proceso, pagina);
  }

  proceso->posicionDelPunteroDeSustitucion = 0;

  log_info(logger, "Proceso %d suspendido", proceso->idProceso);
  log_destroy(logger);
}

void borrar_tablas_del_proceso(Proceso *proceso)
{
  int entradas = MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  for (int numeroDeTablaDeSegundoNivel = 0; numeroDeTablaDeSegundoNivel < entradas; numeroDeTablaDeSegundoNivel++)
  {
    TablaSegundoNivel *tablaSegundoNivel = list_get(proceso->tablaPrimerNivel->entradas, numeroDeTablaDeSegundoNivel);

    for (int entradaDePagina = 0; entradaDePagina < entradas; entradaDePagina++)
    {
      Pagina *pagina = list_get(tablaSegundoNivel->entradas, entradaDePagina);

      free(pagina);
    }

    list_destroy(tablaSegundoNivel->entradas);
    free(tablaSegundoNivel);
  }

  list_destroy(proceso->tablaPrimerNivel->entradas);
  list_destroy(proceso->paginasAsignadas);
  free(proceso->tablaPrimerNivel);
}

void eliminar_proceso_de_lista_de_procesos(Proceso *proceso)
{
  pthread_mutex_lock(&semaforoProcesos);

  bool es_proceso(void *_procesoLista)
  {
    Proceso *procesoLista = (Proceso *)_procesoLista;
    return procesoLista == proceso;
  }

  list_remove_and_destroy_by_condition(procesos, &es_proceso, &free);

  pthread_mutex_unlock(&semaforoProcesos);
}

void desasignar_marcos_al_proceso(Proceso *proceso)
{
  int cantidadDePaginasAsignadas = list_size(proceso->paginasAsignadas);
  for (int numeroPaginaAsignada = 0; numeroPaginaAsignada < cantidadDePaginasAsignadas; numeroPaginaAsignada++)
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, numeroPaginaAsignada);
    desasignar_marco(pagina->marcoAsignado);
  }
}

void finalizar_proceso(int idProcesoAFinalizar)
{
  Logger *logger = iniciar_logger_memoria();
  Proceso *proceso = buscar_proceso_por_id(idProcesoAFinalizar);

  borrar_archivo_swap_del_proceso(proceso);
  desasignar_marcos_al_proceso(proceso);
  borrar_tablas_del_proceso(proceso);
  eliminar_proceso_de_lista_de_procesos(proceso);

  log_info(logger, "Proceso %d finalizado", idProcesoAFinalizar);
  log_destroy(logger);
}

void destruir_semaforos()
{
  // sem_destroy(&semaforoMarcos);
  pthread_mutex_destroy(&semaforoProcesos);
}

void liberar_memoria()
{
  Logger *logger = iniciar_logger_memoria();

  list_destroy_and_destroy_elements(marcos, &free);
  free(memoriaPrincipal);
  list_destroy(procesos);
  destruir_semaforos();

  log_info(logger, "Estructuras de memoria liberadas");
  log_destroy(logger);
}

int obtener_numero_tabla_segundo_nivel(int numeroTablaPrimerNivel, int entradaATablaDePrimerNivel)
{
  return numeroTablaPrimerNivel * MEMORIA_CONFIG.ENTRADAS_POR_TABLA + entradaATablaDePrimerNivel;
}

int obtener_numero_marco(int numeroTablaSegundoNivel, int entradaATablaDeSegundoNivel)
{
  Proceso *proceso = buscar_proceso_de_tabla_segundo_nivel_numero(numeroTablaSegundoNivel);

  if (proceso != NULL)
  {
    int numeroTablaSegundoNivelBuscada = numeroTablaSegundoNivel % MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
    TablaSegundoNivel *tablaSegundoNivelBuscada = list_get(proceso->tablaPrimerNivel->entradas, numeroTablaSegundoNivelBuscada);

    Pagina *paginaBuscada = list_get(tablaSegundoNivelBuscada->entradas, entradaATablaDeSegundoNivel);

    if (paginaBuscada->marcoAsignado == NULL)
    {
      contadorPageFaults++;
      asignar_pagina_a_marco_libre(proceso, paginaBuscada);
    }
    return paginaBuscada->marcoAsignado->numeroMarco;
  }
  else
    return -1; // proceso no encontrado => marco no encontrado
}

Proceso *buscar_proceso_de_tabla_segundo_nivel_numero(int numeroTablaSegundoNivel)
{
  int entradas = MEMORIA_CONFIG.ENTRADAS_POR_TABLA;

  pthread_mutex_lock(&semaforoProcesos);

  int numeroProcesos = list_size(procesos);
  for (int numeroProceso = 0; numeroProceso < numeroProcesos; numeroProceso++)
  {
    Proceso *proceso = list_get(procesos, numeroProceso);
    for (int entradaTablaPrimerNivel = 0; entradaTablaPrimerNivel < entradas; entradaTablaPrimerNivel++)
    {
      TablaSegundoNivel *tablaSegundoNivel = list_get(proceso->tablaPrimerNivel->entradas, entradaTablaPrimerNivel);
      if (tablaSegundoNivel->numeroTablaSegundoNivel == numeroTablaSegundoNivel)
      {
        pthread_mutex_unlock(&semaforoProcesos);
        return proceso;
      }
    }
  }

  // si llego aca es porque no existe dicho proceso
  pthread_mutex_unlock(&semaforoProcesos);
  Logger *logger = iniciar_logger_memoria();
  log_error(logger, "No existe proceso de numero de tabla de segundo nivel %d", numeroTablaSegundoNivel);
  log_destroy(logger);

  return NULL;
}

void destruir_hilos(Hilo hiloCliente1, Hilo hiloCliente2)
{
  pthread_detach(hiloCliente1);
  pthread_detach(hiloCliente2);
}