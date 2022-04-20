#include <memoria_utils.h>
#include <commons/config.h>

int main(int argc, char *argv[])
{
    logger = log_create("memoria.log", "Memoria", true, LOG_LEVEL_DEBUG);

    int socketMemoria = iniciar_servidor_memoria(logger);
    int socketCpu = obtener_socket_cpu(socketMemoria, logger);
    char *mensaje;

    while (true)
    {
        cod_op_cliente codOp = obtener_codigo_operacion(socketCpu);
        switch (codOp)
        {
        case MENSAJE_CLIENTE:
            mensaje = obtener_mensaje(socketCpu);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;

        case DESCONEXION_CLIENTE:
            apagar_servidor_memoria(socketMemoria, logger);
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operación desconocida.");
            break;
        }
    }
    log_destroy(logger);

    return EXIT_SUCCESS;
}