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
    // config_destroy(config);
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

void *recibir_mensajes(int socketCliente)
{

    t_log *logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    char *mensaje;
    t_list *listaInstrucciones;

    while (true)
    {
        cod_protocolo codOp = obtener_codigo_operacion(socketCliente);
        switch (codOp)
        {
        case INICIAR_PROCESO:
            listaInstrucciones = list_create();

            recibir_lista_intrucciones(socketCliente, listaInstrucciones);

            log_info(logger, "Recibi INICIAR_PROCESO y  la lista de instrucciones");

            iniciar_proceso(listaInstrucciones);

            break;
        case MENSAJE_P:
            mensaje = obtener_mensaje(socketCliente);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;
        case DESCONEXION_P:
            log_info(logger, "Se DESCONECTO un cliente");
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
