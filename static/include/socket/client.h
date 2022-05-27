#ifndef CLIENT_H
#define CLIENT_H

#include <socket/socket.h>

/**
 * @brief Creates a connection with a specific server.
 *
 * @param ip Server ip to be connected.
 * @param port Server port to be connected.
 * @return Socket of client.
 */
int create_server_connection(char *ip, char *port);

/**
 * @brief Releases server connection.
 *
 * @param clientSocket Socket of client.
 */
void release_server_connection(int clientSocket);

/**
 * @brief Terminates a program "destroying" params (freeing space).
 *
 * @param socket Socket connected to server.
 * @param config Configuration object.
 * @param logger Logger.
 */
void terminate_program(int socket, Config *config, Logger *logger);

#endif