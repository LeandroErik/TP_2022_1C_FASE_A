#include <memoria_utils.h>
#include <main.h>

int main(void)
{
  Config *config = config_create("Memoria.config");
  Logger *logger = iniciar_logger_memoria();

  rellenar_config_memoria(config);

  log_info(logger, "Iniciando Servidor Memoria...");
  int socketMemoria = iniciar_servidor_memoria();

  if (socketMemoria < 0)
  {
    log_error(logger, "Error intentando iniciar Servidor Memoria.");
    return EXIT_FAILURE;
  }

  log_info(logger, "Servidor Memoria iniciado correctamente.");

  // Estructuras de memoria
  tablasDePrimerNivel = 0;
  memoriaPrincipal = (void *)malloc(MEMORIA_CONFIG.TAM_MEMORIA);
  int cantidadMarcos = MEMORIA_CONFIG.TAM_MEMORIA / MEMORIA_CONFIG.TAM_PAGINA;

  iniciar_marcos(cantidadMarcos);
  procesos = list_create();

  // prueba

  int id = 0;
  int tamanio = 64 * 4; //bytes
  Proceso *procesoNuevo = crear_proceso(id, tamanio);
  agregar_proceso(procesoNuevo);
  log_info(logger, "Tabla de primer nivel del proceso %d, tamanio %d, tabla numero: %d", id, procesoNuevo->tamanio, procesoNuevo->tablaPrimerNivel->nroTablaPrimerNivel);

  asignar_pagina_a_marco(procesoNuevo, 3, 6);

  // escribir_memoria(id, 5, 100);
  // uint32_t leido = leer_de_memoria(id, 100);
  // log_info(logger, "Valor leido %d", leido);

  //  id=2;
  //  tamanio=15;
  // Proceso* procesoNuevo2 = crear_proceso(id,tamanio);
  // nroDeEntradaTablaPrimerNivel = agregar_proceso(procesoNuevo);
  // log_info(logger, "Tabla de primer nivel del proceso %d, tabla numero: %d", id, nroDeEntradaTablaPrimerNivel);

  //  id=3;
  //  tamanio=30;
  // Proceso* procesoNuevo3 = crear_proceso(id,tamanio);
  // nroDeEntradaTablaPrimerNivel = agregar_proceso(procesoNuevo);
  // log_info(logger, "Tabla de primer nivel del proceso %d, tabla numero: %d", id, nroDeEntradaTablaPrimerNivel);
  //
  //

  //

  // Hilos
  //  Hilo hiloCliente;

  // for (int i=0; i<2; i++)
  // {
  //   int socketCliente = esperar_cliente(socketMemoria);

  //   pthread_create(&hiloCliente, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);
  //   pthread_join(hiloCliente, NULL);
  // }

  return EXIT_SUCCESS;
}