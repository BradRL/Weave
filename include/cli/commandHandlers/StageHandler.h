#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/commandServices/StageService.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace cli {

	class StageHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Validates command arguments and flags.
		/// Command syntax: `weave stage`
		/// Args: None.
		/// Flags: None.
		/// </summary>
		bool validateCommand();
	};
}