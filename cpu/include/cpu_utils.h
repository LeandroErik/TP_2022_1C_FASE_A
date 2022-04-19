#ifndef UTILS_H_
#define UTILS_H_

#include <socket/servidor.h>
#include <commons/log.h>

t_log *logger;

/**
 * @brief Iniciar el servidor del módulo CPU para conectar el Módulo de Kernel (como cliente).
 *
 * @param logger Logger de CPU.
 *
 * @return Socket del servidor CPU (int).
 */
int iniciar_servidor_cpu(t_log *logger);

/**
 * @brief Obtener el socket del Módulo de Kernel.
 *
 * @param socketCPU Socket del servidor CPU (int).
 * @param logger Logger de CPU.
 *
 * @return Socket de Kernel (int).
 */
int obtener_socket_kernel(int socketCPU, t_log *logger);

/**
 * @brief Apagar el servidor de CPU.
 *
 * @param socketKernel Socket del servidor CPU (int).
 * @param logger Logger de CPU.
 */
void apagar_servidor_cpu(int socketCPU, t_log *logger);

#endif
