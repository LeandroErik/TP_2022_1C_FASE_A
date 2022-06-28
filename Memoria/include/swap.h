#ifndef SWAP_H
#define SWAP_H

#include <memoria_utils.h>

/**
 * @brief Genera el path que tendra el archivo de swap del proceso
 *
 * @param idProceso
 * @return char* path del arhivo de swap
 */
char *generar_path_archivo_swap(int idProceso);

/**
 * @brief Crea el fichero del archivo de swap, en modo escritura
 *
 * @param idProceso
 * @return FILE*
 */
FILE *crear_archivo_swap(int idProceso);

/**
 * @brief Cierra el fichero y elimina el archivo de swap del proceso
 *
 * @param proceso
 */
void borrar_archivo_swap_del_proceso(Proceso *proceso);

/**
 * @brief Escribe en el marco la pagina del proceso, traida de swap
 *
 * @param proceso
 * @param numeroPagina
 * @param numeroMarco
 */
void escribir_datos_de_pagina_en_memoria(Proceso *proceso, int numeroPagina, int numeroMarco);

/**
 * @brief Escribe la pagina del proceso en su archivo swap
 *
 * @param pagina
 * @param proceso
 */
void escribir_en_swap(Pagina *pagina, Proceso *proceso);

/**
 * @brief Se realiza la espera en milisegundos por acceso a swap
 *
 */
void realizar_espera_swap();

#endif