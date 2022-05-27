#include <console_utils.h>

char *read_line(FILE *file)
{
  char *line = string_new();
  int bufferSize = 0;

  getline(&line, (size_t *restrict)&bufferSize, file);
  delete_line_feed(line);

  return line;
}

int connect_to_server_kernel(void)
{
  return create_server_connection(CONSOLE_CONFIG.IP_KERNEL, CONSOLE_CONFIG.PORT_KERNEL);
}

void delete_line_feed(char *text)
{
  if (text[string_length(text) - 1] == '\n')
    text[string_length(text) - 1] = '\0';
}

int times(char *string, char character)
{
  int times = 0;
  for (short i = 0; i < string_length(string); i++)
    if (string[i] == character)
      times++;

  return times;
}

int tokens_amount(char *string)
{
  return times(string, ' ') + 1;
}

void fill_instruction_params(char **tokens, short tokensAmount)
{
  if (tokensAmount < 2)
    tokens[1] = "-1";
  if (tokensAmount < 3)
    tokens[2] = "-1";
}

char **get_tokens(char *string)
{
  short tokensAmount = tokens_amount(string);
  char **tokens = string_n_split(string, tokensAmount, " ");
  fill_instruction_params(tokens, tokensAmount);

  return tokens;
}

void assign_tokens_to_instruction_line(InstructionLine *instructionLine, char **tokens)
{
  instructionLine->instructionName = strdup(tokens[0]);
  instructionLine->params[0] = atoi(tokens[1]);
  instructionLine->params[1] = atoi(tokens[2]);

  free(*tokens);
}

void add_instruction_line_to_list(InstructionLine *instructionLine, List *list)
{
  if (!strcmp(instructionLine->instructionName, "NO_OP"))
  {
    for (short i = 0; i < instructionLine->params[0]; i++)
      list_add(list, instructionLine);

    instructionLine->params[0] = -1;
  }
  else
    list_add(list, instructionLine);
}

InstructionLine *get_instruction_line(FILE *file)
{
  char *line = read_line(file);
  char **tokens = get_tokens(line);

  // TODO: Search how to free this malloc to solve memcheck failures.
  InstructionLine *instructionLine = malloc(sizeof(InstructionLine));
  assign_tokens_to_instruction_line(instructionLine, tokens);

  free(line);

  return instructionLine;
}

void fill_instructions_list(List *list, FILE *file)
{
  while (!feof(file))
  {
    InstructionLine *instructionLine = get_instruction_line(file);
    add_instruction_line_to_list(instructionLine, list);
  }
}

void terminate_console(int socket, Config *config, Logger *logger, List *list, FILE *file, Package *package)
{
  terminate_program(socket, config, logger);
  delete_package(package);
  list_destroy(list);
  txt_close_file(file);
}