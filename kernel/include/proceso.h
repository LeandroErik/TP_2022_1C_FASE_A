#ifndef PROCESO_H_
#define PROCESO_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <kernel_utils.h>
#include <socket/protocolo.h>
#include <pthread.h>
#include <semaphore.h>
/*Logger de procesos*/

/*Listas y colas de procesos*/
t_queue *colaNuevos;
t_queue *colaBloqueados;
t_queue *colaListos;
t_queue *colaEjecutando;
t_queue *colaSuspendidoBloqueado;
t_queue *colaSuspendidoListo;

/*semaforos*/
pthread_mutex_t mutexNumeroProceso;
pthread_mutex_t mutex_proceso_listo;

pthread_mutex_t mutexColaNuevos;
pthread_mutex_t mutexColaListos;
pthread_mutex_t mutexColaBloqueados;
pthread_mutex_t mutexColaEjecutando;
pthread_mutex_t mutexColaSuspendidoBloqueado;
pthread_mutex_t mutexColaSuspendidoListo;

pthread_mutex_t mutex_cola;

sem_t semaforoProcesoNuevo;
sem_t semaforoProcesoListo;
sem_t semaforoProcesoEjecutando;
sem_t semaforoCantidadProcesosEjecutando;

/*Hilos*/
pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;

/*Funciones proceso*/
void inicializar_semaforos();

pcb *generar_PCB(t_list *, int);

void liberar_instruccion(t_linea_codigo *);

void liberar_PCB(pcb *);

/*Funciones planificacion*/
void inicializar_colas_procesos();

pcb *extraer_proceso_nuevo();

void agregar_proceso_nuevo(pcb *);
void agregar_proceso_listo(pcb *);
void agregar_proceso_bloqueado(pcb *);
void agregar_proceso_ejecutando(pcb *);
void agregar_proceso_suspendido_bloqueado(pcb *);
void agregar_proceso_suspendido_listo(pcb *);

void *planificador_largo_plazo();
void *planificador_corto_plazo();

void iniciar_planificadores();

#endif