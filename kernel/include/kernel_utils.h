#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <socket/servidor.h>
#include <commons/log.h>

#define IP "127.0.0.1"
#define PUERTO "5000"

t_log *logger;

/**
 * @brief Iniciar el servidor del módulo Kernel para conectar el Módulo de Consola (como cliente).
 *
 * @return Socket del servidor Kernel (int).
 */
int iniciar_servidor_kernel(t_log *logger);

/**
 * @brief Obtener el socket del Módulo de Consola.
 *
 * @return Socket de Consola (int).
 */
int obtener_socket_consola(t_log *logger);

#endif /* KERNEL_UTILS_H_ */
