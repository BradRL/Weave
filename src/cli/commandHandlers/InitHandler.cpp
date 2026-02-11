#include "cli/commandHandlers/InitHandler.h"

namespace cli {

	void cli::InitHandler::execute() {

		if (!validate()) {
			return;  // Exit if validation fails
		}

		std::filesystem::path repoPath = commandData.invocationPath;  // Default repository path is the invocation path
		std::string repoName = commandData.args[0];  // Repository name from the first argument

		utils::log("<debug> | InitHandler::execute() > `Command pass`");
	}

	bool cli::InitHandler::validate() {

		if (commandData.args.size() == 0) {
			utils::log("<Error> | InitHandler::validate() > `<Repository Name> argument is required.`");
			return false;  // Invalid if no args provided
		}
		else if (commandData.args.size() > 1) {
			utils::log("<Error> | InitHandler::validate() > `Too many arguments provided.`");
			return false;  // Invalid if more than one argument is provided
		}
		
		for (const auto& [key, value] : commandData.flags) {
			if (key == "-v" || key == "--verbose")
			{
				continue;  // Skip validation for verbose flag 
			}  
			else
			{
				utils::log("<Error> | InitHandler::validate() > `Invalid flag '" + key + "' provided.`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}

		return true;
	}

	void cli::InitHandler::set(const ParsedCommand& cmd) {
		commandData = cmd;
	}
}