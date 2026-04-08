#include "core/commandServices/StageService.h"

namespace stage 
{
	StageService::StageService(const cli::ParsedCommand& cmd) : commandData(cmd) {};

	void StageService::viewStage() 
	{
		this->stageFilePath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "stage";

		std::ifstream stageFile(stageFilePath, std::ios::binary);

		if (!stageFile.is_open()) {
			utils::logError("[Stage] ERROR | Could not open stage file at '" + stageFilePath.string() + "'");
			return;
		}

		models::stageEntyDisk disk{};

		if (stageFile.peek() == std::ifstream::traits_type::eof()) {
			std::cout << ("[Stage] INFO | No files currently staged") << std::endl;
			return;
		}

		std::cout << "[Stage] INFO | Staged Files:" << std::endl;

		while (stageFile.read(reinterpret_cast<char*>(&disk), sizeof(models::stageEntyDisk)))
		{
			std::string pathStr(reinterpret_cast<char*>(disk.path.data()));
			
			std::string flag = (disk.flags == 1) ? "ADD" : "-RM";  // Example: if flags == 1, mark as tracked

			std::cout << (flag + " - " + pathStr) << std::endl;
		}

		stageFile.close();
	}
}