#include <kernel_utils.h>
#include <commons/config.h>

int main(int argc, char *argv[])
{
    cargar_configuracion();

    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    int socketKernel = iniciar_servidor_kernel(logger);
    pthread_t hiloEscucha;

    pthread_create(&hiloEscucha, NULL, (void *)iniciar_escucha, (void *)socketKernel);
    pthread_join(hiloEscucha, NULL);

    log_destroy(logger);

    return EXIT_SUCCESS;
}
