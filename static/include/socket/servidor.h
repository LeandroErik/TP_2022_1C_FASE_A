#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>

typedef enum
{
  DESCONEXION_CLIENTE = -1,
  MENSAJE_CLIENTE
} cod_op_servidor;

/**
 * @brief Iniciar un servidor para que clientes se conecten.
 *
 * @param ip IP del servidor.
 * @param puerto Puerto del servidor.
 *
 * @return Socket del servidor (int).
 */
int iniciar_servidor(char *ip, char *puerto);

/**
 * @brief Esperar a un cliente para que se conecte al servidor.
 *
 * @param socketServidor Socket del servidor (int).
 * @return Socket del cliente (int).
 */
int esperar_cliente(int socketServidor);

/**
 * @brief Obtener el código de operación del paquete recibido.
 *
 * @param socketCliente Socket del cliente (int).
 * @return Código de operación.
 */
cod_op_servidor obtener_codigo_operacion(int socketCliente);

/**
 * @brief Obtener el mensaje dentro del paquete recibido.
 *
 * @param socketCliente Socket del cliente (int).
 * @return Mensaje.
 */
char *obtener_mensaje(int socketCliente);

/**
 * @brief Apagar el servidor
 *
 * @param socketServidor Socket del servidor (int).
 */
void apagar_servidor(int socketServidor);

#endif