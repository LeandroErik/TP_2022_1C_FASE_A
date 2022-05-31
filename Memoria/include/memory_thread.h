#ifndef MEMORY_THREAD_H
#define MEMORY_THREAD_H

#include <memory_utils.h>

/**
 * @brief Manages a package sent by a a client (Kernel | CPU).
 *
 * @param clientSocket Socket of client (Kernel | CPU).
 */
void manage_clients_packages(int clientSocket);

/**
 * @brief Checks if the first message sent by socket is "Kernel" or not.
 *
 * @param clientSocket Socket of client (Kernel | CPU).
 * @return True | False.
 */
bool is_kernel(int clientSocket);

#endif