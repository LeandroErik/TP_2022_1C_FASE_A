#include <consola_utils.h>
#include <socket/protocolo.h>

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

    t_paquete *paquetePrograma = crear_paquete(ENVIAR_PROGRAMA);

    while (!feof(archivoCodigo))
    {
        t_linea_codigo *lineaCodigo = malloc(sizeof(t_linea_codigo));
        leer_lineas_codigo(archivoCodigo, lineaCodigo, listaLineasCodigo);
    }

    int cantidadLineas = list_size(listaLineasCodigo);

    agregar_a_paquete(paquetePrograma, &tamanioProceso, sizeof(int));
    agregar_a_paquete(paquetePrograma, &cantidadLineas, sizeof(int));

    for (int i = 0; i < cantidadLineas; i++)
    {
        t_linea_codigo *lineaCodigo = list_get(listaLineasCodigo, i);
        agregar_a_paquete(paquetePrograma, lineaCodigo->identificador, strlen(lineaCodigo->identificador) + 1);
        agregar_a_paquete(paquetePrograma, &(lineaCodigo->parametros[0]), sizeof(int));
        agregar_a_paquete(paquetePrograma, &(lineaCodigo->parametros[1]), sizeof(int));
    }

    int socketConsola = crear_conexion_con_kernel();

    enviar_paquete(paquetePrograma, socketConsola);

    liberar_conexion_con_kernel(socketConsola);
    terminar_parseo(archivoCodigo, listaLineasCodigo);

    return EXIT_SUCCESS;
}
