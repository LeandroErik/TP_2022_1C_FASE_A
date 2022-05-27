#include <kernel_utils.h>

int main(void)
{
  globalProgramID = 0;
  Config *config = config_create("Kernel.config");
  Logger *logger = init_kernel_logger();

  fill_kernel_config(config);

  log_info(logger, "Starting Kernel Server...");
  int kernelSocket = start_kernel_server();

  if (kernelSocket < 0)
  {
    log_error(logger, "Error trying to start server.");
    return EXIT_FAILURE;
  }

  Thread consolesThread;
  Thread connectInterruptThread;
  Thread connectMemoryThread;

  pthread_create(&consolesThread, NULL, (void *)wait_console, (void *)kernelSocket);
  pthread_create(&connectInterruptThread, NULL, (void *)manage_connection_interrupt, NULL);
  pthread_create(&connectMemoryThread, NULL, (void *)manage_connection_memory, NULL);

  pthread_join(consolesThread, NULL);
  pthread_join(connectInterruptThread, NULL);
  pthread_join(connectMemoryThread, NULL);

  shutdown_server(kernelSocket);
  log_destroy(logger);
  config_destroy(config);

  return EXIT_SUCCESS;
}