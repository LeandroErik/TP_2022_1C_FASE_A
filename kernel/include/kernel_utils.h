#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#include <socket/server.h>
#include <socket/client.h>
#include <kernel_config.h>
#include <kernel_thread.h>
#include <commons/string.h>

int globalProgramID;

/**
 * @brief Initializes a logger in Kernel Module.
 *
 * @return Logger object.
 */
Logger *init_kernel_logger(void);

/**
 * @brief Starts a Server Kernel.
 *
 * @return Socket of client.
 */
int start_kernel_server(void);

/**
 * @brief Fills the instruction lines of a package and returns its process size.
 *
 * @param instructionList Instruction lines.
 * @param clientSocket Socket of client.
 * @return Process size.
 */
int fill_instruction_lines(List *instructionList, int clientSocket);

/**
 * @brief Creates a PCB object.
 *
 * @param instructionsList Instructions list.
 * @param processSize Process size.
 * @return PCB.
 */
Pcb *create_PCB(List *instructionsList, int processSize);

/**
 * @brief Generates a PCB using the instructions list passed by client.
 *
 * @param clientSocket Socket of client.
 */
Pcb *generate_PCB(int clientSocket);

/**
 * @brief Creates a connection with Server CPU using Dispatch Port.
 *
 * @return Socket of client.
 */
int connect_to_cpu_dispatch_server(void);

/**
 * @brief Creates a connection with Server CPU using Interrupt Port.
 *
 * @return Socket of client.
 */
int connect_to_cpu_interrupt_server(void);

/**
 * @brief Creates a connection with Server Memory.
 *
 * @return Socket of client.
 */
int connect_to_memory_server(void);

#endif