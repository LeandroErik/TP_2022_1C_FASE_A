#include <kernel_utils.h>

void cargar_configuracion(void)
{
    t_config *config = config_create("kernel.config");
    if (config == NULL)
    {
        perror("archivo kernel.config NO ENCONTRADO");
        return;
    }
    valores_config.IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    valores_config.IP_CPU = config_get_string_value(config, "IP_CPU");
    valores_config.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    valores_config.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    valores_config.PUERTO_CPU_DISPATCH = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    valores_config.PUERTO_CPU_INTERRUPT = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    valores_config.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    valores_config.ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    valores_config.ESTIMACION_INICIAL = config_get_int_value(config, "ESTIMACION_INICIAL");
    valores_config.ALFA = config_get_double_value(config, "ALFA");
    valores_config.GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    valores_config.TIEMPO_MAXIMO_BLOQUEADO = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
}

int iniciar_servidor_kernel(t_log *logger)
{
    int socketKernel = iniciar_servidor(valores_config.IP_KERNEL, valores_config.PUERTO_ESCUCHA);
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
    return crear_conexion_con_servidor(valores_config.IP_CPU, valores_config.PUERTO_CPU_DISPATCH);
}

int crear_conexion_con_cpu_interrupt(void)
{
    return crear_conexion_con_servidor(valores_config.IP_CPU, valores_config.PUERTO_CPU_INTERRUPT);
}

void liberar_conexion_con_cpu(int socketKernel)
{
    liberar_conexion_con_servidor(socketKernel);
}

int crear_conexion_con_memoria(void)
{
    return crear_conexion_con_servidor(valores_config.IP_MEMORIA, valores_config.PUERTO_MEMORIA);
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
            log_info(logger, "Recibi programa");
            listaRecibida = recibir_paquete(socketCliente);
            int primerElemento = *(int *)list_get(listaRecibida, 0);
            listaInstrucciones = deserializar_lineas_codigo(listaRecibida);

            for (int i = 0; i < list_size(listaInstrucciones); i++)
            {
                t_linea_codigo *linea = malloc(sizeof(t_linea_codigo *));
                linea = list_get(listaInstrucciones, i);
                log_info(logger, "linea %i ,identificador %s ,parametro 1: %i,parametro 2 : %i", i, linea->identificador, linea->parametros[0], linea->parametros[1]);
            }

            // loguear los primeros dos elementos
            log_info(logger, "Recibi el primer elemento: %d", primerElemento);

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
