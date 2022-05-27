#include <cpu_config.h>

void fill_cpu_config(Config *config)
{
  CPU_CONFIG.TLB_INPUTS = config_get_int_value(config, "TLB_INPUTS");
  CPU_CONFIG.TLB_REPLACE = config_get_string_value(config, "TLB_REPLACE");
  CPU_CONFIG.NOOP_DELAY = config_get_int_value(config, "NOOP_DELAY");
  CPU_CONFIG.IP_MEMORY = config_get_string_value(config, "IP_MEMORY");
  CPU_CONFIG.PORT_MEMORY = config_get_string_value(config, "PORT_MEMORY");
  CPU_CONFIG.PORT_KERNEL_DISPATCH = config_get_string_value(config, "PORT_KERNEL_DISPATCH");
  CPU_CONFIG.PORT_KERNEL_INTERRUPT = config_get_string_value(config, "PORT_KERNEL_INTERRUPT");
}