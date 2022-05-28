#include <consola_config.h>

void rellenar_configuracion_consola(Config *config)
{
  CONSOLA_CONFIG.IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
  CONSOLA_CONFIG.PUERTO_KERNEL = config_get_string_value(config, "PUERTO_KERNEL");
}