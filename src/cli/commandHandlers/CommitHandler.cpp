#include "cli/commandHandlers/CommitHandler.h"

namespace cli {

	void cli::CommitHandler::execute() 
	{
		if (!validateCommand()) {
			return;
		}

		if (!utils::repoExistsFromInvocationPath(commandData.invocationPath))
		{
			utils::logError("[Commit] ERROR | Could not find repository at '" + commandData.invocationPath.string() + "'");
			return;
		}

		commit::CommitService commitService = commit::CommitService::CommitService(commandData);

		commitService.commit();
	}

	bool cli::CommitHandler::validateCommand() 
	{
		if (commandData.args.size() != 0)
		{
			utils::logError("[Commit] ERROR | Unknown args provided");
			return false;  // Invalid if no args provided
		}

		for (const auto& [key, value] : commandData.flags)
		{
			if (key == "-v" || key == "--verbose")
			{
				continue;
			}
			else if (key == "-m" || key == "--message")
			{
				continue;
			}
			else
			{
				utils::logError("[Commit] ERROR | Invalid flag '" + key + "' provided`");
				return false;  // Invalid if any flags other than verbose are provided
			}
		}
		return true;
	}
}