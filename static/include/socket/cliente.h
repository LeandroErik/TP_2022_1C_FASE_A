#ifndef CLIENTE_H
#define CLIENTE_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <readline/readline.h>

typedef enum
{
  MENSAJE
} cod_op;

typedef struct
{
  int size;
  void *stream;
} t_buffer;

typedef struct
{
  cod_op codigoOperacion;
  t_buffer *buffer;
} t_paquete;

/**
 * @brief
 *
 * @param ip IP del servidor a conectarse.
 * @param puerto Puerto del servidor a conectarse.
 * @return Socket del cliente (int).
 */
int crear_conexion_con_servidor(char *ip, char *puerto);

/**
 * @brief Liberar la conexión con el servidor.
 *
 * @param socketCliente Socket del cliente (int).
 */
void liberar_conexion_con_servidor(int socketCliente);

/**
 * @brief Enviar un mensaje al servidor conectado.
 *
 * @param mensaje Mensaje a enviar.
 * @param socketCliente Socket del cliente (int).
 */
void enviar_mensaje(char *mensaje, int socketCliente);

#endif