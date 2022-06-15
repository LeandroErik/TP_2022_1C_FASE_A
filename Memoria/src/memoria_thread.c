#include <memoria_thread.h>
#include <memoria_utils.h>

void manejar_paquetes_clientes(int socketCliente)
{
  Logger *logger = iniciar_logger_memoria();
  bool esKernel;

  switch (obtener_codigo_operacion(socketCliente))
  {
  case DESCONEXION:
    log_warning(logger, "Se desconecto un cliente.");
    return;

  case MENSAJE:
    esKernel = es_kernel(socketCliente);
    if (esKernel){
      log_info(logger, "Se recibió un mensaje de Kernel.");
      escuchar_kernel(socketCliente);
    }
    else{
      log_info(logger, "Se recibió un mensaje de CPU.");
      escuchar_cpu(socketCliente);
    }
    break;

  default:
    break;
  }

  log_destroy(logger);
}

bool es_kernel(int socketCliente)
{
  char *mensaje = obtener_mensaje_del_cliente(socketCliente);
  bool esKernel = strcmp(mensaje, "Kernel") == 0;
  free(mensaje);

  return esKernel;
}

void escuchar_kernel(int socketCliente){

  while(1)
  {
    CodigoOperacion codOp = obtener_codigo_operacion(socketCliente);
    //Logger *logger = iniciar_logger_memoria();

    switch (codOp){
      case PROCESO_NUEVO:
        
        // int id=1;
        // int tamanio=10;
        // Proceso* procesoNuevo = crear_proceso(id,tamanio);
        // int nroDeEntradaTablaPrimerNivel = agregar_proceso(procesoNuevo);
        // log_info(logger, "tabla 1 nivel del proceso %d, tabla numero: %d", id, tamanio);

      break;
      case SUSPENDER_PROCESO:
      //
      break;
      default:
      ///
      break;
    }
  }

}

void escuchar_cpu(int socketCliente){

  while(1)
  {

  }
  
}