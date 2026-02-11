#include "cli/commandHandlers/TestHandler.h"

namespace cli {

	void cli::TestHandler::execute() {
		std::cout << "`TestHandler` > METHOD-CALL `Execute()'" << std::endl;
	};

	void cli::TestHandler::set(const ParsedCommand& cmd) {
		std::cout << "`TestHandler` > METHOD-CALL `Set(ParsedCommand cmd)'" << std::endl;
		commandData = cmd;
	}
}