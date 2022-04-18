#ifndef CONSOLA_UTILS_H
#define CONSOLA_UTILS_H

#include <socket/cliente.h>
#include <commons/log.h>

/**
 * @brief Conectar la Consola con el servidor de Kernel.
 *
 * @return Socket de la Consola.
 */
int crear_conexion_con_kernel(void);

/**
 * @brief Liberar la conexión con el servidor de Kernel
 *
 * @param socketConsola Socket de la Consola (int).
 */
void liberar_conexion_con_kernel(int socketConsola);

#endif
