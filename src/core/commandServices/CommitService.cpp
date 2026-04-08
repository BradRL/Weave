#include "core/commandServices/CommitService.h"

namespace commit {

	commit::CommitService::CommitService(const cli::ParsedCommand& cmd) : commandData(cmd) {};

	void commit::CommitService::commit() {

		this->stageFilePath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "stage";
		this->manifestFilePathIndex = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "manifest.i";
		this->manifestFilePathData = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "manifest.d";
		this->commitLogPath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "changelog.i";

		if (isStageEmpty()) {
			utils::logError("[Commit] Error | No changes staged for commit");
			return;
		}

		readStageItems();
		readPreviousManifest();
		generateManifest();

		appendManifestEntry();
		clearStage();
	}

	bool commit::CommitService::isStageEmpty() 
	{
		std::ifstream stageFile(stageFilePath, std::ios::binary);
		return stageFile.peek() == std::ifstream::traits_type::eof();
	}

	void commit::CommitService::readStageItems() 
	{
		std::ifstream stageFile(stageFilePath, std::ios::binary);

		models::stageEntyDisk disk{};

		while (stageFile.read(reinterpret_cast<char*>(&disk), sizeof(models::stageEntyDisk)))
		{
			stageEntries.push_back(disk);
		}

		stageFile.close();
	}

	void commit::CommitService::readPreviousManifest()
	{
		std::ifstream manifestFileIndex(manifestFilePathIndex, std::ios::binary);

		models::manifestIndexEntryDisk mDisk{};

		manifestFileIndex.seekg(0, std::ios::end);
		std::streamsize fileSize = manifestFileIndex.tellg();

		if (fileSize == 0) {
			utils::log("[Commit] INFO | This is your first commit.");
		}

		manifestFileIndex.seekg(fileSize - sizeof(models::manifestIndexEntryDisk));  // Move to the last manifest entry
		manifestFileIndex.read(reinterpret_cast<char*>(&mDisk), sizeof(models::manifestIndexEntryDisk));

		manifestFileIndex.close();

		std::ifstream manifestFileData(manifestFilePathData, std::ios::binary);

		for (uint64_t offset = mDisk.dataOffset; offset < mDisk.dataOffset + mDisk.dataLength; offset += sizeof(models::manifestDataEntryDisk))
		{
			manifestFileData.seekg(offset);
			models::manifestDataEntryDisk dDisk{};
			manifestFileData.read(reinterpret_cast<char*>(&dDisk), sizeof(models::manifestDataEntryDisk));
			manifestEntries.push_back(dDisk);
		}

		manifestFileData.close();
	}

	void commit::CommitService::generateManifest() 
	{ 
		std::unordered_map<std::string, size_t> lookupMap;
		lookupMap.reserve(manifestEntries.size());

		for (size_t i = 0; i < manifestEntries.size(); i++)
		{
			const char* pathData = reinterpret_cast<const char*>(manifestEntries[i].path.data());
			std::string resolvedPath(
				pathData, strnlen(pathData, manifestEntries[i].path.size())
			);

			lookupMap[resolvedPath] = i;
		}

		this->changed = false;

		for (const auto& stageEntry : stageEntries)
		{
			const char* pathData = reinterpret_cast<const char*>(stageEntry.path.data());
			std::string resolvedPath(
				pathData, strnlen(pathData, stageEntry.path.size())
			);

			auto it = lookupMap.find(resolvedPath);

			if (stageEntry.flags == 0) {
				if (it == lookupMap.end()) {
					// file is marked for removal however does not exist in previous manifest, so we can ignore it and not add to new manifest
					utils::log("[Commit] INFO | File '" + resolvedPath + "' marked for removal but does not exist in previous manifest, skipping.");
				}
				else 
				{
					// file is marked for removal and exists in previous manifest, so we should not add to new manifest
					manifestEntries[it->second].path.fill(0);  // clear path data to indicate removal
					utils::log("[Commit] INFO | File '" + resolvedPath + "' removed from repository tracking.");
					changed = true;
				}
			} 
			else 
			{
				if (it == lookupMap.end()) {
					// add file, new file not in previous manifest, so add to new manifest

					models::manifestDataEntryDisk newEntry{};

					newEntry.nodeID = stageEntry.nodeID;
					std::memcpy(
						newEntry.path.data(),
						resolvedPath.c_str(),
						std::min(resolvedPath.size(), newEntry.path.size() - 1)
					);

					manifestEntries.push_back(newEntry);

					utils::log("[Commit] INFO | File '" + resolvedPath + "' added to tracking.");
					changed = true;
				}
				else 
				{
					if (manifestEntries[it->second].nodeID == stageEntry.nodeID)  // compare hash values, if same hash -> skip. else update manifest with new hash. 
					{
						utils::log("[Commit] INFO | File '" + resolvedPath + "' has no changes, skipping.");
					}
					else 
					{
						manifestEntries[it->second].nodeID = stageEntry.nodeID;  // update manifest with new hash
						utils::log("[Commit] INFO | File '" + resolvedPath + "' updated.");
						changed = true;
					}
				}
			}
		}
	}

