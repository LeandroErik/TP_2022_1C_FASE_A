#include <swap.h>

char *generar_path_archivo_swap(int idProceso)
{
  return string_from_format("%s%s%s%s", MEMORIA_CONFIG.PATH_SWAP, "/", string_itoa(idProceso), ".swap");
}

FILE *crear_archivo_swap(int idProceso)
{
  Logger *logger = iniciar_logger_memoria();

  char *pathArchivoSwap = generar_path_archivo_swap(idProceso);

  log_info(logger, "Se creo el archivo de swap del proceso %d en el path %s", idProceso, pathArchivoSwap);
  log_destroy(logger);

  FILE* f = fopen(pathArchivoSwap, "w+r");
  
  return f;
}

void borrar_archivo_swap_del_proceso(Proceso *proceso)
{
  fclose(proceso->archivoSwap);
  remove(generar_path_archivo_swap(proceso->idProceso));
}

void *leer_pagina_de_archivo_swap(int numeroPagina, FILE *archivoSwap)
{
  void *pagina;
  return pagina;
  // TODO: implementar fwread
}

void escribir_datos_de_pagina_en_memoria()
{
  // fseek(f,n*sizeof(T),SEEK_SET);
  // fwrite(xxxx, sizeof(), 1, archivoSwap);
}

void escribir_datos_del_marco_en_archivo_swap(void *datosDelmarco, int numeroPagina, FILE *archivoSwap)
{
  int comienzoDeLaPosicionAEscribir = numeroPagina * MEMORIA_CONFIG.TAM_PAGINA;

  fseek(archivoSwap, comienzoDeLaPosicionAEscribir, SEEK_SET);
  fwrite(&datosDelmarco, MEMORIA_CONFIG.TAM_PAGINA, 1, archivoSwap);
}

void *leer_contenido_del_marco(int numeroMarco)
{
  int direccionFisicaInicioDelMarco = numeroMarco * MEMORIA_CONFIG.TAM_PAGINA;
  return leer_de_memoria(direccionFisicaInicioDelMarco, MEMORIA_CONFIG.TAM_PAGINA);
}

void escribir_en_swap(Pagina *pagina, Proceso *proceso)
{
  Logger *logger = iniciar_logger_memoria();

  int numeroMarco = numero_de_marco(pagina->marcoAsignado);
  int direccionFisicaInicioDelMarco = numeroMarco * MEMORIA_CONFIG.TAM_PAGINA;
  void *leido;
  void *direccionInicioDeLectura = memoriaPrincipal + direccionFisicaInicioDelMarco;
  //memcpy(&leido, direccionInicioDeLectura, MEMORIA_CONFIG.TAM_PAGINA);

  int comienzoDeLaPosicionAEscribir = pagina->nroPagina * MEMORIA_CONFIG.TAM_PAGINA;
  fseek(proceso->archivoSwap, comienzoDeLaPosicionAEscribir, SEEK_SET);
  fwrite(&leido, MEMORIA_CONFIG.TAM_PAGINA, 1, proceso->archivoSwap);

  log_info(logger, "Se swappeo la pagina %d", pagina->nroPagina);
  log_destroy(logger);
  free(leido);
}

// void escribir_en_swap(Pagina* pagina, Proceso *proceso)
// {
//   Logger* logger = iniciar_logger_memoria();

//   int numeroMarco = numero_de_marco(pagina->marcoAsignado);
//   void* datosDelmarco = leer_contenido_del_marco(numeroMarco);

//   escribir_datos_del_marco_en_archivo_swap(datosDelmarco, pagina->nroPagina, proceso->archivoSwap);

//   log_info(logger, "Se swappeo la pagina %d", pagina->nroPagina);
//   log_destroy(logger);
// }