#include <memoria_utils.h>
#include <main.h>

void correr_prueba(Logger* logger){

  Proceso *procesoNuevo = crear_proceso(0, 256);// id, tamanio
  agregar_proceso(procesoNuevo);
  log_info(logger, "Proceso %d tamanio %d, tabla de primer nivel numero: %d", procesoNuevo->idProceso, procesoNuevo->tamanio, procesoNuevo->tablaPrimerNivel->nroTablaPrimerNivel);

  asignar_pagina_a_marco(procesoNuevo, 3);
  asignar_pagina_a_marco(procesoNuevo, 1);
  asignar_pagina_a_marco(procesoNuevo, 0);

  procesoNuevo = crear_proceso(1, 192);
  agregar_proceso(procesoNuevo);
  log_info(logger, "Proceso %d tamanio %d, tabla de primer nivel numero: %d", procesoNuevo->idProceso, procesoNuevo->tamanio, procesoNuevo->tablaPrimerNivel->nroTablaPrimerNivel);

  asignar_pagina_a_marco(procesoNuevo, 2);
}

int main(void)
{
  Config *config = config_create("Memoria.config");
  Logger *logger = iniciar_logger_memoria();

  rellenar_config_memoria(config);

  iniciar_estructuras_memoria();

  //Prueba
  correr_prueba(logger);

  // log_info(logger, "Iniciando Servidor Memoria...");
  // int socketMemoria = iniciar_servidor_memoria();

  // if (socketMemoria < 0)
  // {
  //   log_error(logger, "Error intentando iniciar Servidor Memoria.");
  //   return EXIT_FAILURE;
  // }

  // log_info(logger, "Servidor Memoria iniciado correctamente.");

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