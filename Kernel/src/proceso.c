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

    sem_init(&despertarPlanificadorLargoPlazo, 0, 0);
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

    if (strcmp(KERNEL_CONFIG.ALGORITMO_PLANIFICACION, "FIFO") != 0)
    {
        pthread_create(&hilo_planificador_corto_plazo, NULL, planificador_corto_plazo_srt, NULL);
    }
    else
    {
        pthread_create(&hilo_planificador_corto_plazo, NULL, planificador_corto_plazo_fifo, NULL);
    }

    pthread_create(&hilo_dispositivo_io, NULL, dispositivo_io, NULL);
}

/*Funciones del proceso*/

void ejecutar(Pcb *proceso)
{
    enviar_pcb(proceso, socketDispatch);

    log_info(logger, "Envio el proceso con PID : %d de CPU!", proceso->pid);

    CodigoOperacion codOperacion = obtener_codigo_operacion(socketDispatch);

    Pcb *procesoRecibido;

    switch (codOperacion)
    {
    case PCB:
        procesoRecibido = deserializar_pcb(socketDispatch);
        log_info(logger, "Recibi el proceso con PID : %d de CPU!", proceso->pid);
        manejar_proceso_recibido(procesoRecibido, socketDispatch);

        break;

    case DESCONEXION:
        log_info(logger, "Se desconectó el CPU Dispatch. %d", codOperacion);
        break;

    default:
        log_warning(logger, "Operación desconocida.");
        break;
    }
}

void manejar_proceso_recibido(Pcb *pcb, int socketDispatch)
{
    sacar_proceso_ejecutando();
    int pid;
    Paquete *paquete;

    switch (pcb->escenario->estado)
    {
    case INTERRUPCION_EXTERNA:
        log_info(loggerPlanificacion, "[INTERRUPCION]Proceso : [%d] fue INTERRUPIDO.", pcb->pid);
        log_info(loggerPlanificacion, "Tiempo ejecucion [%d]: %d.", pcb->pid, obtener_tiempo_actual() - pcb->tiempoInicioEjecucion);
        manejar_proceso_interrumpido(pcb);

        break;

    case BLOQUEADO_IO:

        agregar_proceso_bloqueado(pcb);

        // Comienza el analisis de suspension (10 segundos)
        Hilo hiloMonitorizacionSuspension;
        pthread_create(&hiloMonitorizacionSuspension, NULL, &monitorizarSuspension, pcb);

        break;

    case TERMINADO:
        agregar_proceso_finalizado(pcb);

        decrementar_cantidad_procesos_memoria();
        log_info(loggerPlanificacion, "Procesos en MEMORIA: %d", cantidadProcesosEnMemoria);

        pid = pcb->pid;
        paquete = crear_paquete(FINALIZAR_PROCESO);
        agregar_a_paquete(paquete, &pid, sizeof(unsigned int));
        enviar_paquete_a_servidor(paquete, socketMemoria);
        log_info(logger, "Se envio el proceso %d a la memoria para finalizar", pid);
        obtener_mensaje_del_cliente(socketMemoria); // confirmacion de finalizacion

        // TODO:Avisar a consola asi se finaliza.
        //  https://github.com/sisoputnfrba/tp-2022-1c-FASE_A/issues/23

        break;

    default:
        log_warning(loggerPlanificacion, "El proceso %d con operacion desconocida.", pcb->pid);
        break;
    }
}

