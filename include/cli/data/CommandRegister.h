#pragma once

// Include command handler headers here
#include "cli/commandHandlers/TestHandler.h"
#include "cli/commandHandlers/InitHandler.h"
#include "cli/commandHandlers/AddHandler.h"
#include "cli/commandHandlers/RemoveHandler.h"
#include "cli/commandHandlers/HelpHandler.h"
#include "cli/commandHandlers/CommitHandler.h"
#include "cli/commandHandlers/StageHandler.h"
#include "cli/CommandHandlers/LogHandler.h"

namespace cli {

	inline std::unordered_map<std::string, ICommandHandler*> buildCommandMap() {
		static std::unordered_map<std::string, ICommandHandler*> commandMap;
		commandMap["test"] = new TestHandler();
		commandMap["init"] = new InitHandler();
		commandMap["add"] = new AddHandler();
		commandMap["remove"] = new RemoveHandler();
		commandMap["rm"] = commandMap["remove"];  // alias for remove command
		commandMap["help"] = new HelpHandler();
		commandMap["commit"] = new CommitHandler();
		commandMap["stage"] = new StageHandler(); 
		commandMap["log"] = new LogHandler();
		return commandMap;
	}
}