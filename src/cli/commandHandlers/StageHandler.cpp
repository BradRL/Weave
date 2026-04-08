#include "cli/commandHandlers/stageHandler.h"

namespace cli {

	void cli::StageHandler::execute() 
	{
		if (!validateCommand()) {
			return;
		}

		if (!utils::repoExistsFromInvocationPath(commandData.invocationPath))
		{
			utils::logError("[Stage] ERROR | Could not find repository at '" + commandData.invocationPath.string() + "'");
			return;
		}

		stage::StageService stageService = stage::StageService::StageService(commandData);

		stageService.viewStage();
	}

	bool cli::StageHandler::validateCommand() 
	{
		if (commandData.args.size() != 0)
		{
			utils::logError("[Stage] ERROR | Unexpected argument(s) provided");
			return false;  // Invalid if any args provided
		}

		for (const auto& [key, value] : commandData.flags)
		{
			utils::logError("[Stage] ERROR | Unexpected flag '" + key + "' provided`");
			return false;  // Invalid if any flags provided
		}
		
		return true;
	}
}