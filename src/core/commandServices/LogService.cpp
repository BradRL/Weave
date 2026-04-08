#include "core/commandServices/LogService.h"

namespace lg {

	lg::LogService::LogService(const cli::ParsedCommand& cmd) : commandData(cmd) 
	{
		this->commitLogPath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "changelog.i";
	}

	void lg::LogService::viewLog() 
	{
		std::ifstream changelog(commitLogPath, std::ios::binary);

		if (!changelog.is_open()) {
			utils::logError("[Log] ERROR | Could not open commit-log at '" + commitLogPath.string() + "'");
			return;
		}

		models::changelogEntryDisk disk{};

		if (changelog.peek() == std::ifstream::traits_type::eof()) {
			std::cout << ("[Log] INFO | No commit history found") << std::endl;
			return;
		}

		std::cout << "[Log] INFO | Commit log:" << std::endl;

		int logCount = 1;

		while (changelog.read(reinterpret_cast<char*>(&disk), sizeof(models::changelogEntryDisk)))
		{
			std::cout << std::endl;

			// NodeID hash
			const char* hexChars = "0123456789abcdef";

			std::string thisCommitNodeID;
			thisCommitNodeID.reserve(32 * 2);

			for (uint8_t b : disk.commitNodeID)
			{
				thisCommitNodeID.push_back(hexChars[b >> 4]);
				thisCommitNodeID.push_back(hexChars[b & 0x0F]);
			}

			std::string thisParentNodeID;
			thisParentNodeID.reserve(32 * 2);

			for (uint8_t b : disk.parentNodeID)
			{
				thisParentNodeID.push_back(hexChars[b >> 4]);
				thisParentNodeID.push_back(hexChars[b & 0x0F]);
			}

			if (thisParentNodeID == ("0000000000000000000000000000000000000000000000000000000000000000")) {
				thisParentNodeID = "-1";
			}

			std::string thisTimestamp(disk.timestamp, strnlen(disk.timestamp, 20));

			std::string msg(reinterpret_cast<const char*>(disk.message.data()),
				strnlen(reinterpret_cast<const char*>(disk.message.data()),
					disk.message.size()));

			std::cout << "Commit [" << logCount << "] : "<< thisCommitNodeID << std::endl;
			std::cout << "  - Parent  : " << thisParentNodeID << std::endl;
			std::cout << "  - Created : " << thisTimestamp << std::endl;
			std::cout << "  - Message : \""<< msg << "\"" << std::endl;
			logCount++;
		}

		changelog.close();
	}
} 