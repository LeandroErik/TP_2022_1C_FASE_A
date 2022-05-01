#ifndef UTILS_H_
#define UTILS_H_

#include <socket/servidor.h>
#include <socket/cliente.h>
#include <cpu_config.h>
#include <commons/log.h>
#include <socket/protocolo.h>
#include <commons/collections/list.h>

t_log *logger;

/**
 * @brief Iniciar el servidor del módulo CPU para conectar el Módulo de Kernel (como cliente).
 *
 * @param logger Logger de CPU.
 * @param puerto Puerto de CPU (Dispatch o Interrupt)
 *
 * @return Socket del servidor CPU (int).
 */
int iniciar_servidor_cpu(t_log *logger, char *puerto);

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

/**
 * @brief Crear conexion con el modulo Memoria.
 *
 * @return Socket de Memoria.
 */
int crear_conexion_con_memoria(void);

/**
 * @brief Liberar la conexion con el modulo Memoria.
 *
 * @param socketCpu Socket del CPU.
 */
void liberar_conexion_con_memoria(int socketCpu);

#endif
