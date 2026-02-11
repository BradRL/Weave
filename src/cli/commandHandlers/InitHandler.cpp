#include "cli/commandHandlers/InitHandler.h"

namespace cli {

	void cli::InitHandler::execute() {
		std::cout << "`InitHandler` > METHOD-CALL `Execute()'" << std::endl;
		
		if (!validate()) {
			return;  // Exit if validation fails
		}

		std::filesystem::path repoPath;

		if (commandData.args.size() == 1) {
			try {
				repoPath = std::filesystem::canonical(commandData.args[0]);
			} catch (const std::filesystem::filesystem_error& e) {
				std::cout << "`InitHandler` > ERROR `Execute()' - Invalid path provided: " << e.what() << std::endl;
				return;  // Exit if the provided path is invalid
			}
		} 
		else 
		{
			repoPath = commandData.invocationPath;  // Default to invocation path
		}

		std::cout << "InitHandler` > DEBUG `Execute()' - Command pass." << std::endl;
	}

	bool cli::InitHandler::validate() {
		std::cout << "`InitHandler` > METHOD-CALL `Validate()'" << std::endl;

		if (commandData.args.size() > 1) {
			std::cout << "`InitHandler` > ERROR `Validate()' - More than 1 optional argument provided." << std::endl;
			return false;  // Invalid if more than one argument is provided
		}
		
		if (!commandData.flags.empty()) {
			std::cout << "`InitHandler` > ERROR `Validate()' - Invalid flags provided." << std::endl;
			return false;
		}

		return true;
	}

	void cli::InitHandler::set(const ParsedCommand& cmd) {
		std::cout << "`InitHandler` > METHOD-CALL `Set(ParsedCommand cmd)'" << std::endl;
		commandData = cmd;
	}
}