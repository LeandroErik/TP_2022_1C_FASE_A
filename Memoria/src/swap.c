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

  FILE* ficheroArchivoSwap = fopen(pathArchivoSwap, "w+r");
  
  return ficheroArchivoSwap;
}

void borrar_archivo_swap_del_proceso(Proceso *proceso)
{
  fclose(proceso->archivoSwap);
  //remove(generar_path_archivo_swap(proceso->idProceso));
}

void escribir_datos_de_pagina_en_memoria()
{
  // fseek(f,n*sizeof(T),SEEK_SET);
  // fwrite(xxxx, sizeof(), 1, archivoSwap);
}

void escribir_en_swap(Pagina *pagina, Proceso *proceso)
{
  Logger *logger = iniciar_logger_memoria();

  int numeroMarco = pagina->marcoAsignado->numeroMarco;
  void *direccionInicioMarco = memoriaPrincipal + numeroMarco * MEMORIA_CONFIG.TAM_PAGINA;

  int comienzoDeLaPosicionAEscribir = pagina->numeroPagina * MEMORIA_CONFIG.TAM_PAGINA;

  fseek(proceso->archivoSwap, comienzoDeLaPosicionAEscribir, SEEK_SET);
  fwrite(direccionInicioMarco, MEMORIA_CONFIG.TAM_PAGINA, 1, proceso->archivoSwap);

  log_info(logger, "Se swappeo la pagina %d", pagina->numeroPagina);
  log_destroy(logger);
}