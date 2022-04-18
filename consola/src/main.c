#include <consola_utils.h>

int main(int argc, char *argv[])
{
    int socketConsola = crear_conexion_con_kernel();

    char *input = readline("> ");

    while (strcmp(input, ""))
    {
        enviar_mensaje(input, socketConsola);
        free(input);
        input = readline("> ");
    }

    liberar_conexion_con_kernel(socketConsola);

    return 0;
}
