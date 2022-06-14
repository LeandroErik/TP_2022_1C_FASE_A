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

// tablas
Proceso *crear_proceso(int id, int tamanio)
{
  Proceso *proceso = malloc(sizeof(proceso));
  proceso->idProceso = id;
  proceso->tamanio = tamanio;
  proceso->tabla = crear_tabla_primer_nivel();

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
  Proceso *proceso = buscar_proceso_por_id(idProceso);

  Logger *log = iniciar_logger_memoria();
  log_info(log, "id proceso: %d", proceso->idProceso);

  void *n = memoriaPrincipal + desplazamiento;
  memcpy(n, &valorAEscribir, 4);
}

uint32_t leer_de_memoria(int idProceso, int desplazamiento)
{
  Proceso *proceso = buscar_proceso_por_id(idProceso);

  uint32_t leido;
  void *n = memoriaPrincipal + desplazamiento;
  memcpy(&leido, n, 4);
  return leido;
}