#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

#include <protocol.h>

typedef struct MemoryConfig
{
  char *IP_MEMORY;
  char *PORT_MEMORY;
  int MEMORY_SIZE;
  int PAGE_SIZE;
  int PAGES_BY_TABLE;
  int MEMORY_DELAY;
  char *REPLACE_ALGORITHM;
  int PAGE_FRAME_BY_PROCESS;
  int SWAP_DELAY;
  char *PATH_SWAP;
} MemoryConfig;

MemoryConfig MEMORY_CONFIG;

/**
 * @brief Fills MEMORY_CONFIG variable with MEMORY configurations by file.
 *
 * @param config Configuration object.
 */
void fill_memory_config(Config *config);
#endif