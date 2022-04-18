#include <socket/servidor.h>

int iniciar_servidor(char *ip, char *puerto)
{
  int socketServidor;

  struct addrinfo hints, *servinfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(ip, puerto, &hints, &servinfo);

  socketServidor = socket(servinfo->ai_family,
                          servinfo->ai_socktype,
                          servinfo->ai_protocol);

  bind(socketServidor, servinfo->ai_addr, servinfo->ai_addrlen);

  listen(socketServidor, SOMAXCONN);

  freeaddrinfo(servinfo);

  return socketServidor;
  return 0;
}

int esperar_cliente(int socketServidor)
{
  return accept(socketServidor, NULL, NULL);
}

cod_op obtener_codigo_operacion(int socketCliente)
{
  cod_op codigoOperacion;
  if (recv(socketCliente, &codigoOperacion, sizeof(int), MSG_WAITALL) > 0)
    return codigoOperacion;
  else
  {
    close(socketCliente);
    return DESCONEXION;
  }
}

char *obtener_mensaje(int socketCliente)
{

  void *recibir_buffer(int *size, int sC)
  {
    void *buffer;
    recv(sC, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(sC, buffer, *size, MSG_WAITALL);

    return buffer;
  }

  int size;
  char *buffer = recibir_buffer(&size, socketCliente);

  return buffer;
}

void apagar_servidor(int socketServidor)
{
  close(socketServidor);
}