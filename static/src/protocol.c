#include <protocol.h>

// ------- Client & Server functions -------

void delete_package(Package *package)
{
  free(package->buffer->stream);
  free(package->buffer);
  free(package);
}

void delete_instruction_line(InstructionLine *instructionLine)
{
  free(instructionLine->instructionName);
  free(instructionLine);
}

void delete_PCB(Pcb *pcb)
{
  // delete_instruction_line(pcb->instructions);
  free(pcb);
}

// ------- Client functions -------

void init_buffer(Package *package)
{
  package->buffer = malloc(sizeof(Buffer));
  package->buffer->size = 0;
  package->buffer->stream = NULL;
}

Package *create_package(OperationCode operationCode)
{
  Package *package = malloc(sizeof(Package));

  package->operationCode = operationCode;
  init_buffer(package);

  return package;
}

void add_to_package(Package *package, void *value, int size)
{
  package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + size + sizeof(int));

  memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
  memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, size);

  package->buffer->size += size + sizeof(int);
}

void *serialize_package(Package *package, int bytes)
{
  void *magic = malloc(bytes);
  int displacement = 0;

  memcpy(magic + displacement, &(package->operationCode), sizeof(int));
  displacement += sizeof(int);
  memcpy(magic + displacement, &(package->buffer->size), sizeof(int));
  displacement += sizeof(int);
  memcpy(magic + displacement, package->buffer->stream, package->buffer->size);

  return magic;
}

void send_package(Package *package, int clientSocket)
{
  int bytes = package->buffer->size + 2 * sizeof(int);

  void *toSend = serialize_package(package, bytes);

  send(clientSocket, toSend, bytes, 0);

  free(toSend);
}

void send_message_to_server(char *message, int clientSocket)
{
  Package *package = create_package(MESSAGE);

  package->buffer->size = strlen(message) + 1;
  package->buffer->stream = malloc(package->buffer->size);
  memcpy(package->buffer->stream, message, package->buffer->size);

  send_package(package, clientSocket);
  delete_package(package);
}

// ------- Server functions -------

OperationCode get_operation_code(int clientSocket)
{
  OperationCode operationCode;
  if (recv(clientSocket, &operationCode, sizeof(int), MSG_WAITALL) > 0)
    return operationCode;
  else
  {
    close(clientSocket);
    return DISCONNECTION;
  }
}

void *get_buffer(int *size, int clientSocket)
{

  void *buffer;
  recv(clientSocket, size, sizeof(int), MSG_WAITALL);
  buffer = malloc(*size);
  recv(clientSocket, buffer, *size, MSG_WAITALL);

  return buffer;
}

char *get_message_from_client(int clientSocket)
{
  int size;
  char *buffer = get_buffer(&size, clientSocket);

  return buffer;
}

List *get_package_as_list(int clientSocket)
{
  int bufferSize;
  int contentSize;
  int displacement = 0;
  List *content = list_create();
  void *buffer = get_buffer(&bufferSize, clientSocket);

  while (displacement < bufferSize)
  {
    memcpy(&contentSize, buffer + displacement, sizeof(int));
    displacement += sizeof(int);

    void *value = malloc(contentSize);
    memcpy(value, buffer + displacement, contentSize);
    displacement += contentSize;

    list_add(content, value);
  }

  free(buffer);
  return content;
}

void serialize_instructions_list(Package *package, List *instructionsList, int processSize)
{
  int instructionsAmount = list_size(instructionsList);

  add_to_package(package, &processSize, sizeof(int));
  add_to_package(package, &instructionsAmount, sizeof(int));

  InstructionLine *instructionLine;

  for (int i = 0; i < instructionsAmount; i++)
  {
    instructionLine = list_get(instructionsList, i);

    add_to_package(package, instructionLine->instructionName, strlen(instructionLine->instructionName) + 1);
    add_to_package(package, &(instructionLine->params[0]), sizeof(int));
    add_to_package(package, &(instructionLine->params[1]), sizeof(int));
  }

  delete_instruction_line(instructionLine);
}

void deserialize_instruction_lines(List *listToAdd, List *flattenList, int indexAmount, int indexList)
{
  int instructionsAmount = *(int *)list_get(flattenList, indexAmount);

  printf("Instruction Amount: %d\n\n", instructionsAmount);

  for (int i = 0; i < instructionsAmount; i++)
  {
    InstructionLine *instructionLine = malloc(sizeof(InstructionLine));
    short base = 3 * i;

    instructionLine->instructionName = list_get(flattenList, base + indexList + 1);
    instructionLine->params[0] = *(int *)list_get(flattenList, base + indexList + 2);
    instructionLine->params[1] = *(int *)list_get(flattenList, base + indexList + 3);

    list_add(listToAdd, instructionLine);
  }
}

void serialize_PCB(Package *package, Pcb *pcb)
{
  int instructionsAmount = list_size(pcb->instructions);

  add_to_package(package, &(pcb->pid), sizeof(int));
  add_to_package(package, &(pcb->size), sizeof(int));
  add_to_package(package, &(pcb->programCounter), sizeof(int));
  add_to_package(package, &(pcb->pageTable), sizeof(int));
  add_to_package(package, &(pcb->burstEstimation), sizeof(float));
  add_to_package(package, &(pcb->scene->state), sizeof(State));
  add_to_package(package, &(pcb->scene->ioBlockedTime), sizeof(int));
  add_to_package(package, &instructionsAmount, sizeof(int));
  serialize_instructions_list(package, pcb->instructions, pcb->size);
}

Pcb *deserialize_PCB(int clientSocket)
{
  List *flattenProperties = get_package_as_list(clientSocket);
  List *resultList = list_create();
  Pcb *pcb = malloc(sizeof(Pcb));

  pcb->pid = *(int *)list_get(flattenProperties, 0);
  pcb->size = *(int *)list_get(flattenProperties, 1);
  pcb->programCounter = *(int *)list_get(flattenProperties, 2);
  pcb->pageTable = *(int *)list_get(flattenProperties, 3);
  pcb->burstEstimation = *(float *)list_get(flattenProperties, 4);
  pcb->scene = malloc(sizeof(Scene));
  pcb->scene->state = *(State *)list_get(flattenProperties, 5);
  pcb->scene->ioBlockedTime = *(int *)list_get(flattenProperties, 6);
  deserialize_instruction_lines(resultList, flattenProperties, 7, 9);

  pcb->instructions = list_duplicate(resultList);

  list_destroy(flattenProperties);
  list_destroy(resultList);

  return pcb;
}