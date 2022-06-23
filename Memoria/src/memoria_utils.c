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
//

Proceso *buscar_proceso_por_id(int idProceso)
{

  bool es_proceso(void *_proceso)
  {
    Proceso *proceso = (Proceso *)_proceso;
    return proceso->idProceso == idProceso;
  }

  return list_find(procesos, &es_proceso);
}

// Funciones de escritura, lectura y copiado en memoria fisica
void *leer_de_memoria(int direccionFisica, int desplazamiento)
{
  void *leido;
  void *direccionInicioDeLectura = memoriaPrincipal + direccionFisica;
  memcpy(&leido, direccionInicioDeLectura, desplazamiento);

  return leido;
}

void escribir_en_memoria(void *datosAEscribir, int direccionFisica, int desplazamiento)
{
  void *direccionInicioDeEscritura = memoriaPrincipal + direccionFisica;
  memcpy(direccionInicioDeEscritura, &datosAEscribir, desplazamiento);
}

void escribir_entero_en_memoria(uint32_t valorAEscribir, int direccionFisica)
{
  Logger *logger = iniciar_logger_memoria();

  escribir_en_memoria((void *)valorAEscribir, direccionFisica, sizeof(uint32_t));

  log_info(logger, "Valor escrito %d, en la posicion de memoria fisica %d.", valorAEscribir, direccionFisica);
  log_destroy(logger);
}

uint32_t leer_entero_de_memoria(int direccionFisica)
{
  Logger *logger = iniciar_logger_memoria();

  uint32_t leido = (uint32_t)leer_de_memoria(direccionFisica, sizeof(uint32_t));

  log_info(logger, "Valor leido %d, en la posicion de memoria fisica %d.", leido, direccionFisica);
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
  bool tiene_pagina_asignada_del_proceso(void *_marco)
  {
    Marco *marco = (Marco *)_marco;
    return marco->idProceso == proceso->idProceso;
  }

  int marcosAsignados = list_count_satisfying(marcos, &tiene_pagina_asignada_del_proceso);

  return marcosAsignados < MEMORIA_CONFIG.MARCOS_POR_PROCESO;
}

void asignar_pagina_del_proceso_al_marco(Proceso *proceso, Pagina *pagina, Marco *marco)
{
  Logger *logger = iniciar_logger_memoria();

  if (!pagina->paginaVacia)
  {
    escribir_datos_de_pagina_en_memoria(proceso, pagina->numeroPagina, marco->numeroMarco);
    // char *memoriaComoString = (char *)memoriaPrincipal;
    // log_info(logger, "Memoria princial: %s", memoriaComoString);
  }

  list_add(proceso->paginasAsignadas, pagina);
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

Marco *asignar_pagina_a_marco_libre(Proceso *proceso, int numeroPagina) // TODO: Ver cuando se llamaria
{
  Pagina *pagina = obtener_pagina_del_proceso(proceso, numeroPagina);
  Marco *marco;

  if (tiene_marcos_por_asignar(proceso))
  {
    marco = primer_marco_libre();
    if (marco != NULL)
    {
      asignar_pagina_del_proceso_al_marco(proceso, pagina, marco);
    }
    else // memoria llena
    {
      marco = correr_algortimo_sustitucion(proceso);
      asignar_pagina_del_proceso_al_marco(proceso, pagina, marco);
    }
  }
  else // tiene asignada la cantidad maxima de marcos por proceso
  {
    marco = correr_algortimo_sustitucion(proceso);
    asignar_pagina_del_proceso_al_marco(proceso, pagina, marco);
  }

  return marco;
}

Marco *correr_algortimo_sustitucion(Proceso *proceso)
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
  int paginasAsignadas = list_size(proceso->paginasAsignadas);
  while (!eligioVictima)
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, proceso->posicionDelPunteroDeSustitucion);
    if (!pagina->uso)
    {
      eligioVictima = true;
      marcoSustituido = desalojar_pagina(proceso, pagina);
      log_info(logger, "Victima elegida pagina %d del proceso %d", pagina->numeroPagina, proceso->idProceso);
    }
    else
    {
      pagina->uso = false;
    }

    proceso->posicionDelPunteroDeSustitucion++;
    if (proceso->posicionDelPunteroDeSustitucion == paginasAsignadas)
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
  int paginasAsignadas = list_size(proceso->paginasAsignadas);

  int contadorDeVueltas = 0;
  while (!eligioVictima)
  {
    Pagina *pagina = list_get(proceso->paginasAsignadas, proceso->posicionDelPunteroDeSustitucion);
    if (!pagina->uso)
    {
      if (!pagina->modificado || contadorDeVueltas == 3)
      {
        eligioVictima = true;
        marcoSustituido = desalojar_pagina(proceso, pagina);
        log_info(logger, "Victima elegida pagina %d del proceso %d", pagina->numeroPagina, proceso->idProceso);
      }
    }
    else if (contadorDeVueltas == 1)
    {
      pagina->uso = false;
    }

    proceso->posicionDelPunteroDeSustitucion++;
    if (proceso->posicionDelPunteroDeSustitucion == paginasAsignadas)
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
  desasignar_pagina(pagina);
  return marcoDesasignado;
}

