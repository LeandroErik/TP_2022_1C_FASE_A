#ifndef CONSOLE_CONFIG_H
#define CONSOLE_CONFIG_H

#include <protocol.h>

typedef struct ConsoleConfig
{
  char *IP_KERNEL;
  char *PORT_KERNEL;
} ConsoleConfig;

ConsoleConfig CONSOLE_CONFIG;

/**
 * @brief Fills CONSOLE_CONFIG variable with console configurations by file.
 *
 * @param config Configuration object.
 */
void fill_console_config(Config *config);
#endif