#include <memoria_utils.h>
#include <main.h>

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
  int cantidadMarcos = MEMORIA_CONFIG.TAM_MEMORIA / MEMORIA_CONFIG.TAM_PAGINA;
  iniciar_marcos(cantidadMarcos);
  procesos = list_create();

  log_info(logger, "Estructuras de memoria inicializadas");
}

void iniciar_marcos(int cantidadMarcos)
{
  marcos = list_create();
  for (int i = 0; i < cantidadMarcos; i++)
  {
    Marco *marco = malloc(sizeof(Marco));
    marco->idProceso = -1;
    marco->paginaActual = NULL;
    list_add(marcos, marco);
  }
}

Proceso *crear_proceso(int id, int tamanio)
{
  Proceso *proceso = malloc(sizeof(Proceso));
  proceso->idProceso = id;
  proceso->tamanio = tamanio;
  proceso->tablaPrimerNivel = crear_tabla_primer_nivel(); 

  return proceso;
}

void agregar_proceso(Proceso *proceso)
{
  list_add(procesos, proceso);
}

TablaPrimerNivel *crear_tabla_primer_nivel()
{
  TablaPrimerNivel *tabla = malloc(sizeof(TablaPrimerNivel));
  tabla->entradas = list_create();
  tabla->nroTablaPrimerNivel = tablasDePrimerNivel;
  tablasDePrimerNivel++;

  for (int i = 0; i < MEMORIA_CONFIG.ENTRADAS_POR_TABLA; i++) //TODO checkeo de tamaño del proceso, quiza no hace falta completar toda la tabla
  {
    TablaSegundoNivel *tablaSegundoNivel = crear_tabla_segundo_nivel();
    list_add(tabla->entradas, tablaSegundoNivel);
  }

  return tabla;
}

TablaSegundoNivel *crear_tabla_segundo_nivel()
{
  TablaSegundoNivel *tabla = malloc(sizeof(TablaSegundoNivel));
  tabla->entradas = list_create();

  for (int i = 0; i < MEMORIA_CONFIG.ENTRADAS_POR_TABLA; i++)
  {
    Pagina *pag = malloc(sizeof(Pagina));
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

  bool cmpProceso(void *_proceso)
  {
    Proceso *proceso = (Proceso *)_proceso;
    return proceso->idProceso == idProceso;
  }

  return list_find(procesos, &cmpProceso);
}

int obtener_numero_de_marco(int desplazamiento)
{
  return desplazamiento / MEMORIA_CONFIG.TAM_PAGINA;
}

char* escribir_memoria(int idProceso, uint32_t valorAEscribir, int direccionFisica)
{
  Logger *log = iniciar_logger_memoria();

  //if(le pertenece el marco), escribir

  Proceso *proceso = buscar_proceso_por_id(idProceso);
  log_info(log, "id proceso: %d", proceso->idProceso);

  int nroMarco = obtener_numero_de_marco(direccionFisica);
  log_info(log, "numero de marco: %d", nroMarco);

  void *n = memoriaPrincipal + direccionFisica;
  memcpy(n, &valorAEscribir, 4);

  return "";//confirmacion
}

uint32_t leer_de_memoria(int idProceso, int direccionFisica)
{
  Logger *log = iniciar_logger_memoria();

  //if(le pertenece el marco), escribir

  Proceso *proceso = buscar_proceso_por_id(idProceso);
  log_info(log, "id proceso: %d", proceso->idProceso);

  uint32_t leido;
  void *n = memoriaPrincipal + direccionFisica;
  memcpy(&leido, n, 4);
  return leido;
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
  bool tiene_pagina_asignada_del_proceso(void *_marco)
  {
    Marco *marco = (Marco *)_marco;
    return marco->idProceso == proceso->idProceso;
  }

  return list_count_satisfying(marcos, &tiene_pagina_asignada_del_proceso) < MEMORIA_CONFIG.MARCOS_POR_PROCESO;
}

int numero_de_marco(Marco* marco)
{
  for(int i = 0; i<list_size(marcos); i++) //TODO ver si hay funcion de t_list que pueda usar
  {
    if(list_get(marcos, i) == marco){
      return i;
    }
  }

  return -1;
}

Marco *asignar_pagina_a_marco(Proceso *proceso, int nroPagina) //la llamaria luego de que la cpu me pida la entrada a TP segundo nivel?
{
  Logger *log = iniciar_logger_memoria();

  // double d = (double) nroPagina / MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  int numeroDeEntradaDelPrimerNivel = nroPagina / MEMORIA_CONFIG.ENTRADAS_POR_TABLA;  // floor(d);
  int numeroDeEntradaDelSegundoNivel = nroPagina % MEMORIA_CONFIG.ENTRADAS_POR_TABLA; // checkear el %

  TablaSegundoNivel *tablaSegundoNivel = list_get(proceso->tablaPrimerNivel->entradas, numeroDeEntradaDelPrimerNivel);

  Pagina *pagina = list_get(tablaSegundoNivel->entradas, numeroDeEntradaDelSegundoNivel);
  Marco *marco;

  if (tiene_marcos_por_asignar(proceso))
  {
    marco = primer_marco_libre();
    if (marco != NULL)
    {
      pagina->marcoAsignado = marco;
      marco->paginaActual = pagina;
      marco->idProceso = proceso->idProceso;

      log_info(log, "Pagina %d del proceso %d, asignada al Marco %d", nroPagina, proceso->idProceso, numero_de_marco(marco)); 
    }
    else //memoria llena
    {
      // correr algortimo sustitucion global
    }
  }
  else //tiene asignada la cantidad maxima de marcos por proceso
  {
    // correr algortimo sustitucion local
  }

  return marco;
}