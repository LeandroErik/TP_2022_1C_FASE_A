#include "consola_utils.h"

int crear_conexion_con_kernel(void)
{
	return crear_conexion_con_servidor("127.0.0.1", "5000");
}

void liberar_conexion_con_kernel(int socketConsola)
{
	liberar_conexion_con_servidor(socketConsola);
}