#ifndef SERVER_H
#define SERVER_H

#include <socket/socket.h>

/**
 * @brief Starts a server.
 *
 * @param ip Server ip.
 * @param port Server port.
 *
 * @return Socket of server.
 */
int start_server(char *ip, char *port);

/**
 * @brief Awaits a client to connect into the server.
 *
 * @param serverSocket Socket of server.
 * @return Socket of client.
 */
int wait_client(int serverSocket);

/**
 * @brief Shuts down a server.
 *
 * @param serverSocket Socket of server.
 */
void shutdown_server(int serverSocket);

#endif