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
t_queue *cola_nuevos;
t_queue *cola_listos;
t_queue *cola_ejecutando;

/*semaforos*/
pthread_mutex_t mutex_numero_proceso;
pthread_mutex_t mutex_cola_nuevos;
pthread_mutex_t mutex_cola_listos;
pthread_mutex_t mutex_cola_ejecutando;
pthread_mutex_t mutex_proceso_listo;

sem_t semaforo_nuevo_proceso;
sem_t semaforo_listo_proceso;
sem_t semaforo_ejecutando_proceso;

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

void agregar_proceso_nuevo(pcb *);
pcb *extraer_proceso_nuevo();

void agregar_proceso_listo(pcb *);

void agregar_proceso_ejecutando(pcb *);

void *planificador_largo_plazo();
void *planificador_corto_plazo();

void iniciar_planificadores();

#endif