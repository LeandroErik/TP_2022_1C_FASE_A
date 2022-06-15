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
void iniciar_marcos(int cantidadMarcos)
{
  marcos = list_create();
  for (int i=0; i<cantidadMarcos; i++){
    Marco* marco = malloc(sizeof(Marco));
    marco->idProceso = -1;
    marco->paginaActual = NULL;
    list_add(marcos, marco);
  }
}

Proceso *crear_proceso(int id, int tamanio)
{
  Proceso *proceso = malloc(sizeof(proceso));
  proceso->idProceso = id;
  proceso->tamanio = tamanio;
  proceso->tablaPrimerNivel = crear_tabla_primer_nivel();

  return proceso;
}

int agregar_proceso(Proceso *proceso)
{
  return list_add(procesos, proceso);
}

TablaPrimerNivel *crear_tabla_primer_nivel()
{
  TablaPrimerNivel *tabla = malloc(sizeof(TablaPrimerNivel));
  tabla->entradas = list_create();
  tabla->nroTablaPrimerNivel = tablasDePrimerNivel;
  tablasDePrimerNivel++;

  for (int i = 0; i < MEMORIA_CONFIG.ENTRADAS_POR_TABLA; i++)
  {
    TablaSegundoNivel* tablaSegundoNivel = crear_tabla_segundo_nivel();
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

Proceso *buscar_proceso_por_id(int idProceso)
{

  bool cmpProceso(void *_proceso)
  {
    Proceso *proceso = (Proceso *)_proceso;
    return proceso->idProceso == idProceso;
  }

  return list_find(procesos, &cmpProceso);
}

void escribir_memoria(int idProceso, uint32_t valorAEscribir, int desplazamiento)
{
  Logger *log = iniciar_logger_memoria();

  Proceso *proceso = buscar_proceso_por_id(idProceso);
  log_info(log, "id proceso: %d", proceso->idProceso);

  int nroMarco = obtener_numero_de_marco(desplazamiento);
  log_info(log, "numero de marco: %d", nroMarco);

  //asignar_marco(proceso, nroMarco);

  void *n = memoriaPrincipal + desplazamiento;
  memcpy(n, &valorAEscribir, 4);
}

uint32_t leer_de_memoria(int idProceso, int desplazamiento)
{
  Logger *log = iniciar_logger_memoria();
  Proceso *proceso = buscar_proceso_por_id(idProceso);
  log_info(log, "id proceso: %d", proceso->idProceso);

  uint32_t leido;
  void *n = memoriaPrincipal + desplazamiento;
  memcpy(&leido, n, 4);
  return leido;
}

int obtener_numero_de_marco(int desplazamiento)
{
  return desplazamiento / MEMORIA_CONFIG.TAM_PAGINA;
}

Marco* asignar_pagina_a_marco(Proceso* proceso, int nroPagina, int nroMarco){

  Logger *log = iniciar_logger_memoria();

  //double d = (double) nroPagina / MEMORIA_CONFIG.ENTRADAS_POR_TABLA;
  int numeroDeEntradaDelPrimerNivel = nroPagina / MEMORIA_CONFIG.ENTRADAS_POR_TABLA; //floor(d);
  int numeroDeEntradaDelSegundoNivel = nroPagina % MEMORIA_CONFIG.ENTRADAS_POR_TABLA; //checkear el %

  TablaSegundoNivel* tablaSegundoNivel = list_get(proceso->tablaPrimerNivel->entradas, numeroDeEntradaDelPrimerNivel);

  Pagina *pagina = list_get(tablaSegundoNivel->entradas, numeroDeEntradaDelSegundoNivel);

  Marco* marco = list_get(marcos, nroMarco);
  pagina->marcoAsignado = marco;

  marco->idProceso = proceso->idProceso;
  marco->paginaActual = pagina;

  log_info(log, "Pagina %d asignada al Marco %d", nroPagina, nroMarco);

  return marco;
}

// void asignar_marco(Proceso *proceso, int nroMarco)
// {
//   Logger *log = iniciar_logger_memoria();

//   if (list_is_empty(proceso->tabla->entradas))
//   {
//     TablaSegundoNivel *tablaSegundoNivel = crear_tabla_segundo_nivel();

//     list_add(proceso->tabla->entradas, tablaSegundoNivel);
//     Pagina *pagina = list_get(tablaSegundoNivel->entradas, 0);

//     Marco* marco = list_get(marcos, nroMarco);
//     pagina->marcoAsignado = marco;

//     marco->idProceso = proceso->idProceso;
//     marco->paginaActual = pagina;

//     log_info(log, "Tabla de segundo nivel creada correctamente");
//   }
//   else
//   {
//     log_info(log, "Nada");
//   } 
// }