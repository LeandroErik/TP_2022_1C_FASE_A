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

    pthread_mutex_init(&mutexColaSuspendidoListo, NULL);
    pthread_mutex_init(&mutexColaFinalizado, NULL);

    sem_init(&semaforoProcesoNuevo, 0, 0);
    sem_init(&semaforoProcesoListo, 0, 0);

    sem_init(&contadorBloqueados, 0, 0);

    sem_init(&analizarSuspension, 0, 0);
    sem_init(&suspensionFinalizada, 0, 0);

    sem_init(&semaforoCantidadProcesosEjecutando, 0, 1);
}
void inicializar_colas_procesos()
{
    colaNuevos = queue_create();
    colaListos = list_create();
    colaEjecutando = queue_create();
    colaBloqueados = queue_create();
    colaSuspendidoListo = queue_create();
    colaFinalizado = queue_create();
}

void iniciar_planificadores()
{
    pthread_create(&hilo_planificador_largo_plazo, NULL, planificador_largo_plazo, NULL);

    pthread_create(&hilo_planificador_mediano_plazo, NULL, planificador_mediano_plazo, NULL);
    log_info(logger, "CONFIG: %s", KERNEL_CONFIG.ALGORITMO_PLANIFICACION);
    if (strcmp(KERNEL_CONFIG.ALGORITMO_PLANIFICACION, "FIFO") != 0)
    {
        pthread_create(&hilo_planificador_corto_plazo_sjf, NULL, planificador_corto_plazo_sjf, NULL);
    }
    else
    {
        pthread_create(&hilo_planificador_corto_plazo_fifo, NULL, planificador_corto_plazo_fifo, NULL);
    }

    pthread_create(&hilo_dispositivo_io, NULL, dispositivo_io, NULL);
}

/*Funciones del proceso*/

