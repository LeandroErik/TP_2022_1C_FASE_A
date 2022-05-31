#ifndef CPU_THREAD_H
#define CPU_THREAD_H

#include <cpu_utils.h>

/**
 * @brief Espera a Kernel a que se conecte al servidor y recibe sus PCBs.
 *
 * @param socketCpu Socket de CPU en el puerto de Dispatch.
 */
void esperar_kernel_dispatch(int socketCpu);

/**
 * @brief Espera a Kernel a que se conecte al servidor y recibe sus Interrupciones.
 *
 * @param socketCpu Socket de CPU en el puerto de Interrupt.
 */
void esperar_kernel_interrupt(int socketCpu);

/**
 * @brief Maneja un PCB enviado por Kernel.
 *
 * @param socketKernel Socket de Kernel conectado al puerto de Dispatch.
 */
void manejar_paquete_kernel_dispatch(int socketKernel);

/**
 * @brief Maneja una interrupción enviada por Kernel.
 *
 * @param socketKernel Socket de Kernel conectado al puerto de Interrupt.
 */
void manejar_paquete_kernel_interrupt(int socketKernel);

/**
 * @brief Maneja una conexión con Memoria.
 *
 */
void manejar_conexion_memoria();

#endif