#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <socket/servidor.h>
#include <socket/cliente.h>
#include <commons/log.h>

t_log *logger;

/**
 * @brief Iniciar el servidor del módulo Kernel para conectar el Módulo de Consola (como cliente).
 *
 * @param logger Logger de Kernel.
 *
 * @return Socket del servidor Kernel (int).
 */
int iniciar_servidor_kernel(t_log *logger);

/**
 * @brief Obtener el socket del Módulo de Consola.
 *
 * @param socketKernel Socket del servidor Kernel (int).
 * @param logger Logger de Kernel.
 *
 * @return Socket de Consola (int).
 */
int obtener_socket_consola(int socketKernel, t_log *logger);

/**
 * @brief Apagar el servidor de Kernel.
 *
 * @param socketKernel Socket del servidor Kernel (int).
 * @param logger Logger de Kernel.
 */
void apagar_servidor_kernel(int socketKernel, t_log *logger);

/**
 * @brief Conectar Kernel con el servidor de CPU.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_con_cpu(void);

/**
 * @brief Liberar la conexión con el servidor del CPU.
 *
 * @param socketKernel Socket del Kernel (int).
 */
void liberar_conexion_con_cpu(int socketKernel);

#endif
