#pragma once

#include "utils/Logger.h"
#include "core/utils/picosha2.h"
#include "core/utils/IOUtils.h"
#include "cli/data/ParsedCommand.h"
#include "core/models/WeaveStructures.h"
#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <filesystem>

namespace lg {

	class LogService {

	private:
		cli::ParsedCommand commandData;
		std::filesystem::path commitLogPath;

	public:
		LogService(const cli::ParsedCommand& cmd);

		void viewLog();
	};
}