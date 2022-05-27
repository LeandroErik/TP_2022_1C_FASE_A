#include <memory_config.h>

void fill_memory_config(Config *config)
{
  MEMORY_CONFIG.IP_MEMORY = config_get_string_value(config, "IP_MEMORY");
  MEMORY_CONFIG.PORT_MEMORY = config_get_string_value(config, "PORT_MEMORY");
  MEMORY_CONFIG.MEMORY_SIZE = config_get_int_value(config, "MEMORY_SIZE");
  MEMORY_CONFIG.PAGE_SIZE = config_get_int_value(config, "PAGE_SIZE");
  MEMORY_CONFIG.PAGES_BY_TABLE = config_get_int_value(config, "PAGES_BY_TABLE");
  MEMORY_CONFIG.MEMORY_DELAY = config_get_int_value(config, "MEMORY_DELAY");
  MEMORY_CONFIG.REPLACE_ALGORITHM = config_get_string_value(config, "REPLACE_ALGORITHM");
  MEMORY_CONFIG.PAGE_FRAME_BY_PROCESS = config_get_int_value(config, "PAGE_FRAME_BY_PROCESS");
  MEMORY_CONFIG.SWAP_DELAY = config_get_int_value(config, "SWAP_DELAY");
  MEMORY_CONFIG.PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
}