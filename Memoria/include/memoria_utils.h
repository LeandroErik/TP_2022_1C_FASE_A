#ifndef MEMORIA_UTILS_H
#define MEMORIA_UTILS_H

#include <socket/servidor.h>
#include <memoria_config.h>
#include <memoria_thread.h>
#include <main.h>

t_list* procesos;

/**
 * @brief Inicia un logger en el Módulo Memoria.
 *
 * @return Logger.
 */
Logger *iniciar_logger_memoria();

/**
 * @brief Inicia el Servidor Memoria.
 *
 * @return Socket del cliente.
 */
int iniciar_servidor_memoria();

/**
 * @brief Crear tabla de primer nivel  
 * 
 * @return Tabla de primer nivel vacia.
 */
TablaPrimerNivel* crear_tabla_primer_nivel();

/**
 * @brief Crear una tabla de segundo nivel. 
 *
 * @return Tabla de segundo nivel vacia.
 */
TablaSegundoNivel* crear_tabla_segundo_nivel();

/**
 * @brief Crear un proceso y asignarle una tabla de primer nivel. 
 * @param id Id del prcoeso
 * @param tamanio Tamanio del proceso.
 *
 * @return Proceso nuevo.
 */
Proceso* crear_proceso(int id, int tamanio);

/**
 * @brief Agrega un proceso a la lista de procesos. 
 * @param proceso Proceso a agregar a la lista de procesos
 *
 * @return Index donde se agrega el proceso.
 */
int agregar_proceso(Proceso* proceso);

#endif