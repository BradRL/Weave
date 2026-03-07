#include "core/commandServices/RemoveService.h"

namespace rm {

	rm::RemoveService::RemoveService(const cli::ParsedCommand& cmd) : commandData(cmd) {};

	bool rm::RemoveService::removeFile(const std::string& file)
	{
		this->fileName = file;
		this->stageFilePath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "stage";

		if (updateStage())
		{
			utils::log("[Remove] INFO | Removed File '" + fileName + "' from staging");
		}
		else 
		{
			utils::logError("[Remove] ERROR | File '" + fileName + "' not found in staging");
		}

		return true;
	}

	bool rm::RemoveService::updateStage()
	{
		bool found = false;

		std::ifstream stageFile(stageFilePath, std::ios::binary);

		models::stageEntyDisk disk{};
		std::vector<models::stageEntyDisk> stageEntries;

		while (stageFile.read(reinterpret_cast<char*>(&disk), sizeof(models::stageEntyDisk)))
		{
			std::string pathStr(reinterpret_cast<char*>(disk.path.data()));

			if (pathStr == fileName)
			{
				found = true;
				continue;  // Skip writing this entry to the new stage file, effectively removing it
			}
			else 
			{
				stageEntries.push_back(disk);  // Keep this entry for the new stage file
			}
		}

		// only rewrite stage file if we found the file to remove, otherwise keep it as is
		if (found) {
			std::ofstream stageFileOut(stageFilePath, std::ios::binary | std::ios::trunc);  // Open in truncate mode to overwrite existing file

			for (const auto& entry : stageEntries) {
				stageFileOut.write(reinterpret_cast<const char*>(&entry), sizeof(models::stageEntyDisk));
			}
		}

		return found;
	}
}