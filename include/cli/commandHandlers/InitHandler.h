#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace cli {

	class InitHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Validates command arguments and flags.
		/// 
		/// Command syntax: `weave init <repository name>`
		/// Args: <repository name> required parameter, sets the repo's name for easy identification. directory path is assumed to be same as invocation path.
		/// Flags: None.
		/// </summary>
		bool validate();

		/// <summary>
		/// Sets command arguments.
		/// </summary>
		/// <param name="cmd">processed command args</param>
		void set(const ParsedCommand& cmd);
	};
}