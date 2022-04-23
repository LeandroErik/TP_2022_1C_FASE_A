#include <cpu_utils.h>
#include <commons/config.h>

void conectar_memoria(void);

int main(int argc, char *argv[])
{
  t_config* config = config_create("CPU.config");

  int entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
  char* reemplazo_tlb = config_get_string_value(config, "REEMPLAZO_TLB");
  int retardo_noOp = config_get_int_value(config, "RETARDO_NOOP");
  char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
  int puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
  int puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
  int puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");

  logger = log_create("CPU.log", "CPU", true, LOG_LEVEL_DEBUG);

  int socketCPU = iniciar_servidor_cpu(logger);
  int socketKernel = obtener_socket_kernel(socketCPU, logger);
  char *mensaje;

  conectar_memoria();

  while (true)
  {
    cod_op_servidor codOp = obtener_codigo_operacion(socketKernel);

    switch (codOp)
    {
    case MENSAJE_CLIENTE:
      mensaje = obtener_mensaje(socketKernel);
      log_info(logger, "Recibí el mensaje: %s", mensaje);
      break;

    case DESCONEXION_CLIENTE:
      apagar_servidor_cpu(socketCPU, logger);
      return EXIT_FAILURE;
    default:
      log_warning(logger, "Operación desconocida.");
      break;
    }
  }
  log_destroy(logger);
}

void conectar_memoria(void)
{
    int socketCpuCliente = crear_conexion_con_memoria();

    enviar_mensaje("soy CPU, pa", socketCpuCliente);

    liberar_conexion_con_memoria(socketCpuCliente);
} 