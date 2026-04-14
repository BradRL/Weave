#include "core/commandServices/AddService.h"

namespace add {

	add::AddService::AddService(const cli::ParsedCommand& cmd) : commandData(cmd) {};

	bool add::AddService::addFile(const std::string& file)
	{
		this->fileName = file;

		if (!validateAddFile()) {
			return false;
		}

		createStageData();

		if (!hasRevision()) {
			appendRevision();
			utils::log("[Add] INFO | Added File '" + filePath.string() + "' to staging");
		}
		tryUpdateStage();

		return true;
	}

	bool add::AddService::validateAddFile()
	{
		this->filePath = commandData.invocationPath / fileName;

		if (!(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))) {
			utils::logError("[Add] ERROR | Could not find file '" + filePath.string() + "'");
			return false;
		}

		return true;
	}

	void add::AddService::createStageData()
	{
		std::ifstream fileContents(filePath, std::ios::binary);

		resolveRevlogFiles();

		// repo relative path
		std::memset(fileIndexEntry.path.data(), 0, fileIndexEntry.path.size());

		std::memcpy(
			fileIndexEntry.path.data(),
			fileName.data(),
			std::min(fileName.size(), fileIndexEntry.path.size() - 1)
		);

		// byte file length
		fileContents.seekg(0, std::ios::end);
		fileIndexEntry.dataLength = fileContents.tellg();
		fileContents.seekg(0, std::ios::beg);

		// .d byte offset + revlog file check

		if (std::filesystem::exists(dataFilePath) && std::filesystem::is_regular_file(dataFilePath))
		{
			std::ifstream dataFileIn(dataFilePath, std::ios::binary);
			dataFileIn.seekg(0, std::ios::end);
			fileIndexEntry.dataOffset = dataFileIn.tellg();
			dataFileIn.close();
		}
		else
		{
			fileIndexEntry.dataOffset = 0;
			std::ofstream dataFileOut(dataFilePath);
			dataFileOut.close();
		}

		if (!(std::filesystem::exists(indexFilePath) && std::filesystem::is_regular_file(indexFilePath)))
		{
			std::ofstream indexFileOut(indexFilePath);
			indexFileOut.close();
		}

		// Hash generation
		std::vector<unsigned char> hash(picosha2::k_digest_size);
		picosha2::hash256(fileContents, hash.begin(), hash.end());
		std::copy(hash.begin(), hash.end(), fileIndexEntry.nodeID.begin());

		std::string hex1 = picosha2::bytes_to_hex_string(hash.begin(), hash.end());

		/////

		const char* hexChars = "0123456789abcdef";

		std::string hex;
		hex.reserve(32 * 2);

		for (uint8_t b : fileIndexEntry.nodeID)
		{
			hex.push_back(hexChars[b >> 4]);
			hex.push_back(hexChars[b & 0x0F]);
		}

		// Flags TODO
		fileIndexEntry.flags = 0;

		fileContents.close();
	}

	void add::AddService::resolveRevlogFiles()
	{
		std::filesystem::path revlogPath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "store" / "data";
		this->dataFilePath = revlogPath / (fileName + ".d");
		this->indexFilePath = revlogPath / (fileName + ".i");
		this->revlogPath = revlogPath;
		this->stageFilePath = utils::getWeaveRoot() / utils::repoNameFromInvocationPath(commandData.invocationPath) / ".weave" / "stage";

		std::filesystem::create_directories(dataFilePath.parent_path());
		std::filesystem::create_directories(indexFilePath.parent_path());
	}

	void add::AddService::appendRevision()
	{
		std::ifstream fileContents(filePath, std::ios::binary);

		// .d
		std::ofstream dataFile(dataFilePath, std::ios::binary | std::ios::app);

		std::vector<char> buffer(static_cast<size_t>(fileIndexEntry.dataLength));

		fileContents.seekg(0, std::ios::beg);
		fileContents.read(buffer.data(), fileIndexEntry.dataLength);
		dataFile.write(buffer.data(), fileIndexEntry.dataLength);

		dataFile.flush();

		dataFile.close();
		fileContents.close();

		// .i
		models::FileIndexEntryDisk disk{};

		/*std::memcpy(
			disk.path.data(),
			fileIndexEntry.path.c_str(),
			std::min(fileIndexEntry.path.size(), disk.path.size() - 1)
		);*/
		
		disk.path = fileIndexEntry.path;
		disk.dataOffset = fileIndexEntry.dataOffset;
		disk.dataLength = fileIndexEntry.dataLength;
		disk.nodeID = fileIndexEntry.nodeID;
		disk.flags = fileIndexEntry.flags;

		std::ofstream indexFile(indexFilePath, std::ios::binary | std::ios::app);

		indexFile.write(reinterpret_cast<const char*> (&disk), sizeof(models::FileIndexEntryDisk));

		indexFile.close();
	}

	void add::AddService::updateStage() {
		std::ofstream stageFile(stageFilePath, std::ios::binary | std::ios::app);

		models::stageEntyDisk disk{};

		/*std::memcpy(
			disk.path.data(),
			fileIndexEntry.path.c_str(),
			std::min(fileIndexEntry.path.size(), disk.path.size() - 1)
		);*/

		disk.path = fileIndexEntry.path;
		disk.nodeID = fileIndexEntry.nodeID;
		disk.flags = 1; // 1 is add, 0 is delete

		stageFile.write(reinterpret_cast<const char*> (&disk), sizeof(models::stageEntyDisk));

		stageFile.close();
	}

	bool add::AddService::hasRevision() const {
		std::ifstream targetFile(indexFilePath, std::ios::binary);

		if (!targetFile.is_open())
			return false;

		models::FileIndexEntryDisk disk{};

		while (targetFile.read(reinterpret_cast<char*>(&disk), sizeof(models::FileIndexEntryDisk)))
		{
			if (disk.nodeID == fileIndexEntry.nodeID)
			{
				targetFile.close();
				return true;
			}
		}

		return false;
	}

	void add::AddService::tryUpdateStage() {
		bool rewrite = false;

		std::ifstream stageFile(stageFilePath, std::ios::binary);

		models::stageEntyDisk disk{};
		std::vector<models::stageEntyDisk> stageEntries;

		while (stageFile.read(reinterpret_cast<char*>(&disk), sizeof(models::stageEntyDisk)))
		{
			std::string pathStr(reinterpret_cast<char*>(disk.path.data()));

			if (pathStr == fileName)
			{
				if (disk.flags == 0) {
					rewrite = true;
					utils::log("[Add] INFO | File '" + filePath.string() + "' was staged for removal, now re-adding");
					continue;
				}

				if (disk.nodeID == fileIndexEntry.nodeID) 
				{
					utils::logError("[Add] ERROR | File '" + filePath.string() + "' already staged");
					return;
				}

				rewrite = true;
				continue;  // Skip writing this entry to the new stage file, effectively removing it
			}
			else
			{
				stageEntries.push_back(disk);  // Keep this entry for the new stage file
			}
		}

		// only rewrite stage file if we found the file to remove, otherwise keep it as is
		if (rewrite) 
		{
			std::ofstream stageFileOut(stageFilePath, std::ios::binary | std::ios::trunc);  // Open in truncate mode to overwrite existing file

			for (const auto& entry : stageEntries) 
			{
				stageFileOut.write(reinterpret_cast<const char*>(&entry), sizeof(models::stageEntyDisk));
			}
		}
	
		updateStage();
	}

	bool add::AddService::addRevision(const std::string& file)
	{
		this->fileName = file;

		if (!validateAddFile()) 
		{
			return false;
		}
		createStageData();

		if (!hasRevision()) 
		{
			this->deltaRevision = revlogUtils::generateDeltas(file, revlogPath, fileIndexEntry.nodeID);
			appendRevision2(file);
			utils::log("[Add] INFO | Added File '" + filePath.string() + "' to staging");
		}
		tryUpdateStage();
	}

	void add::AddService::appendRevision2(const std::string& file)
	{
		fileIndexEntry.baseRevision = revlogUtils::getRevisionCount(file, revlogPath);
		if (deltaRevision.useSnapshot) 
		{
			fileIndexEntry.flags = 1;
			std::ofstream dataFile(dataFilePath, std::ios::app | std::ios::binary);

			std::string snapshotData = revlogUtils::readFile(filePath);
			fileIndexEntry.dataLength = snapshotData.size();
			dataFile.write(snapshotData.data(), snapshotData.size());
			dataFile.close();
		}
		else 
		{
			fileIndexEntry.flags = 0;
			std::ofstream dataFile(dataFilePath, std::ios::app | std::ios::binary);

			for (const auto& op : deltaRevision.ops)
			{
				if (std::holds_alternative<revlogUtils::EqualOp>(op))
				{
					revlogUtils::writeEqualOp(dataFile, std::get<revlogUtils::EqualOp>(op));
				}
				else if (std::holds_alternative<revlogUtils::InsertOp>(op))
				{
					revlogUtils::writeInsertOp(dataFile, std::get<revlogUtils::InsertOp>(op));
				}
			}

			auto end = dataFile.tellp();
			fileIndexEntry.dataLength = static_cast<uint32_t>(static_cast<uint64_t>(end) - fileIndexEntry.dataOffset);
			dataFile.close();
		}

		std::ofstream indexFile(indexFilePath, std::ios::binary | std::ios::app);

		indexFile.write(reinterpret_cast<const char*> (&fileIndexEntry), sizeof(models::FileIndexEntryDisk));

		indexFile.close();
	}
}