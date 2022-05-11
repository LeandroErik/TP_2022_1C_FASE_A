#include <proceso.h>

void inicializar_semaforos()
{
    pthread_mutex_init(&mutex_numero_proceso, NULL);
    pthread_mutex_init(&mutex_nuevo_proceso, NULL);
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
}

void agregar_proceso_nuevo(pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutex_nuevo_proceso);
    queue_push(cola_nuevos, procesoNuevo);
    pthread_mutex_unlock(&mutex_nuevo_proceso);
}

void iniciar_planificadores()
{
    pthread_create(&hilo_planificador_largo_plazo, NULL, planificador_largo_plazo, NULL);
}

void *planificador_largo_plazo()
{
    pthread_mutex_lock(&mutex_nuevo_proceso);
    int largoNuevos = queue_size(cola_nuevos);
    pthread_mutex_unlock(&mutex_nuevo_proceso);

    while (1)
    {
        if (largoNuevos != queue_size(cola_nuevos))
        {
            printf("cola nuevos: %s \n cola listos: %s \n", leer_cola(cola_nuevos), leer_cola(cola_listos));

            if (queue_size(cola_listos) < valores_config.GRADO_MULTIPROGRAMACION)
            {
                pcb *proceso_saliente = queue_pop(cola_nuevos);

                queue_push(cola_listos, proceso_saliente);
            }
        }

        largoNuevos = queue_size(cola_nuevos);
    }
}
