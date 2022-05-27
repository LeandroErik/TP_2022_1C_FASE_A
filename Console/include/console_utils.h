#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <socket/client.h>
#include <console_config.h>
#include <commons/string.h>
#include <commons/txt.h>

/**
 * @brief Reads a line from a pseudocode.
 *
 * @param file File object pointing to pseudocode file.
 * @return Line read.
 */
char *read_line(FILE *file);

/**
 * @brief Creates a connection with Server Kernel.
 *
 * @return Socket of client.
 */
int connect_to_server_kernel(void);

/**
 * @brief Deletes a line feed of a line.
 *
 * @param text Line.
 */
void delete_line_feed(char *text);

/**
 * @brief Says how many times a character repeats in a string.
 *
 * @param string String to check.
 * @param character Character to look.
 * @return Times appeared.
 */
int times(char *string, char character);

/**
 * @brief Says how many tokens are in a pseudocode line.
 *
 * @param string Pseudocode line.
 * @return Tokens amount.
 */
int tokens_amount(char *string);

/**
 * @brief Fills instruction params in case there aren't tokens enough. (They must be 3)
 *
 * @param tokens Tokens to fill.
 * @param tokensAmount Tokens amount.
 */
void fill_instruction_params(char **tokens, short tokensAmount);

/**
 * @brief Get the tokens from a pseudocode line. (Always 3)
 *
 * @param string Pseudocode line.
 * @return Tokens.
 */
char **get_tokens(char *string);

/**
 * @brief Assigns tokens to an instruction line object.
 *
 * @param instructionLine Instruction line object.
 * @param tokens Tokens.
 */
void assign_tokens_to_instruction_line(InstructionLine *instructionLine, char **tokens);

/**
 * @brief Adds an instruction line to a list.\n
 *        If the instruction is a NO_OP instruction, sets its params[0] to -1
 *        and adds instruction line n times (n = params[0]).
 *
 *        ```c
 *        Example: NO_OP 3.
 *
 *        InstructionLine* noOp  -> Instruction Name = NO_OP
 *                               -> Params[0] = -1
 *                               -> Params[1] = -1
 *
 *        List: [noOp, noOp, noOp]
 *        ```
 *
 * @param instructionLine Instruction line to be added.
 * @param list List.
 */
void add_instruction_line_to_list(InstructionLine *instructionLine, List *list);

/**
 * @brief Returns an instruction line read from file.
 *
 * @param file File to read.
 * @return Instruction line.
 */
InstructionLine *get_instruction_line(FILE *file);

/**
 * @brief Fills an instructions list reading a pseudocode.
 *
 * @param list Instructions list.
 * @param file Pseudocode file object.
 */
void fill_instructions_list(List *list, FILE *file);

/**
 * @brief Terminates a Console module.
 *
 * @param socket Socket.
 * @param config Config object.
 * @param logger Logger object.
 * @param list List.
 * @param file File object.
 * @param package Package object.
 */
void terminate_console(int socket, Config *config, Logger *logger, List *list, FILE *file, Package *package);

#endif