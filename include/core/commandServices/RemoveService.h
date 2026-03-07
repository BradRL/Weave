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

namespace rm {

	class RemoveService {

	private:
		cli::ParsedCommand commandData;
		std::string fileName;  // use as global accessor instead of passing down
		std::filesystem::path stageFilePath;

	private:
		bool updateStage();

	public:
		RemoveService(const cli::ParsedCommand& cmd);
		bool removeFile(const std::string& file);
	};
}