void ejecutar(Pcb *proceso)
{
    proceso->escenario->estado = EJECUTANDO;

    proceso->tiempoInicioEjecucion = obtener_tiempo_actual();

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
        log_info(logger, "Recibi el PCB con PID : %d de CPU!", procesoRecibido->pid);
        manejar_proceso_recibido(procesoRecibido);

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
    pcb->tiempoRafagaRealAnterior = obtener_tiempo_actual() - pcb->tiempoInicioEjecucion;

    switch (pcb->escenario->estado)
    {
    case BLOQUEADO_IO:
        log_info(logger, "El proceso %d se encuentra BLOQUEADO por IO por %d ms", pcb->pid, pcb->escenario->tiempoBloqueadoIO);

        agregar_proceso_bloqueado(pcb);

        break;
    case TERMINADO:
        log_info(logger, "El proceso %d se encuentra TERMINADO ", pcb->pid);
        agregar_proceso_finalizado(pcb);

        decrementar_cantidad_procesos_memoria();
        log_info(logger, "Cantidad procesos en MEMORIA %d", cantidadProcesosEnMemoria);
        break;

    default:
        log_info(logger, "El proceso %d es medio raro", pcb->pid);
        break;
    }
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

void *dispositivo_io()
{
    while (1)
    {

        sem_wait(&contadorBloqueados);
        log_info(logger, "ESTOY EJECUTANDO E/S");
        Pcb *proceso = queue_peek(colaBloqueados); // Aun la dejo en la cola de bloqueados,pero leo el proceso
        int tiempoBloqueo = proceso->escenario->tiempoBloqueadoIO / 1000;
        log_info(logger, "Duermo PID: %d ,%d segundos", proceso->pid, tiempoBloqueo);
        usleep(tiempoBloqueo);
        proceso = sacar_proceso_bloqueado(); // Aca lo saco de la cola de bloqueados.(CON EL VALOR ACTUALIZADO)
        if (proceso->escenario->estado == SUSPENDIDO)
        {
            /*AGREGAR A SUSPENDED READY*/
            agregar_proceso_suspendido_listo(proceso);
        }
        else
        {
            /*sino pasarlo a ready normal*/
            agregar_proceso_listo(proceso);
        }
    }
}
void *planificador_largo_plazo()
{
    while (1)
    {

        if (cantidadProcesosEnMemoria < KERNEL_CONFIG.GRADO_MULTIPROGRAMACION && (queue_size(colaNuevos) > 0 || queue_size(colaSuspendidoListo) > 0))
        {
            Pcb *procesoSaliente;

            procesoSaliente = queue_is_empty(colaSuspendidoListo) ? extraer_proceso_nuevo() : extraer_proceso_suspendido_listo();

            procesoSaliente->escenario->estado = LISTO;

            agregar_proceso_listo(procesoSaliente);

            incrementar_cantidad_procesos_memoria();
        }
    }
}

void *planificador_mediano_plazo()
{
    while (1)
    {
        sem_wait(&analizarSuspension);
        log_info(logger, "Analizando si hay algun proceso para SUSPENDER");
        pthread_mutex_lock(&mutexColaBloqueados);

        if (queue_size(colaBloqueados) > 0)
        {
            /*Realizar el chequeo si suspende o no*/
            for (int i = 0; i < queue_size(colaBloqueados); i++)
            {
                Pcb *procesoActual = malloc(sizeof(Pcb));
                procesoActual = queue_peek_at(colaBloqueados, i);

                int tiempoBloqueo = (obtener_tiempo_actual() - procesoActual->tiempoInicioBloqueo);
                log_info(logger, "Tiempo BLOQUEO proceso %d : %d", procesoActual->pid, tiempoBloqueo);

                if (tiempoBloqueo * 1000 >= KERNEL_CONFIG.TIEMPO_MAXIMO_BLOQUEADO && procesoActual->escenario->estado != SUSPENDIDO)
                {
                    log_info(logger, "EL PROCESO SE TENDRIA QUE SUSPENDER ,CON PID : %d (pasaron %d segundos)", procesoActual->pid, tiempoBloqueo);
                    /*Seteo estado suspendido al proceso que esta bloqueado*/
                    procesoActual->escenario->estado = SUSPENDIDO;
                    /*libero espacio en procesos en memoria*/
                    decrementar_cantidad_procesos_memoria();
                }
            }
        }

        pthread_mutex_unlock(&mutexColaBloqueados);
        sem_post(&suspensionFinalizada);
    }
}

void imprimir_colas()
{
    log_info(logger, "\
    \n\tCola nuevos: %s \
    \n\tCola listos: %s \
    \n\tCola ejecutando: %s \
    \n\tCola bloqueados: %s\
    \n\tCola suspended - ready: % s\
    \n\tCola terminados: %s",
             leer_cola(colaNuevos),
             leer_lista(colaListos), leer_cola(colaEjecutando), leer_cola(colaBloqueados), leer_cola(colaSuspendidoListo), leer_cola(colaFinalizado));
}
char *leer_lista(t_list *cola)
{
    char *out = string_new();

    for (int i = 0; i < list_size(cola); i++)
    {

        Pcb *proceso_actual = list_get(cola, i);

        string_append(&out, "[");

        string_append(&out, string_itoa(proceso_actual->pid));
        string_append(&out, "]");
    }
    return out;
}
void *planificador_corto_plazo_fifo()
{
    log_info(logger, "INICIO PLANIFICACION FIFO");
    while (1)
    {
        sem_wait(&semaforoProcesoListo);
        sem_wait(&semaforoCantidadProcesosEjecutando);

        Pcb *procesoEjecutar = sacar_proceso_listo();

        agregar_proceso_ejecutando(procesoEjecutar);

        sem_post(&analizarSuspension);
        sem_wait(&suspensionFinalizada);
        ejecutar(procesoEjecutar);
    }
}
void *planificador_corto_plazo_sjf()
{
    log_info(logger, "INICIO PLANIFICACION SJF");

    while (1)
    {
        sem_wait(&semaforoProcesoListo);
        sem_wait(&semaforoCantidadProcesosEjecutando);

        Pcb *procesoEjecutar = sacar_proceso_mas_corto();

        agregar_proceso_ejecutando(procesoEjecutar);

        sem_post(&analizarSuspension);
        sem_wait(&suspensionFinalizada);
        ejecutar(procesoEjecutar);
    }
}
Pcb *sacar_proceso_mas_corto()
{

    Pcb *pcbSaliente = malloc(sizeof(Pcb));
    /*Replanifico la cola de listos*/

    pthread_mutex_lock(&mutexColaListos);

    log_info(logger, "Replanifico la cola de Listos");
    list_sort(colaListos, &ordenar_segun_tiempo_de_trabajo);
    pcbSaliente = list_remove(colaListos, 0);
    log_info(logger, "\nPID :%d ESTIMACION: %f,RAFAGA ANTERIOR: %d -> RESULTADO: %f \n", pcbSaliente->pid, pcbSaliente->estimacionRafaga, pcbSaliente->tiempoRafagaRealAnterior, obtener_tiempo_de_trabajo(pcbSaliente));
    pthread_mutex_unlock(&mutexColaListos);

    return pcbSaliente;
}

bool ordenar_segun_tiempo_de_trabajo(void *procesoA, void *procesoB)
{
    return obtener_tiempo_de_trabajo((Pcb *)procesoA) < obtener_tiempo_de_trabajo((Pcb *)procesoB);
}

float obtener_tiempo_de_trabajo(Pcb *proceso)
{
    float alfa = KERNEL_CONFIG.ALFA;
    float estimacionAnterior = proceso->estimacionRafaga;
    int rafagaAnterior = proceso->tiempoRafagaRealAnterior * 1000;
    float resultado = alfa * rafagaAnterior + (1 - alfa) * estimacionAnterior;

    return resultado;
}

/*Colas de planificacion*/

void agregar_proceso_nuevo(Pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutexColaNuevos);
    queue_push(colaNuevos, procesoNuevo);
    log_info(logger, "Proceso con PID: %d , agregado a NEW en posicion : %d .", procesoNuevo->pid, queue_size(colaNuevos));
    pthread_mutex_unlock(&mutexColaNuevos);

    /*Despierto plani de mediano plazo*/
    sem_post(&analizarSuspension);
    sem_wait(&suspensionFinalizada);

    imprimir_colas();
}
void agregar_proceso_finalizado(Pcb *procesoFinalizado)
{
    pthread_mutex_lock(&mutexColaFinalizado);
    queue_push(colaFinalizado, procesoFinalizado);
    log_info(logger, "Proceso con PID: %d , agregado a FIN en posicion : %d .", procesoFinalizado->pid, queue_size(colaFinalizado));
    pthread_mutex_unlock(&mutexColaFinalizado);

    imprimir_colas();
}

