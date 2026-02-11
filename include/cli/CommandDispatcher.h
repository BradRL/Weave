#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "cli/data/ParsedCommand.h"
#include "commandHandlers/ICommandHandler.h"
#include "cli/data/CommandRegister.h"
#include "utils/Logger.h"

namespace cli {

	/// <summary>
	/// Command dispatcher responsible for creating command objects and calling relevent methods.
	/// </summary>
	class Dispatcher {
	private:
		std::unordered_map<std::string, ICommandHandler*> commandMap = cli::buildCommandMap();  // Map of command names to their respective handlers
	public:
		bool dispatch(const cli::ParsedCommand& cmd);  // finds and dispatches parsed command to the correct handler
	};

}