#ifndef MEMORIA_CONFIG_H
#define MEMORIA_CONFIG_H

#include <protocolo.h>

typedef struct MemoriaConfig
{
  char *IP_MEMORIA;
  char *PUERTO_MEMORIA;
  int TAM_MEMORIA;
  int TAM_PAGINA;
  int ENTRADAS_POR_TABLA;
  int RETARDO_MEMORIA;
  char *ALGORITMO_REEMPLAZO;
  int MARCOS_POR_PROCESO;
  int RETARDO_SWAP;
  char *PATH_SWAP;
} MemoriaConfig;

MemoriaConfig MEMORIA_CONFIG;

/**
 * @brief Rellena la variable MEMORIA_CONFIG con las configuraciones de la memoria por archivo.
 *
 * @param config Configuración.
 */
void rellenar_config_memoria(Config *config);
#endif