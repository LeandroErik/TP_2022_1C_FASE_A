#include <cpu_config.h>

void obtener_configuraciones_cpu(void)
{
  t_config *config = config_create("CPU.config");
  CPU_CONFIG.ENTRADAS_TLB = config_get_string_value(config, "ENTRADAS_TLB");
  CPU_CONFIG.REEMPLAZO_TLB = config_get_string_value(config, "REEMPLAZO_TLB");
  CPU_CONFIG.RETARDO_NOOP = config_get_string_value(config, "RETARDO_NOOP");
  CPU_CONFIG.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
  CPU_CONFIG.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
  CPU_CONFIG.PUERTO_ESCUCHA_DISPATCH = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
  CPU_CONFIG.PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
}