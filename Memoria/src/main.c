#include <memoria_utils.h>
#include <main.h>

void correr_prueba(Logger *logger)
{
  Proceso *procesoNuevo1 = crear_proceso(0, 256); // id, tamanio
  agregar_proceso(procesoNuevo1);
  log_info(logger, "Proceso %d tamanio %d, tabla de primer nivel numero: %d", procesoNuevo1->idProceso, procesoNuevo1->tamanio, procesoNuevo1->tablaPrimerNivel->nroTablaPrimerNivel);

  asignar_pagina_a_marco(procesoNuevo1, 3);
  asignar_pagina_a_marco(procesoNuevo1, 1);
  asignar_pagina_a_marco(procesoNuevo1, 0);

  Proceso* procesoNuevo2 = crear_proceso(1, 192);
  agregar_proceso(procesoNuevo2);
  log_info(logger, "Proceso %d tamanio %d, tabla de primer nivel numero: %d", procesoNuevo2->idProceso, procesoNuevo2->tamanio, procesoNuevo2->tablaPrimerNivel->nroTablaPrimerNivel);

  asignar_pagina_a_marco(procesoNuevo2, 2);

  finalizar_proceso(procesoNuevo1->idProceso);
  finalizar_proceso(procesoNuevo2->idProceso);

  //Escritura, lectura y copia en memoria
  escribir_memoria(12, 100);
  leer_de_memoria(100);
  copiar_en_memoria(120,100);
  leer_de_memoria(120);

}

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

  iniciar_estructuras_memoria();

  // Prueba
  //correr_prueba(logger);

  //Hilos
  while(true)
  {
    int socketCliente = esperar_cliente(socketMemoria);
    Hilo hiloCliente;

    pthread_create(&hiloCliente, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);
    //pthread_join(hiloCliente, NULL);
  }

  liberar_memoria();
  log_destroy(logger);
  config_destroy(config);

  return EXIT_SUCCESS;
}