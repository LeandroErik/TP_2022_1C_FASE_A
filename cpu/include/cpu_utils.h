#ifndef CPU_UTILS_H
#define CPU_UTILS_H

#include <socket/cliente.h>
#include <socket/servidor.h>
#include <cpu_config.h>
#include <cpu_thread.h>

/**
 * @brief Iniciar un logger en el Módulo CPU.
 *
 * @return Logger.
 */
Logger *iniciar_logger_cpu();

/**
 * @brief Inicia un Servidor CPU en el puerto de Dispatch.
 *
 * @return Socket de Kernel en el puerto de Dispatch.
 */
int iniciar_servidor_cpu_dispatch();

/**
 * @brief Inicia un Servidor CPU en el puerto de Interrupt.
 *
 * @return Socket de Kernel en el puerto de Interrupt.
 */
int iniciar_servidor_cpu_interrupt();

/**
 * @brief Conecta con Memoria.
 *
 * @return Socket de Memoria.
 */
int conectar_con_memoria();

/**
 * @brief Persiste las líneas de instrucciones en el logger.
 *
 * @param logger Logger.
 * @param listaInstrucciones Lista de instrucciones.
 */
void mostrar_lineas_instrucciones(Logger *logger, Lista *listaInstrucciones);

/**
 * @brief Persiste el PCB en el logger.
 *
 * @param logger Logger.
 * @param pcb PCB.
 */
void mostrar_pcb(Logger *logger, Pcb *pcb);

#endif