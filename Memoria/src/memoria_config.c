#include <memoria_config.h>

void rellenar_config_memoria(Config *config)
{
  MEMORIA_CONFIG.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
  MEMORIA_CONFIG.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
  MEMORIA_CONFIG.TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
  MEMORIA_CONFIG.TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
  MEMORIA_CONFIG.ENTRADAS_POR_TABLA = config_get_int_value(config, "ENTRADAS_POR_TABLA");
  MEMORIA_CONFIG.RETARDO_MEMORIA = config_get_int_value(config, "RETARDO_MEMORIA");
  MEMORIA_CONFIG.ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
  MEMORIA_CONFIG.MARCOS_POR_PROCESO = config_get_int_value(config, "MARCOS_POR_PROCESO");
  MEMORIA_CONFIG.RETARDO_SWAP = config_get_int_value(config, "RETARDO_SWAP");
  MEMORIA_CONFIG.PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
}