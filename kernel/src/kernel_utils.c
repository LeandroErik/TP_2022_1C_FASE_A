#include <kernel_utils.h>

void cargar_configuracion(void)
{
    t_config *config = config_create("kernel.config");
    if (config == NULL)
    {
        perror("archivo kernel.config NO ENCONTRADO");
        return;
    }
    valoresConfig.IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    valoresConfig.IP_CPU = config_get_string_value(config, "IP_CPU");
    valoresConfig.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    valoresConfig.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    valoresConfig.PUERTO_CPU_DISPATCH = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    valoresConfig.PUERTO_CPU_INTERRUPT = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    valoresConfig.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    valoresConfig.ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    valoresConfig.ESTIMACION_INICIAL = config_get_int_value(config, "ESTIMACION_INICIAL");
    valoresConfig.ALFA = config_get_double_value(config, "ALFA");
    valoresConfig.GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    valoresConfig.TIEMPO_MAXIMO_BLOQUEADO = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
}

int iniciar_servidor_kernel(t_log *logger)
{
    int socketKernel = iniciar_servidor(valoresConfig.IP_KERNEL, valoresConfig.PUERTO_ESCUCHA);
    log_info(logger, "Módulo Kernel listo para recibir el Módulo Consola");
    return socketKernel;
}

int obtener_socket_consola(int socketKernel, t_log *logger)
{
    int socketCliente = esperar_cliente(socketKernel);
    log_info(logger, "Se conectó el Módulo de Consola...");

    return socketCliente;
}

void apagar_servidor_kernel(int socketKernel, t_log *logger)
{
    apagar_servidor(socketKernel);
    log_error(logger, "La Consola se desconectó. Apagando Servidor Kernel.");
}

int crear_conexion_con_cpu_dispatch(void)
{
    return crear_conexion_con_servidor(valoresConfig.IP_CPU, valoresConfig.PUERTO_CPU_DISPATCH);
}

int crear_conexion_con_cpu_interrupt(void)
{
    return crear_conexion_con_servidor(valoresConfig.IP_CPU, valoresConfig.PUERTO_CPU_INTERRUPT);
}

void liberar_conexion_con_cpu(int socketKernel)
{
    liberar_conexion_con_servidor(socketKernel);
}

int crear_conexion_con_memoria(void)
{
    return crear_conexion_con_servidor(valoresConfig.IP_MEMORIA, valoresConfig.PUERTO_MEMORIA);
}

void recibir_mensajes(int socketCliente)
{

    t_log *logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    char *mensaje;
    t_list *listaRecibida;
    t_list *listaInstrucciones;

    while (true)
    {
        cod_op codOp = recibir_operacion(socketCliente);

        switch (codOp)
        {

        case MENSAJE_CLIENTE_P:
            mensaje = obtener_mensaje(socketCliente);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;
        case DESCONEXION_CLIENTE_P:
            log_info(logger, "Se DESCONECTO un cliente");
            return;

        case ENVIAR_PROGRAMA:
            log_info(logger, "Recibi un proceso nuevo!");
            listaRecibida = recibir_paquete(socketCliente);
            int tamanioProceso = *(int *)list_get(listaRecibida, 0);

            log_info(logger, "Recibi el primer elemento: %d", tamanioProceso);

            listaInstrucciones = deserializar_lineas_codigo(listaRecibida);

            pcb *nuevoProceso;
            nuevoProceso = generar_PCB(listaInstrucciones, tamanioProceso);
            log_info(logger, "Nuevo proceso creado con PID: %d .", nuevoProceso->pid);

            agregar_proceso_nuevo(nuevoProceso);

            break;

        default:
            log_warning(logger, "Operación desconocida.");
            break;
        }
    }
    log_destroy(logger);
}

void *iniciar_escucha(int socketServidor)
{
    while (1)
    {
        int socketCliente = esperar_cliente(socketServidor);
        pthread_t hiloCliente;

        pthread_create(&hiloCliente, NULL, (void *)recibir_mensajes, (void *)socketCliente);
    }
}

void conectar_memoria(void)
{
    int socketKernelCliente = crear_conexion_con_memoria();

    enviar_mensaje("soy kernel, envio un mensaje al modulo Memoria", socketKernelCliente);

    liberar_conexion_con_servidor(socketKernelCliente);
}

t_list *deserializar_lineas_codigo(t_list *listaRecibida)
{
    t_list *listaLineas = list_create();

    int tamanioLista = *(int *)list_get(listaRecibida, 1);
    int base = 2;
    for (int i = 0; i < tamanioLista; i++)
    {
        t_linea_codigo *linea = malloc(sizeof(t_linea_codigo));

        linea->identificador = (char *)list_get(listaRecibida, base);
        linea->parametros[0] = *(int *)list_get(listaRecibida, base + 1);
        linea->parametros[1] = *(int *)list_get(listaRecibida, base + 2);
        base += 3;
        list_add(listaLineas, linea);

        free(linea->identificador);
        free(linea);
    }

    return listaLineas;
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

        pthread_mutex_lock(&mutex_proceso_listo);
        pcb *proceso_actual = queue_peek_at(cola, i);
        pthread_mutex_unlock(&mutex_proceso_listo);

        string_append(&out, "[");

        string_append(&out, string_itoa(proceso_actual->pid));
        string_append(&out, "]");
    }
    return out;
}

void enviar_pcb(pcb *proceso, int socketCPU, t_log *logger)
{
    t_paquete *paquete = crear_paquete(IMAGEN_PCB_P);

    agregar_a_paquete(paquete, &(proceso->pid), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->tamanio), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->proxima_instruccion), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->tabla_de_paginas), sizeof(int));
    agregar_a_paquete(paquete, &(proceso->estimacion_rafaga), sizeof(float));

    agregar_lista_a_paquete(paquete, proceso);

    enviar_paquete(paquete, socketCPU);

    eliminar_paquete(paquete);
}