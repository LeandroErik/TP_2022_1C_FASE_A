#ifndef CPU_UTILS_H
#define CPU_UTILS_H

#include <socket/cliente.h>
#include <socket/servidor.h>
#include <cpu_config.h>
#include <cpu_thread.h>
#include <math.h>

bool seNecesitaAtenderInterrupcion;

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
 * @brief Devuelve el el tipo de instrucción (enum) de una instrucción dada.
 *
 * @param instruccion Instrucción.
 * @return Tipo de ejecución a realizar.
 */
Instruccion obtener_tipo_instruccion(char *instruccion);

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
 * @brief Lee el valor de una dirección de memoria.
 *
 * @param direccionFisica Dirección de memoria.
 */
void ejecutar_read(int direccionFisica);

/**
 * @brief Escribe un valor en una dirección de memoria.
 *
 * @param proceso Proceso.
 * @param direccionFisica Dirección de memoria.
 * @param valor Valor a escribir.
 */
void ejecutar_write(Pcb *proceso, int direccionFisica, int valor);

/**
 * @brief Copia el valor de una dirección de memoria a otra.
 *
 * @param proceso Proceso.
 * @param direccionDestino Dirección de memoria destino.
 * @param direccionOrigen Dirección de memoria origen.
 */
void ejecutar_copy(Pcb *proceso, int direccionDestino, int direccionOrigen);

/**
 * @brief Lee las líneas de instrucciones dadas por el PCB en el orden que diga el contador de programa.
 *
 * @param pcb PCB.
 * @param socketKernel Socket de Kernel.
 */
void ejecutar_lista_instrucciones_del_pcb(Pcb *pcb, int socketKernel);

/**
 * @brief "Atiende" una interrupción, modificando el PCB y se lo devuelve a Kernel.
 *
 * @param pcb PCB.
 * @param socketKernel Socket de Kernel.
 */
void atender_interrupcion(Pcb *pcb, int socketKernel);

/**
 * @brief "Llama" al MMU para solicitar una dirección de memoria.
 *
 * @param proceso Proceso.
 * @param direccionLogica Dirección lógica.
 * @return Dirección física.
 */
int llamar_mmu(Pcb *proceso, int direccionLogica);

/**
 * @brief Pide el número de tabla de segundo nivel a Memoria.
 *
 * @param numeroTablaPrimerNivel Número de tabla de primer nivel.
 * @param entradaTablaPrimerNivel Entrada de tabla de primer nivel.
 * @return Número de tabla de segundo nivel.
 */
int pedir_tabla_segundo_nivel(int numeroTablaPrimerNivel, int entradaTablaPrimerNivel);

/**
 * @brief Pide el marco a Memoria.
 *
 * @param numeroTablaSegundoNivel Número de tabla de segundo nivel.
 * @param entradaTablaSegundoNivel Entrada de tabla de segundo nivel.
 * @return Número de marco.
 */
int pedir_marco(int numeroTablaSegundoNivel, int entradaTablaSegundoNivel);

#endif