void manejar_proceso_interrumpido(Pcb *pcb)
{
    Pcb *pcbEjecutar = pcb;
    // comparar lo que le falta con las estimaciones de los listos

    float tiempoQueYaEjecuto = obtener_tiempo_actual() - pcb->tiempoInicioEjecucion;

    float estimacionEnSegundos = obtener_tiempo_de_trabajo_actual(pcb) / 1000;

    float tiempoRestanteEnSegundos = estimacionEnSegundos - tiempoQueYaEjecuto;

    log_info(loggerPlanificacion, "[INTERRUPCION] Proceso: [%d] ,(cpu: %f), estimación restante: %f.", pcb->pid, tiempoQueYaEjecuto, tiempoRestanteEnSegundos);

    if (!list_is_empty(colaListos))
    {
        // Ordeno segun esos valores
        list_sort(colaListos, &ordenar_segun_tiempo_de_trabajo);

        log_info(loggerPlanificacion, "[INTERRUPCION] Se reordenó la cola Listos.");

        log_info(loggerPlanificacion, "[INTERRUPCION]Analizando si suspender o ejecutar al actual.");
        // Falta aplicar mutua exclusion
        Pcb *pcbMasCortoDeListos = list_get(colaListos, 0);

        float tiempoPcbMasCortoEnSegundos = obtener_tiempo_de_trabajo_actual(pcbMasCortoDeListos) / 1000;
        log_info(loggerPlanificacion, "[INTERRUPCION] Proceso menor [%d], con estimacion: %f.", pcbMasCortoDeListos->pid, tiempoPcbMasCortoEnSegundos);

        if (tiempoRestanteEnSegundos > tiempoPcbMasCortoEnSegundos)
        {
            log_info(loggerPlanificacion, "Proceso:[%d] se DESALOJA.", pcbEjecutar->pid);
            agregar_proceso_listo(pcb);
            // aca pongo a ejecutar al mas corto,caso contrario sigue ejecutando el otro
            sem_wait(&semaforoProcesoListo); // Asi decremento el semaforo y no saca a ejecutar a otro que no existe

            pcbEjecutar = sacar_proceso_mas_corto(colaListos);
        }
    }
    pthread_mutex_unlock(&mutexColaListos);
    log_info(loggerPlanificacion, "[INTERRUPCION] Proceso:[%d] se ejecuta con puntero en %d", pcbEjecutar->pid, pcbEjecutar->contadorPrograma);

    agregar_proceso_ejecutando(pcbEjecutar);

    ejecutar(pcbEjecutar);
}

