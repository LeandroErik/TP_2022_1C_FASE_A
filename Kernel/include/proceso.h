#ifndef PROCESO_H_
#define PROCESO_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <protocolo.h>
#include <kernel_utils.h>

/*Listas y colas de procesos*/
t_queue *colaNuevos;
t_queue *colaBloqueados;
t_list *colaListos;
t_queue *colaEjecutando;
t_queue *colaFinalizado;
t_queue *colaSuspendidoListo;

/*semaforos*/
pthread_mutex_t mutexNumeroProceso;
pthread_mutex_t mutexProcesoListo;

pthread_mutex_t mutexColaNuevos;
pthread_mutex_t mutexColaListos;
pthread_mutex_t mutexColaBloqueados;
pthread_mutex_t mutexColaEjecutando;
pthread_mutex_t mutexColaFinalizado;
pthread_mutex_t mutexColaSuspendidoListo;

pthread_mutex_t mutex_cola;
pthread_mutex_t mutexcantidadProcesosMemoria;

Semaforo semaforoProcesoNuevo;
Semaforo semaforoProcesoListo;
Semaforo semaforoProcesoEjecutando;

Semaforo contadorBloqueados;

Semaforo analizarSuspension;
Semaforo suspensionFinalizada;
Semaforo despertarPlanificadorLargoPlazo;

Semaforo semaforoCantidadProcesosEjecutando;

/*Hilos*/
Hilo hilo_planificador_largo_plazo;
Hilo hilo_planificador_corto_plazo;
Hilo hilo_dispositivo_io;
/*Contador de procesos en memoria*/
int cantidadProcesosEnMemoria;

/*Funciones del proceso*/
void ejecutar(Pcb *);

/*Funciones de inicializacion*/
void inicializar_colas_procesos();
void iniciar_planificadores();
void inicializar_semaforos();

/*Planificadores*/
void *planificador_largo_plazo();
void *planificador_mediano_plazo();
void *planificador_corto_plazo_fifo();

void *planificador_corto_plazo_sjf();

void *dispositivo_io();

void *monitorizarSuspension(Pcb *);

/*Transiciones*/
void agregar_proceso_nuevo(Pcb *);
void agregar_proceso_listo(Pcb *);
void agregar_proceso_bloqueado(Pcb *);
void agregar_proceso_ejecutando(Pcb *);
void agregar_proceso_suspendido_bloqueado(Pcb *);
void agregar_proceso_suspendido_listo(Pcb *);
void agregar_proceso_finalizado(Pcb *);
Pcb *extraer_proceso_nuevo();
Pcb *extraer_proceso_suspendido_listo();

/*Planificacion SJF*/
Pcb *sacar_proceso_mas_corto();
float obtener_tiempo_de_trabajo(Pcb *);
bool ordenar_segun_tiempo_de_trabajo(void *, void *);

/*Varios*/

int calulo_tiempo_bloqueo_total(Pcb *);

void enviar_pcb(Pcb *, int);
void *queue_peek_at(t_queue *elf, int);
char *leer_cola(t_queue *);
char *leer_lista(t_list *);

/*Monitores de variables globales*/
void incrementar_cantidad_procesos_memoria();
void decrementar_cantidad_procesos_memoria();
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
 * @brief saca al proceso de la cola de bloqueados
 *
 */
Pcb *sacar_proceso_bloqueado();

/**
 * @brief Dependiendo del estado del pcb se lo agrega a una de las colas.
 *
 */
void manejar_proceso_recibido(Pcb *pcb);

Pcb *sacar_proceso_listo();

/**
 * @brief Instante actual en segundos
 *
 *@returns segundos desde 01 / 01 / 1970
 */
int obtener_tiempo_actual();
int tiempo_total_bloqueado();

#endif