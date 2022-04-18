#include <socket/servidor.h>

int iniciar_servidor_kernel(t_log *logger)
{
	int socketKernel = iniciar_servidor("127.0.0.1", "5000");
	log_info(logger, "Módulo Kernel listo para recibir el Módulo Consola");
	return socketKernel;
}

int obtener_socket_consola(t_log *logger)
{
	int socketKernel = iniciar_servidor_kernel(logger);
	int socketCliente = esperar_cliente(socketKernel);
	log_info(logger, "Se conectó el Módulo de Consola...");

	return socketCliente;
}