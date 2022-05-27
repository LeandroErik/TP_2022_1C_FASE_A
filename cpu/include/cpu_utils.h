#ifndef CPU_UTILS_H
#define CPU_UTILS_H

#include <socket/client.h>
#include <socket/server.h>
#include <cpu_config.h>
#include <cpu_thread.h>

/**
 * @brief Initializes a logger in CPU Module.
 *
 * @return Logger object.
 */
Logger *init_cpu_logger(void);

/**
 * @brief Starts a Server CPU in dispatch port.
 *
 * @return Socket of client.
 */
int start_cpu_dispatch_server(void);

/**
 * @brief Starts a Server CPU in interrupt port.
 *
 * @return Socket of client.
 */
int start_cpu_interrupt_server(void);

/**
 * @brief Creates a connection with Server Memory.
 *
 * @return Socket of client.
 */
int connect_to_memory_server();

/**
 * @brief Persists instruction lines in logger.
 *
 * @param logger Logger.
 * @param instructionsList Instructions list.
 */
void show_instruction_lines(Logger *logger, List *instructionsList);

/**
 * @brief Persists PCB in logger.
 *
 * @param logger Logger.
 * @param pcb PCB.
 */
void show_PCB(Logger *logger, Pcb *pcb);

#endif