#ifndef PROCESO_H_
#define PROCESO_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <protocolo.h>
#include <kernel_utils.h>

/*Listas y colas de procesos*/
t_queue *colaNuevos;
t_queue *colaBloqueados;
t_queue *colaListos;
t_queue *colaEjecutando;
t_queue *colaSuspendidoBloqueado;
t_queue *colaSuspendidoListo;

/*semaforos*/
pthread_mutex_t mutexNumeroProceso;
pthread_mutex_t mutexProcesoListo;

pthread_mutex_t mutexColaNuevos;
pthread_mutex_t mutexColaListos;
pthread_mutex_t mutexColaBloqueados;
pthread_mutex_t mutexColaEjecutando;
pthread_mutex_t mutexColaSuspendidoBloqueado;
pthread_mutex_t mutexColaSuspendidoListo;

pthread_mutex_t mutex_cola;

Semaforo semaforoProcesoNuevo;
Semaforo semaforoProcesoListo;
Semaforo semaforoProcesoEjecutando;
Semaforo semaforoCantidadProcesosEjecutando;

/*Hilos*/
pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;

/*Funciones del proceso*/
void ejecutar(Pcb *);

/*Funciones de inicializacion*/
void inicializar_colas_procesos();
void iniciar_planificadores();
void inicializar_semaforos();

/*Planificadores*/
void *planificador_largo_plazo();
void *planificador_corto_plazo();

/*Transiciones*/
Pcb *extraer_proceso_nuevo();
void agregar_proceso_nuevo(Pcb *);
void agregar_proceso_listo(Pcb *);
void agregar_proceso_bloqueado(Pcb *);
void agregar_proceso_ejecutando(Pcb *);
void agregar_proceso_suspendido_bloqueado(Pcb *);
void agregar_proceso_suspendido_listo(Pcb *);

/*Varios*/

void enviar_pcb(Pcb *, int);
void *queue_peek_at(t_queue *elf, int);
char *leer_cola(t_queue *);

/**
 * @brief Imprime las colas de planificacion
 *
 */
void imprimir_colas();

/**
 * @brief saca al proceso de la cola de ejecucion
 *
 */
void sacar_proceso_ejecutando();

/**
 * @brief Dependiendo del estado del pcb se lo agrega a una de las colas.
 *
 */
void manejar_proceso_recibido(Pcb *pcb);

Pcb *sacar_proceso_listo();

#endif