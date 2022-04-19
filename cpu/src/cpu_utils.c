#include <cpu_utils.h>

int iniciar_servidor_cpu(t_log *logger)
{
	int socketCPU = iniciar_servidor("127.0.0.1", "5001");
	log_info(logger, "Módulo CPU listo para recibir el Módulo Kernel");
	return socketCPU;
}

int obtener_socket_kernel(int socketCPU, t_log *logger)
{
	int socketCliente = esperar_cliente(socketCPU);
	log_info(logger, "Se conectó el Módulo de Kernel...");

	return socketCliente;
}

void apagar_servidor_cpu(int socketCPU, t_log *logger)
{
	apagar_servidor(socketCPU);
	log_error(logger, "Kernel se desconectó. Apagando Servidor CPU.");
}