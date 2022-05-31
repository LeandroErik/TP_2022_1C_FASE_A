#include <proceso.h>
#include <kernel_utils.h>

void ejecutar(pcb *proceso);

void inicializar_semaforos()
{
    pthread_mutex_init(&mutexNumeroProceso, NULL);
    pthread_mutex_init(&mutexColaNuevos, NULL);
    pthread_mutex_init(&mutexColaListos, NULL);
    pthread_mutex_init(&mutexColaEjecutando, NULL);
    pthread_mutex_init(&mutexColaBloqueados, NULL);
    pthread_mutex_init(&mutexColaSuspendidoBloqueado, NULL);
    pthread_mutex_init(&mutexColaSuspendidoListo, NULL);
    pthread_mutex_init(&mutex_proceso_listo, NULL);

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
    nuevo_pcb->estado = NUEVO;

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
    colaBloqueados = queue_create();
    colaSuspendidoBloqueado = queue_create();
    colaSuspendidoListo = queue_create();
}

void agregar_proceso_nuevo(pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutexColaNuevos);
    queue_push(colaNuevos, procesoNuevo);
    pthread_mutex_unlock(&mutexColaNuevos);

    log_info(logger, "Proceso con PID: %d , agregado a NEW en posicion : %d .", procesoNuevo->pid, queue_size(colaNuevos));

    sem_post(&semaforoProcesoNuevo);
}

void agregar_proceso_bloqueado(pcb *procesoBloqueado)
{
    pthread_mutex_lock(&mutexColaBloqueados);
    queue_push(colaBloqueados, procesoBloqueado);
    pthread_mutex_unlock(&mutexColaBloqueados);

    log_info(logger, "Proceso con PID: %d , agregado a BLOQUEADO en posicion : %d .", procesoBloqueado->pid, queue_size(colaBloqueados));
}

void agregar_proceso_suspendido_bloqueado(pcb *procesoSuspendidoBloqueado)
{
    pthread_mutex_lock(&mutexColaSuspendidoBloqueado);
    queue_push(colaSuspendidoBloqueado, procesoSuspendidoBloqueado);
    pthread_mutex_unlock(&mutexColaSuspendidoBloqueado);

    log_info(logger, "Proceso con PID: %d , agregado a SUSPENDIDO-BLOQUEADO en posicion : %d .", procesoSuspendidoBloqueado->pid, queue_size(colaSuspendidoBloqueado));
}

void agregar_proceso_suspendido_listo(pcb *procesoSuspendidoListo)
{
    pthread_mutex_lock(&mutexColaSuspendidoListo);
    queue_push(colaSuspendidoBloqueado, procesoSuspendidoListo);
    pthread_mutex_unlock(&mutexColaSuspendidoListo);

    log_info(logger, "Proceso con PID: %d , agregado a SUSPENDIDO-LISTO en posicion : %d .", procesoSuspendidoListo->pid, queue_size(colaSuspendidoListo));
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

        int procesosEnMemoria = queue_size(colaListos) + queue_size(colaEjecutando) + queue_size(colaBloqueados);

        if (procesosEnMemoria < valoresConfig.GRADO_MULTIPROGRAMACION && queue_size(colaNuevos) > 0)
        {
            pcb *procesoSaliente = extraer_proceso_nuevo();

            procesoSaliente->estado = LISTO;
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
        queue_push(colaEjecutando, procesoEjecutar);
        ejecutar(procesoEjecutar);
    }
}

void ejecutar(pcb *proceso)
{
    proceso->estado = EJECUTANDO;

    enviar_pcb(proceso, socketKernelClienteDispatch, logger);

    cod_op codOp = recibir_operacion(socketKernelClienteDispatch);
    char *mensaje;
    switch (codOp)
    {
    case MENSAJE_CLIENTE_P:
        mensaje = obtener_mensaje(socketKernelClienteDispatch);
        log_info(logger, "Recibí el mensaje: %s", mensaje);
        break;

    case DESCONEXION_CLIENTE_P:
        log_info(logger, "Se desconectó el CPU Dispatch... %d", codOp);
        return 1;
        break;

    default:
        log_warning(logger, "Operación desconocida.");
        break;
    }
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

void agregar_proceso_ejecutando(pcb *procesoEjecutando)
{
    pthread_mutex_lock(&mutexColaEjecutando);

    queue_push(colaEjecutando, procesoEjecutando);

    pthread_mutex_unlock(&mutexColaEjecutando);

    log_info(logger, "Agregado a EJECUTANDO el proceso : %d .", procesoEjecutando->pid);
}