	bool commit::CommitService::appendManifestEntry()
	{
		if (!changed) {
			utils::log("[Commit] INFO | No changes to commit after comparing staged items to previous manifest, skipping manifest update.");
			return false;
		}

		std::ofstream manifestFileData(manifestFilePathData, std::ios::binary | std::ios::app);
		std::ofstream manifestFileIndex(manifestFilePathIndex, std::ios::binary | std::ios::app);

		uint64_t dataOffset = manifestFileData.tellp();
		uint32_t dataLength = manifestEntries.size() * sizeof(models::manifestDataEntryDisk);

		for (const auto& entry : manifestEntries)
		{
			manifestFileData.write(reinterpret_cast<const char*>(&entry), sizeof(models::manifestDataEntryDisk));
		}

		models::manifestIndexEntryDisk indexEntry{};

		indexEntry.dataOffset = dataOffset;
		indexEntry.dataLength = dataLength;
		indexEntry.nodeID = computeManifestHash();

		manifestFileIndex.write(reinterpret_cast<const char*>(&indexEntry), sizeof(models::manifestIndexEntryDisk));

		manifestFileData.close();
		manifestFileIndex.close();

		appendChangelogEntry(indexEntry);

		return true;
	}

	void commit::CommitService::clearStage() 
	{
		std::ofstream stageFile(stageFilePath, std::ios::binary | std::ios::trunc);
		stageFile.close();
	}

	std::array<uint8_t, 32> commit::CommitService::computeManifestHash() const {
		std::vector<uint8_t> buffer;

		for (const auto& entry : manifestEntries)
		{
			buffer.insert(buffer.end(), entry.path.begin(), entry.path.end());
			buffer.insert(buffer.end(), entry.nodeID.begin(), entry.nodeID.end());
		}

		std::array<uint8_t, 32> hash{};

		picosha2::hash256(
			buffer.begin(),
			buffer.end(),
			hash.begin(),
			hash.end()
		);

		return hash;
	}

	void commit::CommitService::appendChangelogEntry(const models::manifestIndexEntryDisk& indexEntry)
	{
		models::changelogEntryDisk logEntry{};

		logEntry.commitNodeID = indexEntry.nodeID;

		std::ifstream commitLog(commitLogPath, std::ios::binary);

		commitLog.seekg(0, std::ios::end);
		std::streamsize size = commitLog.tellg();

		if (size < static_cast<std::streamsize>(sizeof(models::changelogEntryDisk)))
		{
			logEntry.parentNodeID.fill(0);
		}
		else 
		{
			commitLog.seekg(size - sizeof(models::changelogEntryDisk), std::ios::beg);

			models::changelogEntryDisk entry{};
			commitLog.read(reinterpret_cast<char*>(&entry), sizeof(entry));
			logEntry.parentNodeID = entry.commitNodeID;
		}

		logEntry.dataOffset = indexEntry.dataOffset;
		logEntry.dataLength = indexEntry.dataLength;

		for (const auto& [key, value] : commandData.flags)
		{
			if (key == "-m" || key == "--message")
			{
				std::memset(logEntry.message.data(), 0, logEntry.message.size());

				std::memcpy(
					logEntry.message.data(),
					value.c_str(),
					std::min(value.size(), logEntry.message.size() - 1)
				);
			}
		}
	
		std::time_t now = std::time(nullptr);
		std::tm tm = *std::localtime(&now);

		char timeStr[20];
		std::strftime(logEntry.timestamp, sizeof(timeStr), "%Y-%m-%d %H:%M", &tm);

		std::ofstream changelogFile(commitLogPath, std::ios::binary | std::ios::app);
		changelogFile.write(reinterpret_cast<const char*>(&logEntry), sizeof(models::changelogEntryDisk));
		changelogFile.close();
	}
}