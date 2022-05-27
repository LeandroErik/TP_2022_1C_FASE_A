#ifndef KERNEL_CONFIG_H
#define KERNEL_CONFIG_H

#include <protocol.h>

typedef struct KernelConfig
{
  char *IP;
  char *PORT_MEMORY;
  char *PORT_CPU_DISPATCH;
  char *PORT_CPU_INTERRUPT;
  char *PORT_KERNEL;
  char *PLANNING_ALGORITHM;
  unsigned int INITIAL_ESTIMATION;
  float ALPHA;
  unsigned int MULTIPROGRAMMING_LEVEL;
  unsigned int MAX_TIME_BLOCKED;

} KernelConfig;

KernelConfig KERNEL_CONFIG;

/**
 * @brief Fills KERNEL_CONFIG variable with kernel configurations by file.
 *
 * @param config Configuration object.
 */
void fill_kernel_config(Config *config);
#endif