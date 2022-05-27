#include <kernel_config.h>

void fill_kernel_config(Config *config)
{
  KERNEL_CONFIG.IP = config_get_string_value(config, "IP");
  KERNEL_CONFIG.PORT_MEMORY = config_get_string_value(config, "PORT_MEMORY");
  KERNEL_CONFIG.PORT_CPU_DISPATCH = config_get_string_value(config, "PORT_CPU_DISPATCH");
  KERNEL_CONFIG.PORT_CPU_INTERRUPT = config_get_string_value(config, "PORT_CPU_INTERRUPT");
  KERNEL_CONFIG.PORT_KERNEL = config_get_string_value(config, "PORT_KERNEL");
  KERNEL_CONFIG.PLANNING_ALGORITHM = config_get_string_value(config, "PLANNING_ALGORITHM");
  KERNEL_CONFIG.INITIAL_ESTIMATION = config_get_int_value(config, "INITIAL_ESTIMATION");
  KERNEL_CONFIG.ALPHA = config_get_double_value(config, "ALPHA");
  KERNEL_CONFIG.MULTIPROGRAMMING_LEVEL = config_get_int_value(config, "MULTIPROGRAMMING_LEVEL");
  KERNEL_CONFIG.MAX_TIME_BLOCKED = config_get_int_value(config, "MAX_TIME_BLOCKED");
}