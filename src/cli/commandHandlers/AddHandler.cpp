#include "cli/commandHandlers/AddHandler.h"

namespace cli {

	void cli::AddHandler::execute() 
	{
		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

		if (!utils::repoExistsFromInvocationPath(commandData.invocationPath))
		{
			utils::logError("[Add] ERROR | Could not find repository at '" + commandData.invocationPath.string() + "'");
			return;
		}

		add::AddService addService = add::AddService::AddService(commandData);

		for (const std::string arg : commandData.args) 
		{
			addService.addFile(arg);
		}
	}

	bool cli::AddHandler::validateCommand() 
	{ 
		if (commandData.args.size() == 0) 
		{
			utils::logError("[Add] ERROR | Required argument '<path/to/file>' not provided");
			return false;  // Invalid if no args provided
		}

		for (const auto& [key, value] : commandData.flags) 
		{
			if (key == "-v" || key == "--verbose") 
			{
				continue; 
			}
			else if (key == "-r" || key == "--recursive") 
			{
				continue;
			}
			else 
			{
				utils::logError("[Add] ERROR | Invalid flag '" + key + "' provided`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}

		return true; 
	}
}