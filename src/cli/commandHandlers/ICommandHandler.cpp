#include "cli/commandHandlers/ICommandHandler.h"

namespace cli {

	void cli::ICommandHandler::execute() {
		std::cout << "`CommandBase` > METHOD-CALL `Execute()`" << std::endl;
	};

	void cli::ICommandHandler::set(const ParsedCommand& cmd) {
		commandData = cmd;
	}
}