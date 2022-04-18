#include <socket/cliente.h>

int crear_conexion_con_servidor(char *ip, char *puerto)
{
  struct addrinfo hints;
  struct addrinfo *serverInfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(ip, puerto, &hints, &serverInfo);

  // Ahora vamos a crear el socket.
  int socketCliente = socket(serverInfo->ai_family,
                             serverInfo->ai_socktype,
                             serverInfo->ai_protocol);

  // Ahora que tenemos el socket, vamos a conectarlo
  connect(socketCliente, serverInfo->ai_addr, serverInfo->ai_addrlen);

  freeaddrinfo(serverInfo);

  return socketCliente;
}

void liberar_conexion_con_servidor(int socketCliente)
{
  close(socketCliente);
}

void enviar_mensaje(char *mensaje, int socketCliente)
{
  void *serializar_paquete(t_paquete * paquete, int bytes)
  {
    void *magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->codigoOperacion), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);

    return magic;
  }

  void eliminar_paquete(t_paquete * paquete)
  {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
  }

  t_paquete *paquete = malloc(sizeof(t_paquete));

  paquete->codigoOperacion = MENSAJE;
  paquete->buffer = malloc(sizeof(t_buffer));
  paquete->buffer->size = strlen(mensaje) + 1;
  paquete->buffer->stream = malloc(paquete->buffer->size);
  memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

  int bytes = paquete->buffer->size + 2 * sizeof(int);

  void *aEnviar = serializar_paquete(paquete, bytes);

  send(socketCliente, aEnviar, bytes, 0);

  free(aEnviar);
  eliminar_paquete(paquete);
}