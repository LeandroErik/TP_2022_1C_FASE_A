#include <socket/server.h>

int start_server(char *ip, char *port)
{
  int serverSocket;

  struct addrinfo hints, *servinfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(ip, port, &hints, &servinfo);

  serverSocket = socket(servinfo->ai_family,
                        servinfo->ai_socktype,
                        servinfo->ai_protocol);

  bind(serverSocket, servinfo->ai_addr, servinfo->ai_addrlen);

  int listening = listen(serverSocket, SOMAXCONN);
  freeaddrinfo(servinfo);

  if (listening < 0)
    return DISCONNECTION;
  else
    return serverSocket;
}

int wait_client(int serverSocket)
{

  return accept(serverSocket, NULL, NULL);
}

void shutdown_server(int serverSocket)
{
  close(serverSocket);
}