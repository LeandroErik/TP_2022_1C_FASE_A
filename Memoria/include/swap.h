#ifndef SWAP_H
#define SWAP_H

#include <memoria_utils.h>

/**
 * @brief Genera el path que tendra el archivo de swap del proceso
 * 
 * @param idProceso 
 * @return char* path del arhivo de swap
 */
char* generar_path_archivo_swap(int idProceso);

/**
 * @brief Crea el fichero del archivo de swap, en modo escritura
 * 
 * @param idProceso 
 * @return FILE* 
 */
FILE* crear_archivo_swap(int idProceso);

/**
 * @brief Cierra el fichero y elimina el archivo de swap del proceso
 * 
 * @param proceso 
 */
void borrar_archivo_swap_del_proceso(Proceso *proceso);



#endif