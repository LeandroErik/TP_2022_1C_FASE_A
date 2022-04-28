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

typedef struct
{
	char *identificador;
	int parametros[2];
} t_linea_codigo;

// TODO: agregar briefs
t_linea_codigo *parser_archivo_codigo(char *rutaArchivo);

void eliminarSaltoDeLinea(char *);

char *leerLinea(FILE *);

int vecesQueAparece(char *, char);

int cantidadTokens(char *);

char **obtenerTokens(char *);

bool orderByRegion(void *, void *);

bool orderByAge(void *, void *);

bool orderByRegionAndAge(void *, void *);

t_log *initLogger();

void terminateProgram(FILE *, t_list *);

#endif
