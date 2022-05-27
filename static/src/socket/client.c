#include <socket/client.h>

int create_server_connection(char *ip, char *port)
{
  struct addrinfo hints;
  struct addrinfo *serverInfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(ip, port, &hints, &serverInfo);

  int clientSocket = socket(serverInfo->ai_family,
                            serverInfo->ai_socktype,
                            serverInfo->ai_protocol);

  int connection = connect(clientSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
  freeaddrinfo(serverInfo);

  if (connection < 0)
    return DISCONNECTION;
  else
    return clientSocket;
}

void release_server_connection(int clientSocket)
{
  close(clientSocket);
}

void terminate_program(int socket, Config *config, Logger *logger)
{
  release_server_connection(socket);
  config_destroy(config);
  log_destroy(logger);
}