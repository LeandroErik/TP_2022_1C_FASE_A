#include <kernel_thread.h>

void wait_console(int kernelSocket)
{
  while (true)
  {
    Logger *logger = init_kernel_logger();
    log_info(logger, "Waiting a console to connect...");
    int consoleSocket = wait_client(kernelSocket);

    log_info(logger, "New console connected to server.");
    log_destroy(logger);

    Thread consoleThread;
    pthread_create(&consoleThread, NULL, (void *)manage_console_package, (void *)consoleSocket);
    pthread_join(consoleThread, NULL);
  }
}

void manage_console_package(int consoleSocket)
{
  while (true)
  {
    Logger *logger = init_kernel_logger();
    char *message;

    switch (get_operation_code(consoleSocket))
    {
    case MESSAGE:
      message = get_message_from_client(consoleSocket);
      log_info(logger, "A console sent: %s.", message);
      free(message);
      break;

    case CODE_LINES:
      manage_PCB(logger, consoleSocket);
      break;

    case DISCONNECTION:
      log_warning(logger, "A console just left my server.");
      return;

    default:
      break;
    }
  }
}

void manage_PCB(Logger *logger, int consoleSocket)
{

  log_info(logger, "Connecting to CPU Server using Dispatch port...");
  int kernelDispatchSocket = connect_to_cpu_dispatch_server();

  if (kernelDispatchSocket < 0)
  {
    log_error(logger, "Connection refused. Server not initialized.");
    return;
  }

  log_info(logger, "Generating PCB...");
  Package *package = create_package(PCB);

  serialize_PCB(package, generate_PCB(consoleSocket));

  log_info(logger, "Sending PCB...");
  send_package(package, kernelDispatchSocket);

  log_info(logger, "Exiting Server.");
  release_server_connection(kernelDispatchSocket);
  log_destroy(logger);
}

void manage_connection_interrupt(void)
{
  int kernelInterruptSocket = connect_to_cpu_interrupt_server();
  send_message_to_server("An external interruption", kernelInterruptSocket);
  release_server_connection(kernelInterruptSocket);
}

void manage_connection_memory(void)
{
  int kernelSocket = connect_to_memory_server();
  send_message_to_server("Kernel", kernelSocket);
  release_server_connection(kernelSocket);
}