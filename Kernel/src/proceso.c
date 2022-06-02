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

    int socketDispatch = conectar_con_cpu_dispatch();

    if (socketDispatch == -1)
    {
        log_error(logger, "No se pudo conectar con el CPU Dispatch");
        return;
    }

    enviar_pcb(proceso, socketDispatch);

    CodigoOperacion codOperacion = obtener_codigo_operacion(socketDispatch);

    Pcb *procesoRecibido;

    switch (codOperacion)
    {
    case PCB:
        procesoRecibido = deserializar_pcb(socketDispatch);

        manejar_proceso_recibido(procesoRecibido);

        log_info(logger, "Recibi el PCB con PID : %d de CPU!", procesoRecibido->pid);
        break;

    case DESCONEXION:
        log_info(logger, "Se desconectó el CPU Dispatch. %d", codOperacion);
        break;

    default:
        log_warning(logger, "Operación desconocida.");
        break;
    }

    liberar_conexion_con_servidor(socketDispatch);
}

void manejar_proceso_recibido(Pcb *pcb)
{
    sacar_proceso_ejecutando();

    switch (pcb->escenario->estado)
    {
    case BLOQUEADO_IO:
        log_info(logger, "El proceso %d se encuentra bloqueado por IO por %d ms", pcb->pid, pcb->escenario->tiempoBloqueadoIO);

        agregar_proceso_bloqueado(pcb);
        break;
    case TERMINADO:
        log_info(logger, "El proceso %d se encuentra terminado", pcb->pid);

        break;

    default:
        log_info(logger, "El proceso %d es medio raro", pcb->pid);
        break;
    }
}

void sacar_proceso_ejecutando()
{
    pthread_mutex_lock(&mutexColaEjecutando);
    queue_pop(colaEjecutando);
    pthread_mutex_unlock(&mutexColaEjecutando);
    sem_post(&semaforoCantidadProcesosEjecutando);
}

Pcb *sacar_proceso_listo()
{
    Pcb *pcbSaliente = malloc(sizeof(Pcb));

    pthread_mutex_lock(&mutexColaListos);
    pcbSaliente = queue_pop(colaListos);
    pthread_mutex_unlock(&mutexColaListos);

    return pcbSaliente;
}

/*Varios*/

void enviar_pcb(Pcb *proceso, int socketDispatch)
{

    Paquete *paquete = crear_paquete(PCB);

    serializar_pcb(paquete, proceso);

    enviar_paquete_a_servidor(paquete, socketDispatch);

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

        Pcb *proceso_actual = queue_peek_at(cola, i);

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

void imprimir_colas()
{

    log_info(logger, "\
    \n\tCola nuevos: %s \
    \n\tCola listos: %s \
    \n\tCola ejecutando: %s \
    \n\tCola bloqueados: %s",
             leer_cola(colaNuevos), leer_cola(colaListos), leer_cola(colaEjecutando), leer_cola(colaBloqueados));
}

void *planificador_corto_plazo()
{
    while (1)
    {
        sem_wait(&semaforoProcesoListo);
        sem_wait(&semaforoCantidadProcesosEjecutando);

        Pcb *procesoEjecutar = sacar_proceso_listo();

        agregar_proceso_ejecutando(procesoEjecutar);

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
    imprimir_colas();
}

void agregar_proceso_bloqueado(Pcb *procesoBloqueado)
{
    pthread_mutex_lock(&mutexColaBloqueados);
    queue_push(colaBloqueados, procesoBloqueado);
    pthread_mutex_unlock(&mutexColaBloqueados);

    log_info(logger, "Proceso con PID: %d , agregado a BLOQUEADO en posicion : %d .", procesoBloqueado->pid, queue_size(colaBloqueados));
    imprimir_colas();
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
    imprimir_colas();
}

void agregar_proceso_ejecutando(Pcb *procesoEjecutando)
{
    pthread_mutex_lock(&mutexColaEjecutando);

    queue_push(colaEjecutando, procesoEjecutando);

    pthread_mutex_unlock(&mutexColaEjecutando);

    log_info(logger, "Agregado a EJECUTANDO el proceso : %d .", procesoEjecutando->pid);
    imprimir_colas();
}