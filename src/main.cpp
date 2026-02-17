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

	std::string displayValid = parsedCommand.valid ? "true" : "false";
	utils::log("<Debug> | main() > `Command parsed, displaying parsed command data...`");
	utils::log("<Debug> | ParsedCommand.valid > `" + displayValid + "'");
	utils::log("<Debug> | ParsedCommand.programPath > '"+ parsedCommand.programPath.string() + "'");
	utils::log("<Debug> | ParsedCommand.invocationPath > '" + parsedCommand.invocationPath.string() + "'");
	utils::log("<Debug> | ParsedCommand.command > '" + parsedCommand.command + "'");

	for (std::string s : parsedCommand.args) {
		utils::log("<Debug> | ParsedCommand.arg(s) > '" + s + "'");
	}
	for (const auto& [k, v] : parsedCommand.flags) {
		utils::log("<Debug> | ParsedCommand.flag(s) > '" + k + ", " + v + "'");
	}

	if (!parsedCommand.valid) {
		utils::logError("<Error> | main() > `Invalid command syntax, terminating program...`");
		utils::displayLogs();
		return 1;
	}

	utils::log("------- | --------------------");

	// Dispatch the command
	dispatcher.dispatch(parsedCommand);  // Assuming a dispatch method exists

	utils::displayLogs();  // Display log history (for debugging purposes)

	return 0;
}