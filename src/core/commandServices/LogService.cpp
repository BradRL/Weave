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

		bool verbose = commandData.flags.count("-v") || commandData.flags.count("--verbose");

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

			if (!verbose)
			{
				logCount++;
				continue;
			}

			std::cout << "  - Tracked :" << std::endl;

			std::ifstream manifestIFile(utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "manifest.i", std::ios::binary);
			models::manifestIndexEntryDisk manifestEntry{};

			while (manifestIFile.read(reinterpret_cast<char*>(&manifestEntry), sizeof(manifestEntry)))
			{
				if (std::equal(manifestEntry.nodeID.begin(), manifestEntry.nodeID.end(), disk.commitNodeID.begin()))
				{
					break;
				}
			}
			manifestIFile.close();

			std::ifstream manifestDFile(utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "manifest.d", std::ios::binary);
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

				std::string fileHash;
				fileHash.reserve(32 * 2);

				for (uint8_t b : entry.nodeID)
				{
					fileHash.push_back(hexChars[b >> 4]);
					fileHash.push_back(hexChars[b & 0x0F]);
				}

				std::cout << "    - " << filePath << " (" << fileHash << ")" << std::endl;
				p1 += s;
			}

			logCount++;
		}

		changelog.close();
	}
} 