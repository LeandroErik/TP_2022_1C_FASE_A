#include <proceso.h>
#include <kernel_utils.h>

/*Inicializadores*/
void inicializar_semaforos()
{
    pthread_mutex_init(&mutexNumeroProceso, NULL);
    pthread_mutex_init(&mutexColaNuevos, NULL);
    pthread_mutex_init(&mutexColaListos, NULL);
    pthread_mutex_init(&mutexColaEjecutando, NULL);
    pthread_mutex_init(&mutexColaBloqueados, NULL);
    pthread_mutex_init(&mutexColaSuspendidoBloqueado, NULL);
    pthread_mutex_init(&mutexColaSuspendidoListo, NULL);

    pthread_mutex_init(&mutexProcesoListo, NULL); /*Para la lectura de la cola*/

    sem_init(&semaforoProcesoNuevo, 0, 0);
    sem_init(&semaforoProcesoListo, 0, 0);
    sem_init(&semaforoCantidadProcesosEjecutando, 0, 1);
}
void inicializar_colas_procesos()
{
    colaNuevos = queue_create();
    colaListos = queue_create();
    colaEjecutando = queue_create();
    colaBloqueados = queue_create();
    colaSuspendidoBloqueado = queue_create();
    colaSuspendidoListo = queue_create();
}

void iniciar_planificadores()
{
    pthread_create(&hilo_planificador_largo_plazo, NULL, planificador_largo_plazo, NULL);
    pthread_create(&hilo_planificador_corto_plazo, NULL, planificador_corto_plazo, NULL);
}

/*Funciones del proceso*/

void ejecutar(Pcb *proceso)
{
    proceso->escenario->estado = EJECUTANDO;

    enviar_pcb(proceso, socketKernelClienteDispatch);

    CodigoOperacion codOperacion = obtener_codigo_operacion(socketKernelClienteDispatch);

    char *mensaje;
    Pcb *procesoRecibido;

    switch (codOperacion)
    {
    case PCB:
        procesoRecibido = deserializar_pcb(socketKernelClienteDispatch);
        log_info(logger, "Recibi el PCB con PID : %d de CPU!", procesoRecibido->pid);
        break;
    case MENSAJE:
        mensaje = obtener_mensaje_del_cliente(socketKernelClienteDispatch);
        log_info(logger, "Recibí el mensaje: %s", mensaje);
        break;

    case DESCONEXION:
        log_info(logger, "Se desconectó el CPU Dispatch. %d", codOperacion);
        break;

    default:
        log_warning(logger, "Operación desconocida.");
        break;
    }
}

/*Varios*/

void enviar_pcb(Pcb *proceso, int socketReceptor)
{
    Paquete *paquete = crear_paquete(PCB);

    serializar_pcb(paquete, proceso);

    enviar_paquete_a_servidor(paquete, socketReceptor);

    eliminar_paquete(paquete);
}

void *queue_peek_at(t_queue *self, int index)
{
    return list_get(self->elements, index);
}

char *leer_cola(t_queue *cola)
{
    char *out = string_new();

    for (int i = 0; i < queue_size(cola); i++)
    {

        pthread_mutex_lock(&mutexProcesoListo);
        Pcb *proceso_actual = queue_peek_at(cola, i);
        pthread_mutex_unlock(&mutexProcesoListo);

        string_append(&out, "[");

        string_append(&out, string_itoa(proceso_actual->pid));
        string_append(&out, "]");
    }
    return out;
}

/*Planificadores*/

void *planificador_largo_plazo()
{
    while (1)
    {
        printf("\tCola nuevos: %s \n\tCola listos: %s \n \tCola ejecutando: %s \n", leer_cola(colaNuevos), leer_cola(colaListos), leer_cola(colaEjecutando));

        sem_wait(&semaforoProcesoNuevo);

        int procesosEnMemoria = queue_size(colaListos) + queue_size(colaEjecutando) + queue_size(colaBloqueados);

        if (procesosEnMemoria < KERNEL_CONFIG.GRADO_MULTIPROGRAMACION && queue_size(colaNuevos) > 0)
        {
            Pcb *procesoSaliente = extraer_proceso_nuevo();

            procesoSaliente->escenario->estado = EJECUTANDO; /*En realidad seria LISTO*/

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

        Pcb *procesoEjecutar = queue_pop(colaListos);
        queue_push(colaEjecutando, procesoEjecutar);

        ejecutar(procesoEjecutar);
    }
}

/*Colas de planificacion*/

void agregar_proceso_nuevo(Pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutexColaNuevos);
    queue_push(colaNuevos, procesoNuevo);
    pthread_mutex_unlock(&mutexColaNuevos);

    log_info(logger, "Proceso con PID: %d , agregado a NEW en posicion : %d .", procesoNuevo->pid, queue_size(colaNuevos));

    sem_post(&semaforoProcesoNuevo);
}

void agregar_proceso_bloqueado(Pcb *procesoBloqueado)
{
    pthread_mutex_lock(&mutexColaBloqueados);
    queue_push(colaBloqueados, procesoBloqueado);
    pthread_mutex_unlock(&mutexColaBloqueados);

    log_info(logger, "Proceso con PID: %d , agregado a BLOQUEADO en posicion : %d .", procesoBloqueado->pid, queue_size(colaBloqueados));
}

void agregar_proceso_suspendido_bloqueado(Pcb *procesoSuspendidoBloqueado)
{
    pthread_mutex_lock(&mutexColaSuspendidoBloqueado);
    queue_push(colaSuspendidoBloqueado, procesoSuspendidoBloqueado);
    pthread_mutex_unlock(&mutexColaSuspendidoBloqueado);

    log_info(logger, "Proceso con PID: %d , agregado a SUSPENDIDO-BLOQUEADO en posicion : %d .", procesoSuspendidoBloqueado->pid, queue_size(colaSuspendidoBloqueado));
}

void agregar_proceso_suspendido_listo(Pcb *procesoSuspendidoListo)
{
    pthread_mutex_lock(&mutexColaSuspendidoListo);
    queue_push(colaSuspendidoBloqueado, procesoSuspendidoListo);
    pthread_mutex_unlock(&mutexColaSuspendidoListo);

    log_info(logger, "Proceso con PID: %d , agregado a SUSPENDIDO-LISTO en posicion : %d .", procesoSuspendidoListo->pid, queue_size(colaSuspendidoListo));
}

Pcb *extraer_proceso_nuevo()
{
    pthread_mutex_lock(&mutexColaNuevos);

    Pcb *proceso_saliente = queue_pop(colaNuevos);

    pthread_mutex_unlock(&mutexColaNuevos);

    return proceso_saliente;
}

void agregar_proceso_listo(Pcb *procesoListo)
{
    pthread_mutex_lock(&mutexColaListos);

    queue_push(colaListos, procesoListo);
    sem_post(&semaforoProcesoListo);

    pthread_mutex_unlock(&mutexColaListos);

    log_info(logger, "Agregado a READY el proceso : %d .", procesoListo->pid);
}

void agregar_proceso_ejecutando(Pcb *procesoEjecutando)
{
    pthread_mutex_lock(&mutexColaEjecutando);

    queue_push(colaEjecutando, procesoEjecutando);

    pthread_mutex_unlock(&mutexColaEjecutando);

    log_info(logger, "Agregado a EJECUTANDO el proceso : %d .", procesoEjecutando->pid);
}