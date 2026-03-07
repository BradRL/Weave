#pragma once

// Include command handler headers here
#include "cli/commandHandlers/TestHandler.h"
#include "cli/commandHandlers/InitHandler.h"
#include "cli/commandHandlers/AddHandler.h"
#include "cli/commandHandlers/RemoveHandler.h"
#include "cli/commandHandlers/HelpHandler.h"

namespace cli {

	inline std::unordered_map<std::string, ICommandHandler*> buildCommandMap() {
		static std::unordered_map<std::string, ICommandHandler*> commandMap;
		commandMap["test"] = new TestHandler();
		commandMap["init"] = new InitHandler();
		commandMap["add"] = new AddHandler();
		commandMap["remove"] = new RemoveHandler();
		commandMap["rm"] = commandMap["remove"];  // alias for remove command
		commandMap["help"] = new HelpHandler();
		return commandMap;
	}
}