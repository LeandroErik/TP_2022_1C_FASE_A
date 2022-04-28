#include <consola_utils.h>

int main(int argc, char *argv[])
{
    // si no se proporcionan los argumentos necesarios
    // el proceso falla.
    if (argc < 3)
        return EXIT_FAILURE;

    char *rutaArchivo = argv[1];
    int tamanioProceso = atoi(argv[2]);

    t_linea_codigo *lineasCodigo = parser_archivo_codigo(rutaArchivo);

    int socketConsola = crear_conexion_con_kernel();

    char *input = readline("> ");

    while (strcmp(input, ""))
    {
        enviar_mensaje(input, socketConsola);
        free(input);
        input = readline("> ");
    }

    liberar_conexion_con_kernel(socketConsola);

    return EXIT_SUCCESS;
}
