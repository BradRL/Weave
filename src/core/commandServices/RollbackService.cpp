#include "core/commandServices/RollbackService.h"

namespace rollb
{
	RollbackService::RollbackService(const cli::ParsedCommand& cmd) : commandData(cmd) {};

	void rollb::RollbackService::rollback() 
	{
		resolveRevlogFiles();

		std::filesystem::path changelogIPath = storePath / "changelog.i";
		std::filesystem::path changelogDPath = storePath / "changelog.d";
		std::filesystem::path manifestIPath = storePath / "manifest.i";
		std::filesystem::path manifestDPath = storePath / "manifest.d";

		std::ifstream commitLogFile(changelogIPath, std::ios::binary);

		commitLogFile.seekg(0, std::ios::end);
		uint32_t fileSize = static_cast<uint32_t>(commitLogFile.tellg());
		uint32_t commitCount = fileSize / (sizeof(models::changelogEntryDisk));
		
		uint32_t targetCommitNumber = commitCount;
		if (!commandData.args.empty()) {
			targetCommitNumber = static_cast<uint32_t>(std::stoul(commandData.args.front()));
			if (targetCommitNumber > commitCount || targetCommitNumber < 1)
			{
				utils::logError("[Rollback] ERROR | Commit index '" + std::to_string(targetCommitNumber) + "' was not found");
				return;
			}
		}

		models::changelogEntryDisk changelogEntry;

		uint32_t targetCommitPointer = (targetCommitNumber - 1) * sizeof(models::changelogEntryDisk);
		commitLogFile.seekg(targetCommitPointer, std::ios::beg);
		commitLogFile.read(reinterpret_cast<char*>(&changelogEntry), sizeof(models::changelogEntryDisk));
		commitLogFile.close();

		std::ifstream manifestIFile(manifestIPath, std::ios::binary);
		models::manifestIndexEntryDisk manifestEntry;

		while (manifestIFile.read(reinterpret_cast<char*>(&manifestEntry), sizeof(manifestEntry))) 
		{
			if (std::equal(manifestEntry.nodeID.begin(), manifestEntry.nodeID.end(), changelogEntry.commitNodeID.begin())) 
			{
				break;
			}
		}
		manifestIFile.close();

		std::ifstream manifestDFile(manifestDPath, std::ios::binary);
		manifestDFile.seekg(manifestEntry.dataOffset, std::ios::beg);
		std::vector<char> buffer(manifestEntry.dataLength);
		manifestDFile.read(buffer.data(), manifestEntry.dataLength);
		manifestDFile.close();

		size_t p1 = 0;
		size_t s = sizeof(models::manifestDataEntryDisk);

		while (p1 + s <= buffer.size()) 
		{
			models::manifestDataEntryDisk entry{};

			std::memcpy(&entry, buffer.data() + p1, s);
			std::string filePath(reinterpret_cast<const char*>(entry.path.data()));

			std::filesystem::path fileIPath = dataPath / (filePath + ".i");
			std::filesystem::path fileDPath = dataPath / (filePath + ".d");

			std::ifstream fileIndexFile(fileIPath, std::ios::binary);
			models::FileIndexEntryDisk fileEntry{};
			utils::repoConfig config = utils::readConfig(configPath);

			while (fileIndexFile.read(reinterpret_cast<char*>(&fileEntry), sizeof(fileEntry)))
			{
				if (std::equal(fileEntry.nodeID.begin(), fileEntry.nodeID.end(), entry.nodeID.begin())) 
				{
					std::string r = revlogUtils::reconstructRevision(filePath, dataPath, fileEntry.nodeID);
					std::filesystem::path targetFilePath = config.root / filePath;

					std::ofstream targetFile(targetFilePath, std::ios::binary | std::ios::trunc);
					targetFile.write(r.data(), r.size());
					targetFile.close();

					std::string msg = "[Rollback] INFO | Restored file '" + filePath + "' to revision in commit " + std::to_string(targetCommitNumber);
					utils::log(msg);

					break;
				}
			}
			p1 += s;

			fileIndexFile.close();
		}

		std::ofstream stageFile(stagePath, std::ios::binary | std::ios::trunc);
		stageFile.close();
		utils::log("[Rollback] INFO | Cleared staging area");
	}

	void rollb::RollbackService::resolveRevlogFiles() 
	{
		std::filesystem::path base = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave";
		this->storePath = base / "store";
		this->dataPath = storePath / "data";
		this->stagePath = base / "stage";
		this->configPath = base / "config";
	}
}