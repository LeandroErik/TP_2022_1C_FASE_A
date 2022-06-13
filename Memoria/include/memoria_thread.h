#ifndef MEMORIA_THREAD_H
#define MEMORIA_THREAD_H

#include <memoria_utils.h>

/**
 * @brief Maneja un paquete enviado por un cliente (Kernel | CPU).
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 */
void manejar_paquetes_clientes(int socketCliente);

/**
 * @brief Chequea si el primer mensaje enviado por el socket es "Kernel" o no.
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 * 
 * @return True si el primer mensaje enviado por el socket es "Kernel".
 */
bool es_kernel(int socketCliente);

/**
 * @brief Maneja un paquete enviado por un cliente (Kernel | CPU).
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 */
void escuchar_kernel(int socketCliente);

/**
 * @brief Maneja un paquete enviado por un cliente (Kernel | CPU).
 *
 * @param socketCliente Socket del cliente (Kernel | CPU).
 */
void escuchar_cpu(int socketCliente);

#endif