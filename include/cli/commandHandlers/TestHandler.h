#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include <string>
#include <iostream>
#include <vector>

namespace cli {

	/// <summary>
	/// Test command handler for testing purposes.
	/// </summary>
	class TestHandler : public ICommandHandler {

	public:

		/// <summary>
		/// Executes command. Creates a Weave repository.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Sets command arguments.
		/// </summary>
		/// <param name="cmd">processed command args</param>
		void set(const ParsedCommand& cmd);
	};
}