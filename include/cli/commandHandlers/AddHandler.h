#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/add/AddService.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace cli {

	class AddHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Validates command arguments and flags.
		/// 
		/// Command syntax: `weave add <file> [file(s)]`
		/// Args: <file> mandatory parameter, adds this file. [file(s)] optional parameters for additional files. files can include paths
		/// Flags: None.
		/// </summary>
		bool validateCommand();

		/// <summary>
		/// Sets command arguments.
		/// </summary>
		/// <param name="cmd">processed command args</param>
		void set(const ParsedCommand& cmd);
	};
}