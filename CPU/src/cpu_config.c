#include <cpu_config.h>

void rellenar_configuracion_cpu(Config *config)
{
  CPU_CONFIG.ENTRADAS_TLB = config_get_int_value(config, "ENTRADAS_TLB");
  CPU_CONFIG.REEMPLAZO_TLB = config_get_string_value(config, "REEMPLAZO_TLB");
  CPU_CONFIG.RETARDO_NOOP = config_get_int_value(config, "RETARDO_NOOP");
  CPU_CONFIG.IP = config_get_string_value(config, "IP");
    CPU_CONFIG.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");

  CPU_CONFIG.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
  CPU_CONFIG.PUERTO_KERNEL_DISPATCH = config_get_string_value(config, "PUERTO_KERNEL_DISPATCH");
  CPU_CONFIG.PUERTO_KERNEL_INTERRUPT = config_get_string_value(config, "PUERTO_KERNEL_INTERRUPT");
}