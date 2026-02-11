#pragma once

// Include command handler headers here
#include "cli/commandHandlers/TestHandler.h"
#include "cli/commandHandlers/InitHandler.h"

namespace cli {

	inline std::unordered_map<std::string, ICommandHandler*> buildCommandMap() {
		static std::unordered_map<std::string, ICommandHandler*> commandMap;
		commandMap["test"] = new TestHandler();
		commandMap["init"] = new InitHandler();
		return commandMap;
	}
}