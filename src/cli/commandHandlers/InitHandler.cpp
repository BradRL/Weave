#include "cli/commandHandlers/InitHandler.h"

namespace cli {

	void cli::InitHandler::execute() 
	{

		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

		init::InitService initService = init::InitService::InitService(commandData);
		
		initService.initializeRepository();

		utils::log("[Init] INFO | Initialized repository '" + commandData.args[0] + "'");
	}

	bool cli::InitHandler::validateCommand() 
	{

		if (commandData.args.size() == 0) 
		{
			utils::logError("[Init] ERROR | Required argument '<Repository Name>' not provided");
			return false;  // Invalid if no args provided
		}
		else if (commandData.args.size() > 1) 
		{
			utils::logError("[Init] ERROR | Too many arguments provided");
			return false;  // Invalid if more than one argument is provided
		}
		
		for (const auto& [key, value] : commandData.flags) 
		{
			if (key == "-v" || key == "--verbose")
			{
				continue;  
			}  
			else if (key == "-a" || key == "--author") 
			{
				continue;
			}
			else
			{
				utils::logError("[Init] ERROR | Invalid flag '" + key + "' provided");
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