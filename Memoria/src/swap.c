#include <swap.h>

char *generar_path_archivo_swap(int idProceso)
{
  return string_from_format("%s%s%d%s", MEMORIA_CONFIG.PATH_SWAP, "/", idProceso, ".swap");
}

FILE *crear_archivo_swap(int idProceso)
{
  Logger *logger = iniciar_logger_memoria();

  char *pathArchivoSwap = generar_path_archivo_swap(idProceso);

  log_info(logger, "Se creo el archivo de swap del proceso %d en el path %s", idProceso, pathArchivoSwap);
  log_destroy(logger);

  FILE *ficheroArchivoSwap = fopen(pathArchivoSwap, "w+");
  free(pathArchivoSwap);

  return ficheroArchivoSwap;
}

void borrar_archivo_swap_del_proceso(Proceso *proceso)
{
  // fclose(proceso->archivoSwap);
  char *pathArchivoSwap = generar_path_archivo_swap(proceso->idProceso);
  remove(pathArchivoSwap);
  free(pathArchivoSwap);
}

void escribir_datos_de_pagina_en_memoria(Proceso *proceso, int numeroPagina, int numeroMarco)
{
  Logger *logger = iniciar_logger_memoria();

  void *direccionInicioMarco = memoriaPrincipal + numeroMarco * MEMORIA_CONFIG.TAM_PAGINA;
  int comienzoDeLaPagina = numeroPagina * MEMORIA_CONFIG.TAM_PAGINA;

  fseek(proceso->archivoSwap, comienzoDeLaPagina, SEEK_SET);
  fread(direccionInicioMarco, MEMORIA_CONFIG.TAM_PAGINA, 1, proceso->archivoSwap);

  realizar_espera_swap();

  log_info(logger, "Se Leyo de swap la pagina %d del proceso %d y se escribio en el marco %d", numeroPagina, proceso->idProceso, numeroMarco);
  log_destroy(logger);
}

void escribir_en_swap(Pagina *pagina, Proceso *proceso)
{
  Logger *logger = iniciar_logger_memoria();

  int numeroMarco = pagina->marcoAsignado->numeroMarco;
  void *direccionInicioMarco = memoriaPrincipal + numeroMarco * MEMORIA_CONFIG.TAM_PAGINA;

  int comienzoDeLaPosicionAEscribir = pagina->numeroPagina * MEMORIA_CONFIG.TAM_PAGINA;

  fseek(proceso->archivoSwap, comienzoDeLaPosicionAEscribir, SEEK_SET);
  fwrite(direccionInicioMarco, MEMORIA_CONFIG.TAM_PAGINA, 1, proceso->archivoSwap);

  realizar_espera_swap();

  log_info(logger, "Se swappeo la pagina %d", pagina->numeroPagina);
  log_destroy(logger);
}

void realizar_espera_swap()
{
  int retardoSwap = MEMORIA_CONFIG.RETARDO_SWAP * 1000;
  usleep(retardoSwap);
}