void agregar_proceso_suspendido_listo(Pcb *procesoSuspendidoListo)
{
    pthread_mutex_lock(&mutexColaSuspendidoListo);
    queue_push(colaSuspendidoListo, procesoSuspendidoListo);
    log_info(logger, "Proceso con PID: %d , agregado a SUSPENDIDO-LISTO en posicion : %d .", procesoSuspendidoListo->pid, queue_size(colaSuspendidoListo));
    pthread_mutex_unlock(&mutexColaSuspendidoListo);
    imprimir_colas();
}
void sacar_proceso_ejecutando()
{

    pthread_mutex_lock(&mutexColaEjecutando);
    queue_pop(colaEjecutando);
    pthread_mutex_unlock(&mutexColaEjecutando);

    sem_post(&semaforoCantidadProcesosEjecutando);
}

Pcb *sacar_proceso_bloqueado()
{
    Pcb *pcbSaliente = malloc(sizeof(Pcb));

    pthread_mutex_lock(&mutexColaBloqueados);
    pcbSaliente = queue_pop(colaBloqueados);
    log_info(logger, "Proceso PID:%d salío de BLOQUEADO.", pcbSaliente->pid);
    pthread_mutex_unlock(&mutexColaBloqueados);

    return pcbSaliente;
}

Pcb *sacar_proceso_listo()
{
    Pcb *pcbSaliente = malloc(sizeof(Pcb));

    pthread_mutex_lock(&mutexColaListos);
    pcbSaliente = list_remove(colaListos, 0);
    pthread_mutex_unlock(&mutexColaListos);

    return pcbSaliente;
}

Pcb *extraer_proceso_nuevo()
{
    pthread_mutex_lock(&mutexColaNuevos);

    Pcb *proceso_saliente = queue_pop(colaNuevos);

    pthread_mutex_unlock(&mutexColaNuevos);

    return proceso_saliente;
}

Pcb *extraer_proceso_suspendido_listo()
{
    pthread_mutex_lock(&mutexColaSuspendidoListo);

    Pcb *proceso_saliente = queue_pop(colaSuspendidoListo);

    pthread_mutex_unlock(&mutexColaSuspendidoListo);

    return proceso_saliente;
}

void agregar_proceso_listo(Pcb *procesoListo)
{
    pthread_mutex_lock(&mutexColaListos);

    list_add(colaListos, procesoListo);
    log_info(logger, "Agregado a READY el proceso : %d .", procesoListo->pid);

    pthread_mutex_unlock(&mutexColaListos);

    sem_post(&semaforoProcesoListo);
    imprimir_colas();
}

void agregar_proceso_ejecutando(Pcb *procesoEjecutando)
{
    pthread_mutex_lock(&mutexColaEjecutando);

    queue_push(colaEjecutando, procesoEjecutando);
    log_info(logger, "Agregado a EJECUTANDO el proceso : %d .", procesoEjecutando->pid);
    pthread_mutex_unlock(&mutexColaEjecutando);

    imprimir_colas();
}

void agregar_proceso_bloqueado(Pcb *procesoBloqueado)
{

    pthread_mutex_lock(&mutexColaBloqueados);
    /*agrego tiempo inicial de bloqueo para la calcular la suspension*/
    procesoBloqueado->tiempoInicioBloqueo = obtener_tiempo_actual();

    queue_push(colaBloqueados, procesoBloqueado);
    log_info(logger, "Proceso con PID: %d , agregado a BLOQUEADO en posicion : %d ", procesoBloqueado->pid, queue_size(colaBloqueados));
    pthread_mutex_unlock(&mutexColaBloqueados);

    /*Aviso al dispositivo de E/S*/
    sem_post(&contadorBloqueados);
    /*Despierto planificador mediano plazo*/
    sem_post(&analizarSuspension);
    sem_wait(&suspensionFinalizada);
    imprimir_colas();
}

/*Monitores*/
void incrementar_cantidad_procesos_memoria()
{
    pthread_mutex_lock(&mutexcantidadProcesosMemoria);
    cantidadProcesosEnMemoria++;
    pthread_mutex_unlock(&mutexcantidadProcesosMemoria);
}
void decrementar_cantidad_procesos_memoria()
{
    pthread_mutex_lock(&mutexcantidadProcesosMemoria);
    cantidadProcesosEnMemoria--;
    pthread_mutex_unlock(&mutexcantidadProcesosMemoria);
}

int obtener_tiempo_actual()
{
    return time(NULL);
}
