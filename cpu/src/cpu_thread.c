#include <cpu_thread.h>

void wait_kernel_dispatch(int cpuSocket)
{
  while (true)
  {
    Logger *logger = init_cpu_logger();
    log_info(logger, "Waiting Kernel to connect in dispatch port...");
    int kernelSocket = wait_client(cpuSocket);

    log_info(logger, "Kernel connected to Dispatch port.");
    log_destroy(logger);

    manage_kernel_dispatch_package(kernelSocket);
  }
}

void wait_kernel_interrupt(int cpuSocket)
{
  while (true)
  {
    Logger *logger = init_cpu_logger();
    log_info(logger, "Waiting Kernel to connect in interrupt port...");
    int kernelSocket = wait_client(cpuSocket);

    log_info(logger, "Kernel connected to Interrupt port.");
    log_destroy(logger);

    manage_kernel_interrupt_package(kernelSocket);
  }
}

void manage_kernel_dispatch_package(int kernelSocket)
{
  while (true)
  {
    Logger *logger = init_cpu_logger();
    Pcb *pcb;
    switch (get_operation_code(kernelSocket))
    {
    case PCB:
      pcb = deserialize_PCB(kernelSocket);
      show_PCB(logger, pcb);
      break;

    case DISCONNECTION:
      log_warning(logger, "Kernel just left Dispatch Port.");
      return;

    default:
      break;
    }
  }
}

void manage_kernel_interrupt_package(int kernelSocket)
{
  while (true)
  {
    Logger *logger = init_cpu_logger();
    char *interrupt;

    switch (get_operation_code(kernelSocket))
    {
    case MESSAGE:
      interrupt = get_message_from_client(kernelSocket);
      log_info(logger, "Interrupt: %s", interrupt);
      free(interrupt);
      break;

    case DISCONNECTION:
      log_warning(logger, "Kernel just left Interrupt Port.");
      return;

    default:
      break;
    }
  }
}

void manage_connection_memory(void)
{
  int cpuSocket = connect_to_memory_server();
  send_message_to_server("CPU", cpuSocket);
  release_server_connection(cpuSocket);
}