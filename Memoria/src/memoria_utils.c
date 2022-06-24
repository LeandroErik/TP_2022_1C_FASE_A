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
  memoriaPrincipal = (void *)malloc(MEMORIA_CONFIG.TAM_MEMORIA);
  memset(memoriaPrincipal, '0', MEMORIA_CONFIG.TAM_MEMORIA);
  int cantidadMarcos = MEMORIA_CONFIG.TAM_MEMORIA / MEMORIA_CONFIG.TAM_PAGINA;
  iniciar_marcos(cantidadMarcos);
  procesos = list_create();

  log_info(logger, "Estructuras de memoria inicializadas");
  log_destroy(logger);
}

void iniciar_marcos(int cantidadMarcos)
{
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

  list_add(procesos, proceso);

  log_destroy(logger);

  return proceso;
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

  int entradasPorTabla = MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  for (int i = 0; i < entradasPorTabla; i++)
  {
    Pagina *pag = malloc(sizeof(Pagina));
    pag->numeroPagina = i + nroPrimerPaginaDeTabla;
    pag->paginaVacia = true;
    pag->modificado = false;
    pag->uso = true;
    pag->marcoAsignado = NULL;
    list_add(tabla->entradas, pag);
  }
  return tabla;
}
// Fin funciones de estructuras

// Funciones de escritura, lectura y copiado en memoria fisica
void escribir_entero_en_memoria(uint32_t valorAEscribir, int direccionFisica)
{
  Logger *logger = iniciar_logger_memoria();

  void *direccionInicioEscritura = memoriaPrincipal + direccionFisica;
  memcpy(direccionInicioEscritura, &valorAEscribir, sizeof(valorAEscribir));

  log_info(logger, "Valor escrito %d, en la posicion de memoria fisica %d.", valorAEscribir, direccionFisica);
  log_destroy(logger);
}

uint32_t leer_entero_de_memoria(int direccionFisica)
{
  Logger *logger = iniciar_logger_memoria();

  uint32_t leido;
  void *direccionInicioLectura = memoriaPrincipal + direccionFisica;
  memcpy(&leido, direccionInicioLectura, sizeof(leido));

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
  bool es_proceso(void *_proceso)
  {
    Proceso *proceso = (Proceso *)_proceso;
    return proceso->idProceso == idProceso;
  }

  return list_find(procesos, &es_proceso);
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

void asignar_pagina_del_proceso_al_marco(Proceso *proceso, Pagina *pagina, Marco *marco)
{
  Logger *logger = iniciar_logger_memoria();

  if (!pagina->paginaVacia) // Para saber si hay que traer info de swap o no
  {
    escribir_datos_de_pagina_en_memoria(proceso, pagina->numeroPagina, marco->numeroMarco);
  }

  list_add(proceso->paginasAsignadas, pagina); 
  // TODO: deberia agregarse en la posicion de la lista en la que se sustituyo (que es la del cont-1) excepto si es 0 o si no se sustituyo (que seria al final) 
  pagina->marcoAsignado = marco;
  marco->paginaActual = pagina;
  marco->idProceso = proceso->idProceso;

  log_info(logger, "Pagina %d del proceso %d, asignada al Marco %d", pagina->numeroPagina, proceso->idProceso, marco->numeroMarco);
  log_destroy(logger);
}

Pagina *obtener_pagina_del_proceso(Proceso *proceso, int numeroPagina) // Tambien se podria buscar el numeroPagina en las tablas del proceso, uso cuenta de la MMU
{
  // double numeroEntradasPrimerNivel = numeroPagina / MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  int numeroDeEntradaDelPrimerNivel = floor(numeroPagina / MEMORIA_CONFIG.ENTRADAS_POR_TABLA);
  int numeroDeEntradaDelSegundoNivel = numeroPagina % MEMORIA_CONFIG.ENTRADAS_POR_TABLA;

  TablaSegundoNivel *tablaSegundoNivel = list_get(proceso->tablaPrimerNivel->entradas, numeroDeEntradaDelPrimerNivel);

  return list_get(tablaSegundoNivel->entradas, numeroDeEntradaDelSegundoNivel);
}

bool hay_que_sustituir_pagina_del_proceso(Proceso *proceso, Marco *marcoLibre)
{
  return !tiene_marcos_por_asignar(proceso) || marcoLibre == NULL;
}

Marco *asignar_pagina_a_marco_libre(Proceso *proceso, int numeroPagina) // TODO: Ver cuando se llamaria, cuando se implemente comunicacion con CPU
{
  Logger *logger = iniciar_logger_memoria();
  Pagina *pagina = obtener_pagina_del_proceso(proceso, numeroPagina);

  Marco *marco = primer_marco_libre();
  if (hay_que_sustituir_pagina_del_proceso(proceso, marco))
  {
    log_info(logger, "No se pudo asignar la pagina %d del proceso %d, se debe realizar una sustitucion", numeroPagina, proceso->idProceso);
    marco = marco_del_proceso_sustituido(proceso);
  }
  asignar_pagina_del_proceso_al_marco(proceso, pagina, marco);

  log_destroy(logger);
  return marco;
}

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

  int contadorDeVueltas = 0;
  while (!eligioVictima)
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, proceso->posicionDelPunteroDeSustitucion);
    if (!pagina->uso)
    {
      if (!pagina->modificado || contadorDeVueltas == 3)
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
      contadorDeVueltas++;
    }
  }

  return marcoSustituido;
}

Marco *desalojar_pagina(Proceso *proceso, Pagina *pagina)
{
  Marco *marcoDesasignado = pagina->marcoAsignado;
  escribir_en_swap(pagina, proceso);
  desasignar_pagina(proceso, pagina); 
  return marcoDesasignado;
}

void desasignar_marco(Marco *marco)
{
  marco->paginaActual = NULL;
  marco->idProceso = -1;
}

void desasignar_pagina(Proceso *proceso, Pagina *pagina)
{
  desasignar_marco(pagina->marcoAsignado);
  pagina->marcoAsignado = NULL;
  pagina->modificado = false;
  pagina->uso = true; // TODO: checkear bits
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

  while ( !list_is_empty(proceso->paginasAsignadas) )
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, 0);
    if (pagina->modificado)
    {
      escribir_en_swap(pagina, proceso);
    }
    desasignar_pagina(proceso, pagina);
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
  bool es_proceso(void *_procesoLista)
  {
    Proceso *procesoLista = (Proceso *)_procesoLista;
    return procesoLista == proceso;
  }

  list_remove_and_destroy_by_condition(procesos, &es_proceso, &free);
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

void liberar_memoria()
{
  Logger *logger = iniciar_logger_memoria();

  list_destroy_and_destroy_elements(marcos, &free);
  free(memoriaPrincipal);
  list_destroy(procesos);

  log_info(logger, "Estructuras de memoria liberadas");
  log_destroy(logger);
}