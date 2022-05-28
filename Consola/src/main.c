#include <consola_utils.h>
#include <commons/log.h>

int main(int argc, char *argv[])
{
  Config *config = config_create("Consola.config");
  Logger *logger = log_create("Consola.log", "Consola", true, LOG_LEVEL_INFO);

  // if(argc < 3)
  // {
  //   log_error(logger, "Los argumentos son dos.");
  //   return EXIT_FAILURE;
  // }

  rellenar_configuracion_consola(config);

  // char* direccionArchivo = argv[1];
  //  int tamanioProceso = atoi(argv[2]);

  char *direccionArchivo = "/pseudocodigo.txt";
  int tamanioProceso = 100;

  log_info(logger, "Conectando con Servidor Kernel...");
  int socketKernel = conectar_con_kernel();

  if (socketKernel < 0)
  {
    log_error(logger, "Conexión rechazada. El servidor no está disponible.");
    return EXIT_FAILURE;
  }

  log_info(logger, "Conexión aceptada. Iniciando cliente...");

  FILE *pseudocodigo = fopen(direccionArchivo, "r");
  Lista *listaInstrucciones = list_create();
  rellenar_lista_de_instrucciones(listaInstrucciones, pseudocodigo);

  Paquete *paquete = crear_paquete(LINEAS_CODIGO);
  serializar_lista_de_instrucciones(paquete, listaInstrucciones, tamanioProceso);
  enviar_paquete_a_servidor(paquete, socketKernel);
  log_info(logger, "Enviando lista de instrucciones...");

  log_warning(logger, "Saliendo del servidor...");
  terminar_consola(socketKernel, config, logger, listaInstrucciones, pseudocodigo, paquete);

  return EXIT_SUCCESS;
}