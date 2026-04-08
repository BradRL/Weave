#include "cli/commandHandlers/LogHandler.h"

namespace cli {

	void cli::LogHandler::execute()
	{
		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

		if (!utils::repoExistsFromInvocationPath(commandData.invocationPath))
		{
			utils::logError("[Log] ERROR | Could not find repository at '" + commandData.invocationPath.string() + "'");
			return;
		}

		lg::LogService logService = lg::LogService::LogService(commandData);

		logService.viewLog();
	}

	bool cli::LogHandler::validateCommand()
	{
		if (commandData.args.size() != 0)
		{
			utils::logError("[Log] ERROR | Unexpected argument(s) provided");
			return false;  // Invalid if no args provided
		}

		for (const auto& [key, value] : commandData.flags)
		{
			if (key == "-v" || key == "--verbose")
			{
				continue;
			}
			else
			{
				utils::logError("[Log] ERROR | Invalid flag '" + key + "' provided`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}

		return true;
	}
}