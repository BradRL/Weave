#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "cli/data/ParsedCommand.h"
#include "core/models/WeaveStructures.h"
#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <filesystem>

namespace stage {

	class StageService {

	private:
		cli::ParsedCommand commandData;
		std::filesystem::path stageFilePath;

	public:
		StageService(const cli::ParsedCommand& cmd);

		void viewStage();
	};
}