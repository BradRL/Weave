#include "cli/commandHandlers/ICommandHandler.h"

namespace cli {

	void cli::ICommandHandler::execute() {
		std::cout << "`CommandBase` > METHOD-CALL `Execute()`" << std::endl;
	};

	void cli::ICommandHandler::set(const cli::ParsedCommand& cmd) {
		std::cout << "`CommandBase` > METHOD-CALL `Set(ParsedCommand cmd)" << std::endl;
		commandData = cmd;
	};
}