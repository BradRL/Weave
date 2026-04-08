#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/commandServices/LogService.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace cli {

	class LogHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Validates command arguments and flags.
		/// 
		/// Command syntax: `weave log `
		/// Args: none
		/// Flags: None.
		/// </summary>
		bool validateCommand();
	};
}