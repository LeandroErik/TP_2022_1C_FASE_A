#ifndef MEMORIA_UTILS_H
#define MEMORIA_UTILS_H

#include <socket/servidor.h>
#include <memoria_config.h>
#include <memoria_thread.h>
#include <main.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <commons/string.h>
#include <unistd.h>

// Variables globales
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

/**
 * @brief Inicia las estructuras de la memoria
 *
 */
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
 * @param nroPrimerPaginaDeTabla numero de la primer pagina de la tabla
 *
 * @return Tabla de segundo nivel vacia.
 */
TablaSegundoNivel *crear_tabla_segundo_nivel(int nroPrimerPaginaDeTabla);

/**
 * @brief Crear un proceso, asignarle una tabla de primer nivel vacia y agregarlo a la lista de procesos.
 * @param id Id del prcoeso
 * @param tamanio Tamanio del proceso.
 *
 * @return Proceso nuevo.
 */
Proceso *crear_proceso(int id, int tamanio);

void* leer_de_memoria(int direccionFisica, int desplazamiento);

void escribir_en_memoria(void* datosAEscribir, int direccionFisica, int desplazamiento);

/**
 * @brief Escribir un entero en Memoria Principal.
 * @param valorAEscribir El valor que se va a escribir en Memoria Principal.
 * @param direccionFisica 
 */
void escribir_entero_en_memoria(uint32_t valorAEscribir, int direccionFisica);

/**
 * @brief Leer un entero de la Memoria Principal.
 * @param direccionFisica 
 *
 * @return Valor numerico de lo que hay en la direccion en memoria.
 */
uint32_t leer_entero_de_memoria(int direccionFisica);

/**
 * @brief Copia los datos de una direccion fisica en otra
 *
 * @param direccionFisicaDestino
 * @param direccionFisicaOrigen
 */
void copiar_entero_en_memoria(int direccionFisicaDestino, int direccionFisicaOrigen);

/**
 * @brief Busca el proceso en la lista de procesos.
 * @param idProceso Id del proceso a buscar.
 *
 * @return Index del proceso buscado.
 */
Proceso *buscar_proceso_por_id(int idProceso);

/**
 * @brief Le asigna una pagina al primer marco libre marco o realiza sustitucion si no lo hay.
 * @param proceso
 * @param numeroPagina
 *
 * @return
 */
Marco *asignar_pagina_a_marco_libre(Proceso *proceso, int numeroPagina);

/**
 * @brief Asigna una pagina del proceso a un marco
 *
 * @param idProceso
 * @param pagina
 * @param marco
 */
void asignar_pagina_del_proceso_al_marco(int idProceso, Pagina *pagina, Marco *marco);

/**
 * @brief Obtiene la pagina fisica del proceso, en base a su numero de pagina
 *
 * @param proceso
 * @param numeroPagina
 * @return Pagina*
 */
Pagina *obtener_pagina_del_proceso(Proceso *proceso, int numeroPagina);

/**
 * @brief busca el primer marco libre en la memoria principal y lo retorna
 *
 * @return Marco*
 */
Marco *primer_marco_libre();

/**
 * @brief Retorna si el proceso tiene asignados menos marcos que los maximo que puede tener
 *
 * @param proceso
 * @return true
 * @return false
 */
bool tiene_marcos_por_asignar(Proceso *proceso);

/**
 * @brief Suspende un proceso
 *
 * @param idProcesoASuspender
 */
void suspender_proceso(int idProcesoASuspender);

/**
 * @brief Finaliza al proceso borrando sus estructuras
 *
 * @param idProcesoAFinalizar
 */
void finalizar_proceso(int idProcesoAFinalizar);

/**
 * @brief Limpiar el espacio usado por la memoria principal al cerrar el servidor
 *
 *
 */
void liberar_memoria();

/**
 * @brief Borras la tablas de primer nivel y las de segundo nivel del proceso
 *
 * @param proceso
 */
void borrar_tablas_del_proceso(Proceso *proceso);

/**
 * @brief Elimina un proceso de la lista de procesos en memoria
 *
 * @param proceso
 */
void eliminar_proceso_de_lista_de_procesos(Proceso *proceso);

/**
 * @brief Libera los marcos ocupados por el proceso
 *
 * @param idProceso
 */
void desasignar_marcos_al_proceso(int idProceso);

/**
 * @brief Libera un solo marco de un proceso
 *
 * @param marco
 */
void desasignar_marco(Marco *marco);

void desasignar_pagina(Pagina* pagina);


#endif