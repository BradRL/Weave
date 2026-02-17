#include "cli/commandHandlers/InitHandler.h"

namespace cli {

	void cli::InitHandler::execute() 
	{

		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

		utils::log("<Debug> | InitHandler::execute() > `Command pass`");

		revlog::RepoInitializer repoInitializer = revlog::RepoInitializer::RepoInitializer(commandData);
		
		repoInitializer.initializeRepository();

		std::cout << ("[Weave] INFO | Init: Repository initialized.") << std::endl;
	}

	bool cli::InitHandler::validateCommand() 
	{

		if (commandData.args.size() == 0) {
			utils::logError("<Error> | InitHandler::validateCommand() > `<Repository Name> argument is required.`");
			return false;  // Invalid if no args provided
		}
		else if (commandData.args.size() > 1) {
			utils::logError("<Error> | InitHandler::validateCommand() > `Too many arguments provided.`");
			return false;  // Invalid if more than one argument is provided
		}
		
		for (const auto& [key, value] : commandData.flags) {
			if (key == "-v" || key == "--verbose")
			{
				continue;  // Skip validation for verbose flag 
			}  
			else if (key == "-a" || key == "--author") 
			{
				continue;
			}
			else
			{
				utils::logError("<Error> | InitHandler::validateCommand() > `Invalid flag '" + key + "' provided.`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}

		return true;
	}

	void cli::InitHandler::set(const ParsedCommand& cmd) 
	{
		commandData = cmd;
	}
}