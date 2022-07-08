#ifndef MEMORIA_THREAD_H
#define MEMORIA_THREAD_H

#include <memoria_utils.h>

/**
 * @brief Maneja los paquetes enviados por un cliente (Kernel | CPU).
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 */
void manejar_paquetes_clientes(int socketCliente);

/**
 * @brief Chequea si el primer mensaje enviado por el socket es "Kernel" o no, para discriminar la conexion.
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 *
 * @return True si el primer mensaje enviado por el socket es "Kernel".
 */
bool es_kernel(int socketCliente);

/**
 * @brief Escucha las peticiones de Kernel.
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 */
void escuchar_kernel(int socketCliente);

/**
 * @brief Escucha las peticiones de CPU.
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 */
void escuchar_cpu(int socketCliente);

/**
 * @brief Crea el proceso, sus estructuras y notifica a Kernel el numero de tablas de primer nivel.
 *
 * @param socketKernel
 * @param logger
 */
void atender_creacion_de_proceso(int socketKernel, Logger *logger);

/**
 * @brief Suspende el proceso, lo manda a SWAP y notifica a Kernel.
 *
 * @param socketKernel
 * @param logger
 */
void atender_suspension_de_proceso(int socketKernel, Logger *logger);

/**
 * @brief Finaliza un proceso, libera sus estructuras y notifica a Kernel.
 *
 * @param socketKernel
 * @param logger
 */
void atender_finalizacion_de_proceso(int socketKernel, Logger *logger);

/**
 * @brief Espera el tiempo dado por config antes de responder al cliente
 *
 */
void realizar_espera_de_memoria();

/**
 * @brief Envia a CPU las estructuras de la memoria, dadas por config.
 *
 * @param socketCPU
 * @param logger
 */
void enviar_estructuras_de_memoria_a_cpu(int socketCPU, Logger *logger);

/**
 * @brief Envia a CPU el numero de tabla de segundo nivel, dado por un numero de tabla de primer nivel y su entrada.
 *
 * @param socketCPU
 * @param logger
 */
void atender_pedido_de_tabla_de_segundo_nivel(int socketCPU, Logger *logger);

/**
 * @brief Envia a CPU el numero de marco al que asignada una entrada de una tabla de segundo nivel (y la asigna si no lo esta).
 *
 * @param socketCPU
 * @param logger
 */
void atender_pedido_de_marco(int socketCPU, Logger *logger);

/**
 * @brief Escribe en memoria un entero en la direccion fisica recibida.
 *
 * @param socketCPU
 * @param logger
 */
void atender_escritura_en_memoria(int socketCPU, Logger *logger);

/**
 * @brief Lee de memoria un entero en la direccion fisica recibida.
 *
 * @param socketCPU
 * @param logger
 */
void atender_lectura_de_memoria(int socketCPU, Logger *logger);

/**
 * @brief Copia un entero de una direccion origen a una destino.
 *
 * @param socketCPU
 * @param logger
 */
void atender_copiado_en_memoria(int socketCPU, Logger *logger);

#endif