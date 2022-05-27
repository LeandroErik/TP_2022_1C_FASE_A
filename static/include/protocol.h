#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <socket/socket.h>
#include <commons/collections/list.h>
#include <semaphore.h>

typedef t_config Config;
typedef t_list List;
typedef t_log Logger;
typedef pthread_t Thread;
typedef sem_t Semaphore;

typedef enum OperationCode
{
  DISCONNECTION = -1,
  MESSAGE,
  CODE_LINES,
  PCB,
  INTERRUPT
} OperationCode;

typedef enum
{
  EXECUTING,
  INPUT_OUTPUT,
  EXTERNAL_INTERRUPTION,
  EXIT
} State;
typedef struct Scene
{
  State state;
  int ioBlockedTime;
} Scene;

typedef struct Pcb
{
  unsigned int pid;
  unsigned int size;
  unsigned int programCounter;
  unsigned int pageTable;
  float burstEstimation;
  Scene *scene;
  List *instructions;
} Pcb;

typedef struct InstructionLine
{
  char *instructionName;
  int params[2];
} InstructionLine;

typedef struct Buffer
{
  int size;
  void *stream;
} Buffer;

typedef struct Package
{
  OperationCode operationCode;
  Buffer *buffer;
} Package;

// ------- Client & Server functions -------

/**
 * @brief Deletes a package and its buffer.
 *
 * @param package Package to delete.
 */
void delete_package(Package *package);

/**
 * @brief Frees an instruction line.
 *
 * @param instructionLine Instruction line.
 */
void delete_instruction_line(InstructionLine *instructionLine);

/**
 * @brief Deletes a PCB.
 *
 * @param pcb PCB.
 */
void delete_PCB(Pcb *pcb);

// ------- Client functions -------

/**
 * @brief Initializes a buffer in a package.
 *
 * @param package Package.
 */
void init_buffer(Package *package);

/**
 * @brief Creates a package object passing an operation code.
 *
 * @param operationCode Operation code.
 * @return Package.
 */
Package *create_package(OperationCode operationCode);

/**
 * @brief Adds element to a package.
 *
 * @param package Package.
 * @param value Value to add.
 * @param size Size of value.
 */
void add_to_package(Package *package, void *value, int size);

/**
 * @brief Serializes a package.
 *
 * @param package Package to serialize.
 * @param bytes Bytes to allocate to serialized package.
 * @return Serialized package.
 */
void *serialize_package(Package *package, int bytes);

/**
 * @brief Sends a package to the server connected.
 *
 * @param package Package to be sent.
 * @param clientSocket Socket of client.
 */
void send_package(Package *package, int clientSocket);

/**
 * @brief Sends a message to the server connected.
 *
 * @param message Message to be sent.
 * @param clientSocket Socket of client.
 */
void send_message_to_server(char *message, int clientSocket);

// ------- Server functions -------

/**
 * @brief Gets the received package operation code.
 *
 * @param clientSocket Socket of client.
 * @return Operation code.
 */
OperationCode get_operation_code(int clientSocket);

/**
 * @brief Gets the buffer object sent by client.
 *
 * @param size Buffer size.
 * @param clientSocket Socket of client.
 * @return Buffer.
 */
void *get_buffer(int *size, int clientSocket);

/**
 * @brief Gets the received package message.
 *
 * @param clientSocket Socket of client.
 * @return Message.
 */
char *get_message_from_client(int clientSocket);

/**
 * @brief Gets the package as a list.
 *
 * @param clientSocket Socket of client.
 * @return Package content as a list.
 */
List *get_package_as_list(int clientSocket);

/**
 * @brief Serializes the instructions list to be sent.
 *
 * @param package Package to fill.
 * @param instructionsList Instructions list.
 * @param processSize Process size.
 */
void serialize_instructions_list(Package *package, List *instructionsList, int processSize);

/**
 * @brief Deserializes instruction lines given a list to add those instructions, a flatten list
 * with the properties of each instruction line and an index to start.
 *
 * @param listToAdd List to fill.
 * @param flattenList List to read.
 * @param indexAmount Index where allocates size of list.
 * @param indexList Index where starts instructions flatten list.
 */
void deserialize_instruction_lines(List *listToAdd, List *flattenList, int indexAmount, int indexList);

/**
 * @brief Serializes the PCB to be sent.
 *
 * @param package Package to fill.
 * @param pcb PCB.
 */
void serialize_PCB(Package *package, Pcb *pcb);

/**
 * @brief Deserializes a PCB given by a client.
 *
 * @param clientSocket Socket of client.
 *
 * @return PCB.
 */
Pcb *deserialize_PCB(int clientSocket);

#endif