void desasignar_marco(Marco *marco)
{
  marco->paginaActual = NULL;
  marco->idProceso = -1;
}

void desasignar_pagina(Pagina *pagina)
{
  desasignar_marco(pagina->marcoAsignado);
  pagina->marcoAsignado = NULL;
  pagina->modificado = false;
  pagina->uso = true; // TODO: checkear bits
}

void suspender_proceso(int idProcesoASuspender)
{
  Logger *logger = iniciar_logger_memoria();

  Proceso *proceso = buscar_proceso_por_id(idProcesoASuspender);

  log_info(logger, "Iniciando suspension del proceso %d", proceso->idProceso);

  int entradas = MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  for (int numeroDeTablaDeSegundoNivel = 0; numeroDeTablaDeSegundoNivel < entradas; numeroDeTablaDeSegundoNivel++)
  {
    TablaSegundoNivel *tablaSegundoNivel = list_get(proceso->tablaPrimerNivel->entradas, numeroDeTablaDeSegundoNivel);

    for (int entradaDePagina = 0; entradaDePagina < entradas; entradaDePagina++)
    {
      Pagina *pagina = list_get(tablaSegundoNivel->entradas, entradaDePagina);
      if (pagina->marcoAsignado != NULL)
      {
        if (pagina->modificado)
        {
          escribir_en_swap(pagina, proceso);
        }
        desasignar_pagina(pagina);
      }
    }
  }

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

    free(tablaSegundoNivel);
  }

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

void desasignar_marcos_al_proceso(int idProceso)
{
  void limpiar_marco_si_lo_tiene_el_proceso(void *_marco)
  {
    Marco *marco = (Marco *)_marco;
    if (marco->idProceso == idProceso)
    {
      desasignar_marco(marco);
    }
  }

  list_iterate(marcos, &limpiar_marco_si_lo_tiene_el_proceso);
}

void finalizar_proceso(int idProcesoAFinalizar)
{
  Logger *logger = iniciar_logger_memoria();
  Proceso *proceso = buscar_proceso_por_id(idProcesoAFinalizar);

  borrar_tablas_del_proceso(proceso);
  borrar_archivo_swap_del_proceso(proceso);
  eliminar_proceso_de_lista_de_procesos(proceso);
  desasignar_marcos_al_proceso(idProcesoAFinalizar);

  log_info(logger, "Proceso %d finalizado", idProcesoAFinalizar);
  log_destroy(logger);
}

void liberar_memoria()
{
  Logger *logger = iniciar_logger_memoria();

  list_destroy_and_destroy_elements(marcos, &free);

  free(memoriaPrincipal);
  free(procesos);

  log_info(logger, "Estructuras de memoria liberadas");
  log_destroy(logger);
}