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
Lista *procesos;
Lista *marcos;
void *memoriaPrincipal;
int tablasDePrimerNivel, tablasDeSegundoNivel;

pthread_mutex_t semaforoProcesos;
//Semaforo semaforoMarcos;

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

/**
 * @brief Retorna el marco en el que esta contenida la direccion fisica
 *
 * @param direccionFisica
 * @return Marco*
 */
Marco *marco_de_direccion_fisica(int direccionFisica);

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
 * @brief Agrega la nueva pagina asignada en la posicion correspondiente
 *
 * @param proceso
 * @param pagina
 */
void agregar_pagina_a_paginas_asignadas_del_proceso(Proceso *proceso, Pagina *pagina);

/**
 * @brief Le asigna una pagina al primer marco libre marco o realiza sustitucion si no lo hay.
 * @param proceso
 * @param pagina
 *
 * @return Marco*
 */
Marco *asignar_pagina_a_marco_libre(Proceso *proceso, Pagina *pagina);

/**
 * @brief Asigna una pagina del proceso a un marco
 *
 * @param proceso
 * @param pagina
 * @param marco
 */
void asignar_pagina_del_proceso_al_marco(Proceso *proceso, Pagina *pagina, Marco *marco);

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
void desasignar_marcos_al_proceso(Proceso *proceso);

/**
 * @brief Libera un solo marco de un proceso
 *
 * @param marco
 */
void desasignar_marco(Marco *marco);

/**
 * @brief Pone a la pagina sin un marco asignado.
 *
 * @param proceso
 * @param pagina
 */
void desasignar_pagina(Proceso *proceso, Pagina *pagina);

/**
 * @brief Desasigna el marco asignado a la pagina, lo escribe en swap y lo retorna.
 *
 * @param proceso
 * @param pagina
 * @return Marco*
 */
Marco *desalojar_pagina(Proceso *proceso, Pagina *pagina);

/**
 * @brief Retorna si correponde sustitucion por limite de marcos asignados o memoria llena
 *
 * @param proceso
 * @param marcoLibre
 * @return true
 * @return false
 */
bool hay_que_sustituir_pagina_del_proceso(Proceso *proceso, Marco *marcoLibre);

/**
 * @brief Retorna el marco del proceso que se sustitutuyo
 *
 * @param proceso
 * @return Marco*
 */
Marco *marco_del_proceso_sustituido(Proceso *proceso);

/**
 * @brief Corre el algoritmo de sustitucion Clock
 *
 * @param proceso
 * @param logger
 * @return Marco*
 */
Marco *correr_clock(Proceso *proceso, Logger *logger);

/**
 * @brief Corre el algoritmo de sustitucion Clock Modificado
 *
 * @param proceso
 * @param logger
 * @return Marco*
 */
Marco *correr_clock_modificado(Proceso *proceso, Logger *logger);

/**
 * @brief Saca la pagina de la lista de paginas asignadas a un marco del proceso
 *
 * @param proceso
 * @param pagina
 */
void sacar_pagina_de_paginas_asignadas(Proceso *proceso, Pagina *pagina);

/**
 * @brief Retorna el numero de tabla de segundo nivel de la entrada de la tabla de primer nivel.
 *
 * @param numeroTablaPrimerNivel
 * @param entradaATablaDePrimerNivel
 * @return int
 */
int obtener_numero_tabla_segundo_nivel(int numeroTablaPrimerNivel, int entradaATablaDePrimerNivel);

/**
 * @brief Retorna el numero de marco asignado a la pagina de la entrada de la tabla de segundo nivel.
 * @param numeroTablaSegundoNivel
 * @param entradaATablaDeSegundoNivel
 * @return int
 */
int obtener_numero_marco(int numeroTablaSegundoNivel, int entradaATablaDeSegundoNivel);

/**
 * @brief Retorna el proceso que tiene dicho numero de tabla de segundo nivel.
 *
 * @param numeroTablaSegundoNivel
 * @return Proceso*
 */
Proceso *buscar_proceso_de_tabla_segundo_nivel_numero(int numeroTablaSegundoNivel);

void iniciar_semaforos();

void destruir_semaforos();

void agregar_proceso_a_lista_de_procesos(Proceso* proceso);

void destruir_hilos(Hilo hiloCliente1, Hilo hiloCliente2);

#endif