#include <proceso.h>
#include <kernel_utils.h>

void inicializar_semaforos()
{
    pthread_mutex_init(&mutex_numero_proceso, NULL);
    pthread_mutex_init(&mutex_cola_nuevos, NULL);
    pthread_mutex_init(&mutex_cola_listos, NULL);
    pthread_mutex_init(&mutex_cola_ejecutando, NULL);

    sem_init(&semaforo_nuevo_proceso, 0, 0);
    sem_init(&semaforo_listo_proceso, 0, 0);
}

pcb *generar_PCB(t_list *listaInstrucciones, int tamanioProceso)
{
    pcb *nuevo_pcb = malloc(sizeof(pcb));

    pthread_mutex_lock(&mutex_numero_proceso);
    nuevo_pcb->pid = id_proceso_total;
    id_proceso_total++;
    pthread_mutex_unlock(&mutex_numero_proceso);

    nuevo_pcb->tamanio = tamanioProceso;
    nuevo_pcb->lista_instrucciones = listaInstrucciones;
    nuevo_pcb->proxima_instruccion = 1;
    nuevo_pcb->tabla_de_paginas = 1;
    nuevo_pcb->estimacion_rafaga = 1.1;

    return nuevo_pcb;
}

void liberar_PCB(pcb *proceso)
{
    list_destroy_and_destroy_elements(proceso->lista_instrucciones, liberar_instruccion);
    free(proceso);
}

void liberar_instruccion(t_linea_codigo *lineaCodigo)
{
    free(lineaCodigo->identificador);
    free(lineaCodigo);
}

/*Planificacion*/
void inicializar_colas_procesos()
{
    cola_nuevos = queue_create();
    cola_listos = queue_create();
    cola_ejecutando = queue_create();
}

void agregar_proceso_nuevo(pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutex_cola_nuevos);
    queue_push(cola_nuevos, procesoNuevo);
    pthread_mutex_unlock(&mutex_cola_nuevos);

    log_info(logger, "Proceso con PID: %d , agregado a NEW en posicion : %d .", procesoNuevo->pid, queue_size(cola_nuevos));

    sem_post(&semaforo_nuevo_proceso);
}

void iniciar_planificadores()
{
    pthread_create(&hilo_planificador_largo_plazo, NULL, planificador_largo_plazo, NULL);
}

void *planificador_largo_plazo()
{

    while (1)
    {
        printf("\tCola nuevos: %s \n\tCola listos: %s \n", leer_cola(cola_nuevos), leer_cola(cola_listos));

        sem_wait(&semaforo_nuevo_proceso);

        if (queue_size(cola_listos) < valores_config.GRADO_MULTIPROGRAMACION && queue_size(cola_nuevos) > 0)
        {
            pcb *procesoSaliente = extraer_proceso_nuevo();

            agregar_proceso_listo(procesoSaliente);
        }
    }
}

void *planificador_corto_plazo()
{
    while (1)
    {
        printf("\tCola Ejecutando: %s\n", leer_cola(cola_ejecutando));

        sem_wait(&semaforo_listo_proceso);

        pcb *procesoEjecutar = queue_pop(cola_listos);
        agregar_proceso_ejecutando(procesoEjecutar);
    }
}
pcb *extraer_proceso_nuevo()
{
    pthread_mutex_lock(&mutex_cola_nuevos);

    pcb *proceso_saliente = queue_pop(cola_nuevos);

    pthread_mutex_unlock(&mutex_cola_nuevos);

    return proceso_saliente;
}

void agregar_proceso_listo(pcb *procesoListo)
{
    pthread_mutex_lock(&mutex_cola_listos);

    queue_push(cola_listos, procesoListo);
    sem_post(&semaforo_listo_proceso);

    pthread_mutex_unlock(&mutex_cola_listos);

    log_info(logger, "Agregado a READY el proceso : %d .", procesoListo->pid);
}

void agregar_proceso_ejecutando(pcb *procesoEjecutar)
{
    pthread_mutex_lock(&mutex_cola_ejecutando);

    queue_push(cola_ejecutando, procesoEjecutar);
    sem_post(&semaforo_ejecutando_proceso);

    pthread_mutex_unlock(&mutex_cola_ejecutando);

    log_info(logger, "Agregado a ejecutando proceso %d", procesoEjecutar->pid);
}
