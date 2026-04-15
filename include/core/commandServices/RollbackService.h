#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "core/utils/revlogUtils.h"
#include "cli/data/ParsedCommand.h"
#include "core/models/WeaveStructures.h"
#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <filesystem>

namespace rollb {

	class RollbackService {

	private:
		cli::ParsedCommand commandData;
		std::filesystem::path storePath;
		std::filesystem::path dataPath;
		std::filesystem::path stagePath;
		std::filesystem::path configPath;

	private:
		void resolveRevlogFiles();

	public:
		RollbackService(const cli::ParsedCommand& cmd);

		void rollback();
	};
}