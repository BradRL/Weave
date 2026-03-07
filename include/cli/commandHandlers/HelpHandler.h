#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/commandServices/RemoveService.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace cli {

	class HelpHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Validates command arguments and flags.
		/// Command syntax: `weave help [command]`
		/// Args: [command] optional parameter, if provided shows detailed help for that command, otherwise shows general help with list of commands.
		/// Flags: None.
		/// </summary>
		bool validateCommand();
		
	};
}