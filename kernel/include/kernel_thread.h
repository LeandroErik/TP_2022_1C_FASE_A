#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include <kernel_utils.h>

/**
 * @brief Waits a console to enter the server and receives its instruction lines.
 *
 * @param kernelSocket Socket of Kernel.
 */
void wait_console(int kernelSocket);

/**
 * @brief Manages a package sent by a console.
 *
 * @param consoleSocket Socket of Console.
 */
void manage_console_package(int consoleSocket);

/**
 * @brief Manages a PCB received and send it to CPU via Dispatch.
 *
 * @param logger Logger.
 * @param consoleSocket Socket of Console.
 */
void manage_PCB(Logger *logger, int consoleSocket);

/**
 * @brief Manages a connection to CPU using Interrupt port.
 *
 */
void manage_connection_interrupt(void);

/**
 * @brief Manages a connection to Memory.
 *
 */
void manage_connection_memory(void);

#endif