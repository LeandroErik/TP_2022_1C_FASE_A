#include <cpu_utils.h>

int main(void)
{
  Config *config = config_create("CPU.config");
  Logger *logger = init_cpu_logger();

  fill_cpu_config(config);

  log_info(logger, "Starting CPU Server...");
  int cpuDispatchSocket = start_cpu_dispatch_server();
  int cpuInterruptSocket = start_cpu_interrupt_server();

  if (cpuDispatchSocket < 0 || cpuInterruptSocket < 0)
  {
    log_error(logger, "Error trying to start server.");
    return EXIT_FAILURE;
  }

  Thread kernelDispatchThread;
  Thread kernelInterruptThread;
  Thread connectMemoryThread;

  pthread_create(&kernelDispatchThread, NULL, (void *)wait_kernel_dispatch, (void *)cpuDispatchSocket);
  pthread_create(&kernelInterruptThread, NULL, (void *)wait_kernel_interrupt, (void *)cpuInterruptSocket);

  pthread_create(&connectMemoryThread, NULL, (void *)manage_connection_memory, NULL);

  pthread_join(kernelDispatchThread, NULL);
  pthread_join(kernelInterruptThread, NULL);

  log_destroy(logger);
  config_destroy(config);
}
