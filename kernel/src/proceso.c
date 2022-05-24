#include <proceso.h>
#include <kernel_utils.h>

void inicializar_semaforos()
{
    pthread_mutex_init(&mutexNumeroProceso, NULL);
    pthread_mutex_init(&mutexColaNuevos, NULL);
    pthread_mutex_init(&mutexColaListos, NULL);
    pthread_mutex_init(&mutexColaEjecutando, NULL);

    sem_init(&semaforoProcesoNuevo, 0, 0);
    sem_init(&semaforoProcesoListo, 0, 0);
    sem_init(&semaforoCantidadProcesosEjecutando, 0, 1);
}

pcb *generar_PCB(t_list *listaInstrucciones, int tamanioProceso)
{
    pcb *nuevo_pcb = malloc(sizeof(pcb));

    pthread_mutex_lock(&mutexNumeroProceso);
    nuevo_pcb->pid = id_proceso_total;
    id_proceso_total++;
    pthread_mutex_unlock(&mutexNumeroProceso);

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
    colaNuevos = queue_create();
    colaListos = queue_create();
    colaEjecutando = queue_create();
}

void agregar_proceso_nuevo(pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutexColaNuevos);
    queue_push(colaNuevos, procesoNuevo);
    pthread_mutex_unlock(&mutexColaNuevos);

    log_info(logger, "Proceso con PID: %d , agregado a NEW en posicion : %d .", procesoNuevo->pid, queue_size(colaNuevos));

    sem_post(&semaforoProcesoNuevo);
}

void iniciar_planificadores()
{
    pthread_create(&hilo_planificador_largo_plazo, NULL, planificador_largo_plazo, NULL);
    pthread_create(&hilo_planificador_corto_plazo, NULL, planificador_corto_plazo, NULL);
}

void *planificador_largo_plazo()
{

    while (1)
    {
        printf("\tCola nuevos: %s \n\tCola listos: %s \n \tCola ejecutando: %s \n", leer_cola(colaNuevos), leer_cola(colaListos), leer_cola(colaEjecutando));

        sem_wait(&semaforoProcesoNuevo);

        if (queue_size(colaListos) < valoresConfig.GRADO_MULTIPROGRAMACION && queue_size(colaNuevos) > 0)
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
        sem_wait(&semaforoProcesoListo);
        sem_wait(&semaforoCantidadProcesosEjecutando);
        pcb *procesoEjecutar = queue_pop(colaListos);
        ejecutar(procesoEjecutar);
    }
}

void ejecutar(pcb *proceso)
{
}

pcb *extraer_proceso_nuevo()
{
    pthread_mutex_lock(&mutexColaNuevos);

    pcb *proceso_saliente = queue_pop(colaNuevos);

    pthread_mutex_unlock(&mutexColaNuevos);

    return proceso_saliente;
}

void agregar_proceso_listo(pcb *procesoListo)
{
    pthread_mutex_lock(&mutexColaListos);

    queue_push(colaListos, procesoListo);
    sem_post(&semaforoProcesoListo);

    pthread_mutex_unlock(&mutexColaListos);

    log_info(logger, "Agregado a READY el proceso : %d .", procesoListo->pid);
}
