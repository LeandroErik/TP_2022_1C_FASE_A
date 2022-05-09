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

/*semaforos*/
pthread_mutex_t mutex_numero_proceso;

/*Funciones*/
void inicializar_semaforos();
pcb *generar_PCB(t_list *, int);

void liberar_instruccion(t_linea_codigo *);

void liberar_PCB(pcb *);

#endif