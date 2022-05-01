#include <consola_utils.h>

int main(int argc, char *argv[])
{
    // si no se proporcionan los argumentos necesarios
    // el proceso falla.
    if (argc < 3)
        return EXIT_FAILURE;

    char *rutaArchivo = argv[1];
    int tamanioProceso = atoi(argv[2]);

    FILE *archivoCodigo = fopen(rutaArchivo, "r");
    t_list *listaLineasCodigo = list_create();

    while (!feof(archivoCodigo))
    {
        t_linea_codigo *lineaCodigo = malloc(sizeof(t_linea_codigo));
        leer_lineas_codigo(archivoCodigo, lineaCodigo, listaLineasCodigo);
    }

    for (int i = 0; i < list_size(listaLineasCodigo); i++)
    {
        t_linea_codigo *lineaCodigo = list_get(listaLineasCodigo, i);
        printf("%s %d %d\n", lineaCodigo->identificador, lineaCodigo->parametros[0], lineaCodigo->parametros[1]);
    }

    terminar_parseo(archivoCodigo, listaLineasCodigo);

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
