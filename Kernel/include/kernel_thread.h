#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include <kernel_utils.h>

/**
 * @brief Espera a que una consola se conecte al servidor y recibe sus instrucciones.
 *
 * @param socketKernel Socket de Kernel.
 */
void esperar_consola(int socketKernel);

/**
 * @brief Maneja un paquete enviado por consola.
 *
 * @param socketConsola Socket de Consola.
 */
void manejar_paquete_consola(int socketConsola);

/**
 * @brief Maneja una conexión con CPU usando el puerto de Interrupción.
 *
 */
void manejar_conexion_cpu_interrupcion();

/**
 * @brief Maneja una conexión con Memoria.
 *
 */
void manejar_conexion_memoria();

#endif