#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/commandServices/RollbackService.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include "core/utils/IOUtils.h"

namespace cli {

	class RollbackHandler : public ICommandHandler {

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