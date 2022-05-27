#include <kernel_utils.h>

Logger *init_kernel_logger(void)
{
  return log_create("Kernel.log", "Kernel", true, LOG_LEVEL_INFO);
}

int start_kernel_server()
{
  return start_server(KERNEL_CONFIG.IP, KERNEL_CONFIG.PORT_KERNEL);
}

int fill_instruction_lines(List *instructionsList, int clientSocket)
{
  List *flattenProperties = get_package_as_list(clientSocket);

  int processSize = *(int *)list_get(flattenProperties, 0);
  deserialize_instruction_lines(instructionsList, flattenProperties, 1, 1);

  list_destroy(flattenProperties);

  return processSize;
}

Pcb *create_PCB(List *instructionsList, int processSize)
{
  Pcb *pcb = malloc(sizeof(Pcb));

  pcb->pid = globalProgramID;
  pcb->size = processSize;
  pcb->programCounter = 0;
  pcb->pageTable = 0;
  pcb->burstEstimation = KERNEL_CONFIG.INITIAL_ESTIMATION;
  pcb->scene = malloc(sizeof(Scene));
  pcb->scene->state = EXECUTING;
  pcb->scene->ioBlockedTime = 0;
  pcb->instructions = list_duplicate(instructionsList);

  list_destroy(instructionsList);
  globalProgramID++;

  return pcb;
}

Pcb *generate_PCB(int clientSocket)
{
  List *instructionsList = list_create();
  int processSize = fill_instruction_lines(instructionsList, clientSocket);

  return create_PCB(instructionsList, processSize);
}

int connect_to_cpu_dispatch_server(void)
{
  return create_server_connection(KERNEL_CONFIG.IP, KERNEL_CONFIG.PORT_CPU_DISPATCH);
}

int connect_to_cpu_interrupt_server(void)
{
  return create_server_connection(KERNEL_CONFIG.IP, KERNEL_CONFIG.PORT_CPU_INTERRUPT);
}

int connect_to_memory_server(void)
{
  return create_server_connection(KERNEL_CONFIG.IP, KERNEL_CONFIG.PORT_MEMORY);
}