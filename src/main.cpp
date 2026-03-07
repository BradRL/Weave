#include "cli/CommandParser.h"
#include "cli/CommandDispatcher.h"
#include "utils/Logger.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {

	std::filesystem::current_path();

	// cli tokens to vector
	std::vector<std::string> tokens(argv, argv + argc);

	// Initialize parser and dispatcher
	cli::Parser parser(tokens);

	cli::Dispatcher dispatcher;

	// attempts to parse given command from CLI
	cli::ParsedCommand parsedCommand = parser.parse();

	if (!parsedCommand.valid) {
		utils::logError("[Weave] ERROR | Invalid command syntax");
		utils::displayLogs();
		return 1;
	}

	// Dispatch the command
	dispatcher.dispatch(parsedCommand);  // Assuming a dispatch method exists

	utils::displayLogs();  // Display log history (for debugging purposes)

	return 0;
}