#ifndef CONSOLA_UTILS_H
#define CONSOLA_UTILS_H

#include <socket/cliente.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <socket/protocolo.h>

/**
 * @brief Conectar la Consola con el servidor de Kernel.
 *
 * @return Socket de la Consola.
 */
int crear_conexion_con_kernel(void);

/**
 * @brief Liberar la conexión con el servidor de Kernel.
 *
 * @param socketConsola Socket de la Consola (int).
 */
void liberar_conexion_con_kernel(int socketConsola);

// TODO: agregar briefs
/**
 * @brief Leer las lineas de código del archivo de código.
 *
 * @param archivoCodigo Archivo de código.
 * @param lineasCodigo Lineas de código.
 * @param listaLineasCodigo Lista de lineas de código.
 */
void leer_lineas_codigo(FILE *archivoCodigo, t_linea_codigo *lineasCodigo, t_list *listaLineasCodigo);

void eliminar_salto_de_linea(char *);

char *leer_linea(FILE *);

int apariciones(char *string, char caracter);

/**
 * @brief Obtener la cantidad de tokens de una cadena.
 *
 * @return int - Cantidad de tokens.
 */
int cantidad_de_tokens(char *);

char **obtener_tokens(char *);

t_log *initLogger();

void terminar_parseo(FILE *, t_list *);

#endif
