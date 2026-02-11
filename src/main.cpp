#include "cli/CommandParser.h"
#include "cli/CommandDispatcher.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {

	std::cout << "Starting CLI application..." << std::endl;

	std::filesystem::current_path();

	// cli tokens to vector
	std::vector<std::string> tokens(argv, argv + argc);

	// Initialize parser and dispatcher
	cli::Parser parser(tokens);

	cli::Dispatcher dispatcher;

	// attempts to parse given command from CLI
	cli::ParsedCommand parsedCommand = parser.parse();

	std::cout << "'CommandParser' > DEBUG program path : " << parsedCommand.programPath << std::endl;
	std::cout << "`CommandParser` > DEBUG invocation path : " << parsedCommand.invocationPath << std::endl;
	std::cout << "`CommandParser` > DEBUG command type : " << parsedCommand.command << std::endl;
	for (std::string s : parsedCommand.args) {
		std::cout << "`CommandParser` > DEBUG arg(s) : " << s << std::endl;
	}
	for (const auto& [k, v] : parsedCommand.flags) {
		std::cout << "`CommandParser` > DEBUG flag(s) : " << k << " -> " << v << std::endl;
	}
	std::cout << "`CommandParser` > DEBUG valid : " << parsedCommand.valid << std::endl;

	if (!parsedCommand.valid) {
		std::cout << "Error : Unable to parse command. Please check your syntax and try again." << std::endl << "Terminating program..." << std::endl;
		return 1;
	}

	// Dispatch the command
	dispatcher.dispatch(parsedCommand);  // Assuming a dispatch method exists
	return 0;
}