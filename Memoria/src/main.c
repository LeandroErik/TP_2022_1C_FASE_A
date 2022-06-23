#include <memoria_utils.h>
#include <main.h>

void correr_prueba()
{
  Proceso *procesoNuevo1 = crear_proceso(0, 256); // id, tamanio

  asignar_pagina_a_marco_libre(procesoNuevo1, 3);
  asignar_pagina_a_marco_libre(procesoNuevo1, 1);
  asignar_pagina_a_marco_libre(procesoNuevo1, 0);

  Proceso* procesoNuevo2 = crear_proceso(1, 192);

  asignar_pagina_a_marco_libre(procesoNuevo2, 2);

  finalizar_proceso(procesoNuevo1->idProceso);
  finalizar_proceso(procesoNuevo2->idProceso);

  Proceso* procesoNuevo3 = crear_proceso(2, 192);  
  asignar_pagina_a_marco_libre(procesoNuevo3, 0);
  asignar_pagina_a_marco_libre(procesoNuevo3, 1);

  //hardcode como si el proceso escribiera en memoria, no esta implementado aun
  escribir_entero_en_memoria(12, 100);
  TablaSegundoNivel* ts = list_get(procesoNuevo3->tablaPrimerNivel->entradas, 0);
  Pagina* pagina = list_get(ts->entradas, 1);
  pagina->modificado = true;

  suspender_proceso(procesoNuevo3->idProceso);
  asignar_pagina_a_marco_libre(procesoNuevo3, 1);

  finalizar_proceso(procesoNuevo3->idProceso);

  //Escritura, lectura y copia en memoria
  // escribir_entero_en_memoria(12, 100);
  // leer_entero_de_memoria(100);
  // copiar_entero_en_memoria(120,100);
  // leer_entero_de_memoria(120);
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

  correr_prueba();

  // //Hilos
  // while(true)
  // {
  //   int socketCliente = esperar_cliente(socketMemoria);
  //   Hilo hiloCliente;

  //   pthread_create(&hiloCliente, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);
  //   //pthread_join(hiloCliente, NULL);
  // }

  liberar_memoria();
  log_destroy(logger);
  config_destroy(config);

  return EXIT_SUCCESS;
}