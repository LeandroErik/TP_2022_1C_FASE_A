#ifndef MEMORIA_UTILS_H
#define MEMORIA_UTILS_H

#include <socket/servidor.h>
#include <memoria_config.h>
#include <memoria_thread.h>
#include <main.h>
#include <stdbool.h>
#include <math.h>

//Variables globales
t_list *procesos;
t_list *marcos;
void *memoriaPrincipal;
int tablasDePrimerNivel;

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

void iniciar_estructuras_memoria();

/**
 * @brief Inicia los marcos de la memoria
 * @param cantidadMarcos Numero de marcos que tendra la memoria
 */
void iniciar_marcos(int cantidadMarcos);

/**
 * @brief Crear tabla de primer nivel
 *
 * @return Tabla de primer nivel vacia.
 */
TablaPrimerNivel *crear_tabla_primer_nivel();

/**
 * @brief Crear una tabla de segundo nivel.
 *
 * @return Tabla de segundo nivel vacia.
 */
TablaSegundoNivel *crear_tabla_segundo_nivel();

/**
 * @brief Crear un proceso y asignarle una tabla de primer nivel.
 * @param id Id del prcoeso
 * @param tamanio Tamanio del proceso.
 *
 * @return Proceso nuevo.
 */
Proceso *crear_proceso(int id, int tamanio);

/**
 * @brief Agrega un proceso a la lista de procesos.
 * @param proceso Proceso a agregar a la lista de procesos
 *
 * @return Index donde se agrega el proceso.
 */
void agregar_proceso(Proceso *proceso);

/**
 * @brief Escribir en Memoria Principal.
 * @param valorAEscribir El valor que se va a escribir en Memoria Principal.
 * @param desplazamiento Bytes que se va a desplazar en Memoria Principal.
 */
char* escribir_memoria(int idProceso, uint32_t valorAEscribir, int desplazamiento);

/**
 * @brief Leer lo que hay en Memoria Principal.
 * @param desplazamiento Bytes que se va a desplazar en Memoria Principal.
 *
 * @return Valor numerico de lo que hay en la direccion en memoria.
 */
uint32_t leer_de_memoria(int idProceso, int desplazamiento);

/**
 * @brief Busca el proceso en la lista de procesos.
 * @param idProceso Id del proceso a buscar.
 *
 * @return Index del proceso buscado.
 */
Proceso *buscar_proceso_por_id(int idProceso);

/**
 * @brief Obtener el numero de Marco buscado.
 * @param desplazamiento
 *
 * @return Marco buscado.
 */
int obtener_numero_de_marco(int desplazamiento);

/**
 * @brief A
 * @param proceso
 * @param nroMarco
 *
 * @return
 */
void asignar_marco(Proceso *proceso, int nroMarco);

/**
 * @brief Le asigna una pagina al primer marco libre marco o realiza sustitucion.
 * @param proceso
 * @param nroPagina
 *
 * @return
 */
Marco* asignar_pagina_a_marco(Proceso* proceso, int nroPagina);

Marco* primer_marco_libre();

bool tiene_marcos_por_asignar(Proceso* proceso);

int numero_de_marco(Marco* marco);

#endif