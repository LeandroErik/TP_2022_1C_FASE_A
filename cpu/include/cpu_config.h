#ifndef CPU_CONFIG_H
#define CPU_CONFIG_H

#include <protocolo.h>

typedef struct CpuConfig
{
  int ENTRADAS_TLB;
  char *REEMPLAZO_TLB;
  int RETARDO_NOOP;
  char *IP;
  char *PUERTO_MEMORIA;
  char *PUERTO_KERNEL_DISPATCH;
  char *PUERTO_KERNEL_INTERRUPT;
} CpuConfig;

CpuConfig CPU_CONFIG;

/**
 * @brief Rellena la variable CPU_CONFIG con las configuraciones de la CPU por archivo.
 *
 * @param config Configuración.
 */
void rellenar_configuracion_cpu(Config *config);
#endif