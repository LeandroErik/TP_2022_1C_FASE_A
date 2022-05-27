#include <cpu_utils.h>

Logger *init_cpu_logger(void)
{
  return log_create("CPU.log", "CPU", true, LOG_LEVEL_INFO);
}

int start_cpu_dispatch_server(void)
{
  return start_server(CPU_CONFIG.IP_MEMORY, CPU_CONFIG.PORT_KERNEL_DISPATCH);
}

int start_cpu_interrupt_server(void)
{
  return start_server(CPU_CONFIG.IP_MEMORY, CPU_CONFIG.PORT_KERNEL_INTERRUPT);
}

int connect_to_memory_server(void)
{
  return create_server_connection(CPU_CONFIG.IP_MEMORY, CPU_CONFIG.PORT_MEMORY);
}

void show_instruction_lines(Logger *logger, List *instructions)
{
  InstructionLine *instruction;

  for (int i = 0; i < list_size(instructions); i++)
  {
    instruction = list_get(instructions, i);
    log_info(logger, "Instruction Nº %d: %s\t- Param 1: %d\t- Param 2: %d", i, instruction->instructionName, instruction->params[0], instruction->params[1]);
  }

  delete_instruction_line(instruction);
  list_destroy(instructions);
}

void show_PCB(Logger *logger, Pcb *pcb)
{
  log_info(logger, "PCB Nº %d", pcb->pid);
  log_info(logger, "Process Size: %d", pcb->size);
  log_info(logger, "Burst Estimation: %f", pcb->burstEstimation);
  log_info(logger, "State: %d", pcb->scene->state);
  log_info(logger, "I/O Blocked Time: %d", pcb->scene->ioBlockedTime);
  show_instruction_lines(logger, pcb->instructions);
}