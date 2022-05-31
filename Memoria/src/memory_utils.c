#include <memory_utils.h>

Logger *init_memory_logger(void)
{
  return log_create("Memory.log", "Memory", true, LOG_LEVEL_INFO);
}

int start_memory_server(void)
{
  return start_server(MEMORY_CONFIG.IP_MEMORY, MEMORY_CONFIG.PORT_MEMORY);
}