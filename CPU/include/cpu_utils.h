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

/**
 * @brief Devuelve el el tipo de ejecución (enum) de una instrucción dada.
 *
 * @param instruccion Instrucción.
 * @return Tipo de ejecución a realizar.
 */
TipoEjecucion obtener_tipo_ejecucion(char *instruccion);

/**
 * @brief No hace nada por un tiempo determinado dado por el config.
 *
 */
void ejecutar_noop();

/**
 * @brief Modifica el PCB y se lo devuelve a Kernel para bloquearse.
 *
 * @param pcb PCB.
 * @param tiempoBloqueadoIO Tiempo de bloqueo de I/O.
 * @param socketKernel Socket de Kernel.
 */
void ejecutar_io(Pcb *pcb, int tiempoBloqueadoIO, int socketKernel);

/**
 * @brief Modifica el PCB y se lo devuelve a Kernel para su finalización.
 *
 * @param pcb PCB.
 * @param socketKernel Socket de Kernel.
 */
void ejecutar_exit(Pcb *pcb, int socketKernel);

/**
 * @brief Lee las líneas de instrucciones dadas por el PCB en el orden que diga el contador de programa.
 *
 * @param pcb PCB.
 * @param socketKernel Socket de Kernel.
 */
void ejecutar_lista_instrucciones_del_pcb(Pcb *pcb, int socketKernel);

#endif