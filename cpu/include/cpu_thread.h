#ifndef CPU_THREAD_H
#define CPU_THREAD_H

#include <cpu_utils.h>

/**
 * @brief Waits Kernel to enter the server and receives PCBs.
 *
 * @param cpuSocket Socket of CPU in Dispatch port.
 */
void wait_kernel_dispatch(int cpuSocket);

/**
 * @brief Waits Kernel to enter the server and receives Interrupts.
 *
 * @param cpuSocket Socket of CPU in Interrupt port.
 */
void wait_kernel_interrupt(int cpuSocket);

/**
 * @brief Manages a PCB sent by Kernel.
 *
 * @param kernelSocket Socket of Kernel connected to Dispatch port.
 */
void manage_kernel_dispatch_package(int kernelSocket);

/**
 * @brief Manages an Interrupt sent by Kernel.
 *
 * @param kernelSocket Socket of Kernel connected to Interrupt port.
 */
void manage_kernel_interrupt_package(int kernelSocket);

/**
 * @brief Manages a connection to Memory.
 *
 */
void manage_connection_memory(void);

#endif