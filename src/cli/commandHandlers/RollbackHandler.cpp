#include "cli/commandHandlers/RollbackHandler.h"

namespace cli {

	void cli::RollbackHandler::execute()
	{
		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

		if (!utils::repoExistsFromInvocationPath(commandData.invocationPath))
		{
			utils::logError("[Rollback] ERROR | Could not find repository at '" + commandData.invocationPath.string() + "'");
			return;
		}

		rollb::RollbackService rollbackService = rollb::RollbackService::RollbackService(commandData);
		rollbackService.rollback();
	}

	bool cli::RollbackHandler::validateCommand()
	{
		if (commandData.args.size() > 1)
		{
			utils::logError("[Rollback] ERROR | Unexpected argument(s) provided");
			return false;  // Invalid if no args provided
		}

		for (const auto& [key, value] : commandData.flags)
		{
			if (key == "-v" || key == "--verbose")
			{
				continue;
			}
			else if (key == "-x" || key == "--x") 
			{
				continue;
			}
			else
			{
				utils::logError("[Rollback] ERROR | Invalid flag '" + key + "' provided`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}

		return true;
	}
}