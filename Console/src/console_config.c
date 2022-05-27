#include <console_config.h>

void fill_console_config(Config *config)
{
  CONSOLE_CONFIG.IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
  CONSOLE_CONFIG.PORT_KERNEL = config_get_string_value(config, "PORT_KERNEL");
}