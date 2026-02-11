#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
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
		/// Command syntax: `weave init [path/to/dir]`
		/// Args: [path/to/dir] - optional argument specifying directory to initialize repository in (defaults to invocation path if not provided).
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