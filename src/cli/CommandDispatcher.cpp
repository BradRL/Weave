#include "cli/CommandDispatcher.h"

namespace cli
{
	bool cli::Dispatcher::dispatch(const cli::ParsedCommand& cmd) {
		auto handler = commandMap.find(cmd.command);  // itterates through map of handlers

		if (handler == commandMap.end()) {
			return false;  // command not found (return false to indicate issue)
		}

		handler->second->set(cmd);

		handler->second->execute(); // gets the value (handler class) from the found handler keyword and executes it
		
		return true;
	}
}