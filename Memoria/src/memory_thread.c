#include <memory_thread.h>

void manage_clients_packages(int clientSocket)
{
  Logger *logger = init_memory_logger();
  bool isKernel;

  OperationCode operationCode = get_operation_code(clientSocket);

  switch (operationCode)
  {
  case MESSAGE:
    isKernel = is_kernel(clientSocket);
    if (isKernel)
      log_info(logger, "Kernel just connected");
    else
      log_info(logger, "CPU just connected");
    break;

  case DISCONNECTION:
    return;

  default:
    log_info(logger, "Code: %d. Unknown operation.", operationCode);
    break;
  }

  log_destroy(logger);
}

bool is_kernel(int clientSocket)
{
  char *message = get_message_from_client(clientSocket);
  return !strcmp("Kernel", message);
}