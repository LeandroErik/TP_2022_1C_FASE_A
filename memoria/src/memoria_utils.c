#include <memoria_utils.h>


int iniciar_servidor_memoria(t_log *logger)
{
	int socketMemoria = iniciar_servidor("127.0.0.1", "5002");
	log_info(logger, "Módulo Memoria listo para recibir el Módulo CPU o Kernel");
	return socketMemoria;
}

int obtener_socket_cpu(int socketMemoria, t_log *logger)
{
	int socketCliente = esperar_cliente(socketMemoria);
	log_info(logger, "Se conectó el Módulo de CPU...");

	return socketCliente;
}

int obtener_socket_kernel(int socketMemoria, t_log *logger)
{
	int socketCliente = esperar_cliente(socketMemoria);
	log_info(logger, "Se conectó el Módulo de Kernel...");

	return socketCliente;
}

void apagar_servidor_memoria(int socketMemoria, t_log *logger)
{
	apagar_servidor(socketMemoria);
	log_error(logger, "El CLIENTE se desconectó. Apagando Servidor Memoria.");
}