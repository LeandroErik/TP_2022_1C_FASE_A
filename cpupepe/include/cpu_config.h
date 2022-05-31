#ifndef CPU_CONFIG_H
#define CPU_CONFIG_H

#include <commons/config.h>

typedef struct CPU_Config
{
  char *ENTRADAS_TLB;
  char *REEMPLAZO_TLB;
  char *RETARDO_NOOP;
  char *IP_MEMORIA;
  char *IP_CPU;
  char *PUERTO_MEMORIA;
  char *PUERTO_ESCUCHA_DISPATCH;
  char *PUERTO_ESCUCHA_INTERRUPT;

} CPU_Config;

CPU_Config CPU_CONFIG;

/**
 * @brief Obtiene los parámetros dados por el archivo de configuración "CPU.config"
 *
 */
void obtener_configuraciones_cpu(void);
#endif