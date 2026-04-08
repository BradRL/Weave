#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/commandServices/CommitService.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace cli {

	class CommitHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Validates command arguments and flags.
		/// 
		/// Command syntax: `weave commit -m message`
		/// </summary>
		bool validateCommand();
	};
}