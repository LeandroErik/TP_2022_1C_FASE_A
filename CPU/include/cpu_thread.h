#ifndef CPU_THREAD_H
#define CPU_THREAD_H

#include <cpu_utils.h>

typedef struct EstructuraMemoria
{
  int SOCKET_MEMORIA;
  int ENTRADAS_POR_TABLA;
  int TAMANIO_PAGINA;
} EstructuraMemoria;

EstructuraMemoria ESTRUCTURA_MEMORIA;

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
 * @brief Realiza el handshake con Memoria para obtener la estructura de memoria.
 *
 * @param socketMemoria Socket de Memoria.
 */
void realizar_handshake_con_memoria(int socketMemoria);

/**
 * @brief Cargar la estructura con los datos de la memoria.
 *
 * @param socketMemoria Socket de Memoria.
 */
void cargar_estructura_memoria(int socketMemoria);

#endif