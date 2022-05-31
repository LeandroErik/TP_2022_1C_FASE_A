#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include <socket/servidor.h>
#include <socket/cliente.h>
#include <commons/log.h>
#include <pthread.h>

t_log *logger;

/**
 * @brief Iniciar el servidor del módulo Memoria para conectar el Módulo de CPU o Kernel (como cliente).
 *
 * @param logger Logger de Memoria.
 *
 * @return Socket del servidor Memoria (int).
 */
int iniciar_servidor_memoria(t_log *logger);

/**
 * @brief Obtener el socket del Módulo de CPU.
 *
 * @param socketMemoria Socket del servidor Memoria (int).
 * @param logger Logger de Memoria.
 *
 * @return Socket de CPU (int).
 */
int obtener_socket_cpu(int socketMemoria, t_log *logger);

/**
 * @brief Obtener el modulo de Kernel.
 *
 * @param socketMemoria Socket del servidor Memoria (int).
 * @param logger Logger de Memoria.
 *
 * @return Socket de Kernel (int).
 */
int obtener_socket_kernel(int socketMemoria, t_log *logger);

/**
 * @brief Apagar el servidor de Memoria.
 *
 * @param socketMemoria Socket del servidor Memoria (int).
 * @param logger Logger de Memoria.
 */
void apagar_servidor_memoria(int socketMemoria, t_log *logger);

#endif