void monitorizarSuspension(Pcb *proceso)
{
    int tiempoMaximoBloqueo = KERNEL_CONFIG.TIEMPO_MAXIMO_BLOQUEADO;
    // Pasados 10 segundos de bloqueado ,se suspende.
    int tiempoMaximoBloqueoEnMicrosegundos = tiempoMaximoBloqueo * 1000;

    usleep(tiempoMaximoBloqueoEnMicrosegundos);

    if (proceso->escenario->estado == BLOQUEADO_IO)
    {
        proceso->escenario->estado = SUSPENDIDO;
        log_info(loggerPlanificacion, "El proceso: [%d],se movio a SUSPENDIDO-BLOQUEADO", proceso->pid);

        decrementar_cantidad_procesos_memoria();

        int pid = proceso->pid;
        Paquete *paquete = crear_paquete(SUSPENDER_PROCESO);
        agregar_a_paquete(paquete, &pid, sizeof(unsigned int));
        enviar_paquete_a_servidor(paquete, socketMemoria);
        log_info(logger, "Se envio el proceso %d a la memoria para suspender", pid);
        obtener_mensaje_del_cliente(socketMemoria); // confirmacion de suspension

        // TODO:Recibir confirmacion de suspension
        // https://github.com/sisoputnfrba/tp-2022-1c-FASE_A/issues/24
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

/*Planificadores*/

void *dispositivo_io()
{
    while (1)
    {

        sem_wait(&contadorBloqueados);

        Pcb *proceso = queue_peek(colaBloqueados);

        int tiempoBloqueo = proceso->escenario->tiempoBloqueadoIO;

        log_info(loggerPlanificacion, "[DISP I/O] Proceso: [%d] ,se bloqueara %d segundos", proceso->pid, tiempoBloqueo / 1000);
        // Bloqueo el proceso.
        int tiempoBloqueoEnMicrosegundos = tiempoBloqueo * 1000;

        usleep(tiempoBloqueoEnMicrosegundos);

        log_info(loggerPlanificacion, "[DISP I/O] Proceso: [%d] ,termino I/O %d segundos.", proceso->pid, tiempoBloqueo / 1000);

        proceso = sacar_proceso_bloqueado();
        // Analizo si fue suspendido.
        if (proceso->escenario->estado == SUSPENDIDO)
        {

            agregar_proceso_suspendido_listo(proceso);
        }
        else
        {

            agregar_proceso_listo(proceso);
        }
    }
}

void *planificador_largo_plazo()
{
    log_info(loggerPlanificacion, "Inicio planificacion LARGO PLAZO [%s]", KERNEL_CONFIG.ALGORITMO_PLANIFICACION);
    while (1)
    {
        sem_wait(&despertarPlanificadorLargoPlazo);
        log_info(loggerPlanificacion, "[LARGO-PLAZO] Procesos en MEMORIA: %d", cantidadProcesosEnMemoria);

        if (sePuedeAgregarMasProcesos())
        {
            Pcb *procesoSaliente;

            procesoSaliente = queue_is_empty(colaSuspendidoListo) ? extraer_proceso_nuevo() : extraer_proceso_suspendido_listo();
            // Agrego verificacion de que sea nuevo,porque puede que entre a listo de suspendido.
            if (procesoSaliente->escenario->estado = NUEVO)
            {
                int tablaPaginasPrimerNivel = tabla_pagina_primer_nivel(procesoSaliente->pid, procesoSaliente->tamanio);
                procesoSaliente->tablaPaginas = tablaPaginasPrimerNivel;
            }

            agregar_proceso_listo(procesoSaliente);

            // Envio interrupcion por cada vez que que entra uno a ready
            bool esSrt = strcmp(KERNEL_CONFIG.ALGORITMO_PLANIFICACION, "SRT") == 0;

            if (esSrt)
            {
                enviar_interrupcion();
            }

            incrementar_cantidad_procesos_memoria();
        }
    }
}

bool sePuedeAgregarMasProcesos()
{
    return cantidadProcesosEnMemoria < KERNEL_CONFIG.GRADO_MULTIPROGRAMACION && (lectura_cola_mutex(colaNuevos, &mutexColaNuevos) > 0 || lectura_cola_mutex(colaSuspendidoListo, &mutexColaSuspendidoListo) > 0);
}

void *planificador_corto_plazo_fifo()
{
    log_info(loggerPlanificacion, "INICIO PLANIFICACION FIFO");
    while (1)
    {
        sem_wait(&semaforoProcesoListo);
        sem_wait(&semaforoCantidadProcesosEjecutando);

        Pcb *procesoEjecutar = sacar_proceso_listo();

        agregar_proceso_ejecutando(procesoEjecutar);

        ejecutar(procesoEjecutar);
    }
}

void *planificador_corto_plazo_srt()
{
    log_info(logger, "INICIO PLANIFICACION SRT");

    while (1)
    {

        sem_wait(&semaforoProcesoListo);
        sem_wait(&semaforoCantidadProcesosEjecutando);

        Pcb *procesoEjecutar = sacar_proceso_mas_corto();

        agregar_proceso_ejecutando(procesoEjecutar);

        ejecutar(procesoEjecutar);
    }
}
Pcb *sacar_proceso_mas_corto()
{

    Pcb *pcbSaliente;
    /*Replanifico la cola de listos*/

    pthread_mutex_lock(&mutexColaListos);

    list_sort(colaListos, &ordenar_segun_tiempo_de_trabajo);

    pcbSaliente = list_remove(colaListos, 0);

    log_info(loggerPlanificacion, "\n[REPLANIFICACION] Proceso:[%d] rafaga anterior: %d ,estimacion anterior: %f-> estimacion actual: %f \n", pcbSaliente->pid, pcbSaliente->tiempoRafagaRealAnterior, pcbSaliente->estimacionRafaga, obtener_tiempo_de_trabajo_actual(pcbSaliente));

    pthread_mutex_unlock(&mutexColaListos);

    return pcbSaliente;
}

/*Funciones para aniadir proceso a cola*/

void agregar_proceso_nuevo(Pcb *procesoNuevo)
{
    pthread_mutex_lock(&mutexColaNuevos);

    queue_push(colaNuevos, procesoNuevo);
    log_info(loggerPlanificacion, "Proceso:[%d] se movio NUEVO.", procesoNuevo->pid);

    pthread_mutex_unlock(&mutexColaNuevos);

    /*Despierto al Planificador de Largo Plazo*/
    sem_post(&despertarPlanificadorLargoPlazo);

    imprimir_colas();
}
void agregar_proceso_listo(Pcb *procesoListo)
{

    pthread_mutex_lock(&mutexColaListos);

    procesoListo->escenario->estado = LISTO;
    list_add(colaListos, procesoListo);
    log_info(loggerPlanificacion, "Proceso:[%d] se movio LISTO.", procesoListo->pid);

    pthread_mutex_unlock(&mutexColaListos);

    sem_post(&semaforoProcesoListo);

    imprimir_colas();
}

int tabla_pagina_primer_nivel(int pid, int tamanio)
{

    Paquete *paquete = crear_paquete(PROCESO_NUEVO);

    agregar_a_paquete(paquete, &pid, sizeof(unsigned int));
    agregar_a_paquete(paquete, &tamanio, sizeof(unsigned int));

    enviar_paquete_a_servidor(paquete, socketMemoria);

    log_info(logger, "Se envio el proceso %d a la memoria", pid);

    CodigoOperacion codOperacion = obtener_codigo_operacion(socketMemoria);

    char *mensajeDeMemoria;
    int tablaPrimerNivel;

    switch (codOperacion)
    {
    case MENSAJE:

        mensajeDeMemoria = obtener_mensaje_del_cliente(socketMemoria);
        tablaPrimerNivel = atoi(mensajeDeMemoria);
        log_info(logger, "Se recibio de memoria el numero de tabla de primer nivel del proceso");
        break;

    default:
        log_warning(logger, "Operación desconocida.");
        return EXIT_FAILURE;
    }

    eliminar_paquete(paquete);

    return tablaPrimerNivel;
}

void agregar_proceso_ejecutando(Pcb *procesoEjecutando)
{
    pthread_mutex_lock(&mutexColaEjecutando);
    if (procesoEjecutando->escenario->estado != INTERRUPCION_EXTERNA)
    {
        // Aca si pongo tiempo inicio ejecucion asi no cambia por cada interrupcion(acepta que venga de LISTO)
        procesoEjecutando->tiempoInicioEjecucion = obtener_tiempo_actual();
        log_info(loggerPlanificacion, "Se puso tiempo inicio ejecucion proceso %d", procesoEjecutando->pid);
    }
    procesoEjecutando->escenario->estado = EJECUTANDO;

    queue_push(colaEjecutando, procesoEjecutando);
    log_info(loggerPlanificacion, "Proceso:[%d] se movio EJECUTANDO.", procesoEjecutando->pid);

    pthread_mutex_unlock(&mutexColaEjecutando);

    imprimir_colas();
}

void agregar_proceso_bloqueado(Pcb *procesoBloqueado)
{

    pthread_mutex_lock(&mutexColaBloqueados);
    // Se guarda la estimacion anterior
    procesoBloqueado->estimacionRafaga = obtener_tiempo_de_trabajo_actual(procesoBloqueado);

    // Se guarda tiempo de ejecucion anterior
    procesoBloqueado->tiempoRafagaRealAnterior = obtener_tiempo_actual() - procesoBloqueado->tiempoInicioEjecucion;

    log_info(loggerPlanificacion, "Proceso [%d] rafaga real anterior: %d", procesoBloqueado->pid, procesoBloqueado->tiempoRafagaRealAnterior);
    log_info(loggerPlanificacion, "Proceso [%d] estimacion rafaga anterior: %f", procesoBloqueado->pid, procesoBloqueado->estimacionRafaga);

    procesoBloqueado->escenario->estado = BLOQUEADO_IO;

    queue_push(colaBloqueados, procesoBloqueado);

    log_info(loggerPlanificacion, "Proceso: [%d] se movio a BLOQUEADO (%d segundos).", procesoBloqueado->pid, procesoBloqueado->escenario->tiempoBloqueadoIO / 1000);

    pthread_mutex_unlock(&mutexColaBloqueados);

    // Despierto dispositivo I/O
    sem_post(&contadorBloqueados);

    // Despierto al planificador de largo plazo

    sem_post(&despertarPlanificadorLargoPlazo);

    imprimir_colas();
}
void agregar_proceso_finalizado(Pcb *procesoFinalizado)
{
    pthread_mutex_lock(&mutexColaFinalizado);

    queue_push(colaFinalizado, procesoFinalizado);
    log_info(loggerPlanificacion, "Proceso:[%d] se encuentra FINALIZADO.", procesoFinalizado->pid);

    pthread_mutex_unlock(&mutexColaFinalizado);

    // Despierto al planificador de mediano plazo.
    sem_post(&despertarPlanificadorLargoPlazo);

    imprimir_colas();
}

void agregar_proceso_suspendido_listo(Pcb *procesoSuspendidoListo)
{
    pthread_mutex_lock(&mutexColaSuspendidoListo);

    procesoSuspendidoListo->escenario->estado = LISTO;
    queue_push(colaSuspendidoListo, procesoSuspendidoListo);
    log_info(loggerPlanificacion, "Proceso:[%d] se encuentra SUSPENDIDO-LISTO.", procesoSuspendidoListo->pid);

    pthread_mutex_unlock(&mutexColaSuspendidoListo);

    // Despierto al Planificador de Largo Plazo
    sem_post(&despertarPlanificadorLargoPlazo);

    imprimir_colas();
}

/*Funciones para sacar procesos a cola.*/

void sacar_proceso_ejecutando()
{

    pthread_mutex_lock(&mutexColaEjecutando);

    Pcb *pcbSaliente = queue_pop(colaEjecutando);
    log_info(loggerPlanificacion, "Proceso : [%d] salío de EJECUTANDO.", pcbSaliente->pid);
    pthread_mutex_unlock(&mutexColaEjecutando);
    // le aviso al planificador de corto plazo
    sem_post(&semaforoCantidadProcesosEjecutando);

    /*Despierto al Planificador de Largo Plazo*/
    sem_post(&despertarPlanificadorLargoPlazo);

    imprimir_colas();
}

Pcb *sacar_proceso_bloqueado()
{
    Pcb *pcbSaliente;

    pthread_mutex_lock(&mutexColaBloqueados);
    pcbSaliente = queue_pop(colaBloqueados);
    log_info(loggerPlanificacion, "Proceso : [%d] salío de BLOQUEADO. (real ant : %d)", pcbSaliente->pid, pcbSaliente->tiempoRafagaRealAnterior);
    pthread_mutex_unlock(&mutexColaBloqueados);

    // Envio interrupcion por cada vez que sale de bloqueado algun proceso.
    bool esSrt = strcmp(KERNEL_CONFIG.ALGORITMO_PLANIFICACION, "SRT") == 0;

    if (esSrt)
    {
        enviar_interrupcion();
    }

    return pcbSaliente;
}

Pcb *sacar_proceso_listo()
{

    pthread_mutex_lock(&mutexColaListos);
    Pcb *pcbSaliente = list_remove(colaListos, 0);
    log_info(loggerPlanificacion, "Proceso : [%d] salío de LISTO.", pcbSaliente->pid);
    pthread_mutex_unlock(&mutexColaListos);

    return pcbSaliente;
}

Pcb *extraer_proceso_nuevo()
{
    pthread_mutex_lock(&mutexColaNuevos);

    Pcb *pcbSaliente = queue_pop(colaNuevos);
    log_info(loggerPlanificacion, "Proceso : [%d] salío de NUEVO.", pcbSaliente->pid);

    pthread_mutex_unlock(&mutexColaNuevos);

    return pcbSaliente;
}

Pcb *extraer_proceso_suspendido_listo()
{
    pthread_mutex_lock(&mutexColaSuspendidoListo);

    Pcb *pcbSaliente = queue_pop(colaSuspendidoListo);
    log_info(logger, "Proceso : [%d] salío de SUSPENDIDO-LISTO.", pcbSaliente->pid);

    pthread_mutex_unlock(&mutexColaSuspendidoListo);

    return pcbSaliente;
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

int lectura_cola_mutex(t_queue *cola, pthread_mutex_t *semaforoMutex)
{
    int tamanio;
    pthread_mutex_lock(semaforoMutex);
    tamanio = queue_size(cola);
    pthread_mutex_unlock(semaforoMutex);
    return tamanio;
}

// Varios
void imprimir_colas()
{
    log_info(loggerPlanificacion, "\
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
int obtener_tiempo_actual()
{
    return time(NULL);
}

float obtener_tiempo_de_trabajo_actual(Pcb *proceso)
{
    float alfa = KERNEL_CONFIG.ALFA;
    float estimacionAnterior = proceso->estimacionRafaga;
    int rafagaAnterior = proceso->tiempoRafagaRealAnterior * 1000;

    float resultado = alfa * rafagaAnterior + (1 - alfa) * estimacionAnterior;

    return resultado;
}

bool ordenar_segun_tiempo_de_trabajo(void *procesoA, void *procesoB)
{
    return obtener_tiempo_de_trabajo_actual((Pcb *)procesoA) < obtener_tiempo_de_trabajo_actual((Pcb *)procesoB);
}

void liberar_estructuras()
{
    queue_destroy_and_destroy_elements(colaListos, (void *)liberar_pcb);

    queue_destroy_and_destroy_elements(colaNuevos, (void *)liberar_pcb);

    queue_destroy_and_destroy_elements(colaBloqueados, (void *)liberar_pcb);

    queue_destroy_and_destroy_elements(colaEjecutando, (void *)liberar_pcb);

    queue_destroy_and_destroy_elements(colaSuspendidoListo, (void *)liberar_pcb);

    queue_destroy_and_destroy_elements(colaFinalizado, (void *)liberar_pcb);
}
void liberar_pcb(Pcb *pcb)
{
    free(pcb->escenario);
    // list_destroy_and_destroy_elements(pcb->instrucciones, liberar_instruccion);

    free(pcb);
}
void liberar_instruccion(LineaInstruccion *linea)
{
    free(linea->identificador);
}

void liberar_semaforos()
{
    pthread_mutex_destroy(&mutexNumeroProceso);
    pthread_mutex_destroy(&mutexProcesoListo);
    pthread_mutex_destroy(&mutexColaNuevos);
    pthread_mutex_destroy(&mutexColaListos);
    pthread_mutex_destroy(&mutexColaBloqueados);
    pthread_mutex_destroy(&mutexColaEjecutando);
    pthread_mutex_destroy(&mutexColaFinalizado);
    pthread_mutex_destroy(&mutexColaSuspendidoListo);
    pthread_mutex_destroy(&mutex_cola);
    pthread_mutex_destroy(&mutexcantidadProcesosMemoria);

    sem_destroy(&semaforoProcesoNuevo);
    sem_destroy(&semaforoProcesoListo);
    sem_destroy(&semaforoProcesoEjecutando);
    sem_destroy(&contadorBloqueados);
    sem_destroy(&analizarSuspension);
    sem_destroy(&suspensionFinalizada);
    sem_destroy(&despertarPlanificadorLargoPlazo);
    sem_destroy(&semaforoCantidadProcesosEjecutando);
}
