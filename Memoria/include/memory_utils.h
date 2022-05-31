#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include <socket/server.h>
#include <memory_config.h>
#include <memory_thread.h>

int kernelSocket;
int cpuSocket;

/**
 * @brief Initializes a logger in Memory Module.
 *
 * @return Logger object.
 */
Logger *init_memory_logger(void);

/**
 * @brief Starts a Memory Kernel.
 *
 * @return Socket of client.
 */
int start_memory_server(void);

#endif