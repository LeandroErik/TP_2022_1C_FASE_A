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

  //Estructuras de memoria
  void* memoriaPrincipal = (void*) malloc(MEMORIA_CONFIG.TAM_MEMORIA);
  int cantidadMarcos = MEMORIA_CONFIG.TAM_MEMORIA / MEMORIA_CONFIG.TAM_PAGINA;

  Marco marcos[cantidadMarcos]; 
  procesos = list_create();

  //PRUEBA:
  int id=1;
  int tamanio=10;
  Proceso* procesoNuevo = crear_proceso(id,tamanio);
  int nroDeEntradaTablaPrimerNivel = agregar_proceso(procesoNuevo);
  log_info(logger, "Tabla de primer nivel del proceso %d, tabla numero: %d", id, nroDeEntradaTablaPrimerNivel);

   id=2;
   tamanio=15;
  Proceso* procesoNuevo2 = crear_proceso(id,tamanio);
  nroDeEntradaTablaPrimerNivel = agregar_proceso(procesoNuevo);
  log_info(logger, "Tabla de primer nivel del proceso %d, tabla numero: %d", id, nroDeEntradaTablaPrimerNivel);

   id=3;
   tamanio=30;
  Proceso* procesoNuevo3 = crear_proceso(id,tamanio);
  nroDeEntradaTablaPrimerNivel = agregar_proceso(procesoNuevo);
  log_info(logger, "Tabla de primer nivel del proceso %d, tabla numero: %d", id, nroDeEntradaTablaPrimerNivel);
  //
  //



  //

  //Hilos
  // Hilo hiloCliente;

  // for (int i=0; i<2; i++)
  // {
  //   int socketCliente = esperar_cliente(socketMemoria);

  //   pthread_create(&hiloCliente, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);
  //   pthread_join(hiloCliente, NULL);
  // }

  return EXIT_SUCCESS;
}