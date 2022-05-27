#ifndef CPU_CONFIG_H
#define CPU_CONFIG_H

#include <protocol.h>

typedef struct CpuConfig
{
  int TLB_INPUTS;
  char *TLB_REPLACE;
  int NOOP_DELAY;
  char *IP_MEMORY;
  char *PORT_MEMORY;
  char *PORT_KERNEL_DISPATCH;
  char *PORT_KERNEL_INTERRUPT;
} CpuConfig;

CpuConfig CPU_CONFIG;

/**
 * @brief Fills CPU_CONFIG variable with CPU configurations by file.
 *
 * @param config Configuration object.
 */
void fill_cpu_config(Config *config);
#endif