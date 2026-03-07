#include "cli/commandHandlers/RemoveHandler.h"

namespace cli {

	void cli::RemoveHandler::execute() 
	{
		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

		if (!utils::repoExistsFromInvocationPath(commandData.invocationPath))
		{
			utils::logError("[Remove] ERROR | Could not find repository at '" + commandData.invocationPath.string() + "'");
			return;
		}

		rm::RemoveService removeService = rm::RemoveService::RemoveService(commandData);

		for (const std::string arg : commandData.args)
		{
			removeService.removeFile(arg);
		}
	}
	bool cli::RemoveHandler::validateCommand()
	{
		if (commandData.args.size() == 0)
		{
			utils::logError("[Remove] ERROR | Required argument '<path/to/file>' not provided");
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
				utils::logError("[Remove] ERROR | Invalid flag '" + key + "' provided`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}
		return true;
	}
}