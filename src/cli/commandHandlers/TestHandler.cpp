#include "cli/commandHandlers/TestHandler.h"

namespace cli {

	void cli::TestHandler::execute() {};

	void cli::TestHandler::set(const ParsedCommand& cmd) {
		commandData = cmd;
	}
}