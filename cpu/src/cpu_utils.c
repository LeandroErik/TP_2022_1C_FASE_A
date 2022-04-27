#include <cpu_utils.h>

int iniciar_servidor_cpu(t_log *logger, char *puerto)
{
	int socketCPU = iniciar_servidor(CPU_CONFIG.IP_CPU, puerto);
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

int crear_conexion_con_memoria(void)
{
	return crear_conexion_con_servidor(CPU_CONFIG.IP_MEMORIA, CPU_CONFIG.PUERTO_MEMORIA);
}

void liberar_conexion_con_memoria(int socketCpu)
{
	liberar_conexion_con_servidor(socketCpu);
}