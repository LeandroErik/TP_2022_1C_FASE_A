#ifndef CONSOLA_CONFIG_H
#define CONSOLA_CONFIG_H

#include <protocolo.h>

typedef struct ConsolaConfig
{
  char *IP_KERNEL;
  char *PUERTO_KERNEL;
} ConsolaConfig;

ConsolaConfig CONSOLA_CONFIG;

/**
 * @brief Rellena la variable CONSOLE_CONFIG con las configuraciones de la consola por archivo.
 *
 * @param config Configuración.
 */
void rellenar_configuracion_consola(Config *config);

#endif