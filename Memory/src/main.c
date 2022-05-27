#include <memory_utils.h>

int main(void)
{
  Config *config = config_create("Memory.config");
  Logger *logger = init_memory_logger();

  fill_memory_config(config);

  log_info(logger, "Starting Memory Server...");
  int memorySocket = start_memory_server();

  if (memorySocket < 0)
  {
    log_error(logger, "Error trying to start server.");
    return EXIT_FAILURE;
  }

  Thread clientThread;

  while (true)
  {
    int clientSocket = wait_client(memorySocket);
    pthread_create(&clientThread, NULL, (void *)manage_clients_packages, (void *)clientSocket);
  }

  return EXIT_SUCCESS;
}