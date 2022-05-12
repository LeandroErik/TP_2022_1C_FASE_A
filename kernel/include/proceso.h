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

/*Listas y colas de procesos*/
t_queue *cola_nuevos;
t_queue *cola_listos;

/*semaforos*/
pthread_mutex_t mutex_numero_proceso;
pthread_mutex_t mutex_nuevo_proceso;
pthread_mutex_t mutex_proceso_listo;
sem_t semaforo_nuevo_proceso;

/*Hilos*/
pthread_t hilo_planificador_largo_plazo;

/*Funciones proceso*/
void inicializar_semaforos();

pcb *generar_PCB(t_list *, int);

void liberar_instruccion(t_linea_codigo *);

void liberar_PCB(pcb *);

/*Funciones planificacion*/
void inicializar_colas_procesos();

void agregar_proceso_nuevo(pcb *);

void *planificador_largo_plazo();

void iniciar_planificadores();

#endif