#include <console_utils.h>
#include <commons/log.h>

int main(int argc, char *argv[])
{
  Config *config = config_create("Console.config");
  Logger *logger = log_create("Console.log", "Console", true, LOG_LEVEL_INFO);

  // if (argc < 3)
  // {
  //   log_error(logger, "Must pass two arguments.");
  //   return EXIT_FAILURE;
  // }

  fill_console_config(config);

  // char *filePath = argv[1];
  // int processSize = atoi(argv[2]);

  char *filePath = "./pseudocode.txt";
  int processSize = 100;

  log_info(logger, "Connecting to Kernel Server...");
  int consoleSocket = connect_to_server_kernel();

  if (consoleSocket < 0)
  {
    log_error(logger, "Connection refused. Server not initialized.");
    return EXIT_FAILURE;
  }

  log_info(logger, "Connection accepted. Starting client...");

  FILE *pseudocodeFile = fopen(filePath, "r");
  List *instructionsList = list_create();
  fill_instructions_list(instructionsList, pseudocodeFile);

  Package *package = create_package(CODE_LINES);
  serialize_instructions_list(package, instructionsList, processSize);
  send_package(package, consoleSocket);
  log_info(logger, "Sending Instruction Lines...");

  log_warning(logger, "Exiting Server.");
  terminate_console(consoleSocket, config, logger, instructionsList, pseudocodeFile, package);

  return EXIT_SUCCESS;
}