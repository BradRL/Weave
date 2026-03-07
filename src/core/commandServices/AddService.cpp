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

		if (!isFileStaged(fileIndexEntry.nodeID))
		{
			appendRevision();
			updateStage();
			utils::log("[Add] INFO | Added File '" + filePath.string() + "' to staging");
		}
		else
		{
			utils::logError("[Add] ERROR | File '" + filePath.string() + "' already staged");
		}

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
		fileIndexEntry.path = fileName;

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

		std::memcpy(
			disk.path.data(),
			fileIndexEntry.path.c_str(),
			std::min(fileIndexEntry.path.size(), disk.path.size() - 1)
		);

		disk.dataOffset = fileIndexEntry.dataOffset;
		disk.dataLength = fileIndexEntry.dataLength;
		disk.nodeID = fileIndexEntry.nodeID;
		disk.flags = fileIndexEntry.flags;

		std::ofstream indexFile(indexFilePath, std::ios::binary | std::ios::app);

		indexFile.write(reinterpret_cast<const char*> (&disk), sizeof(models::FileIndexEntryDisk));

		indexFile.close();
	}

	bool add::AddService::isFileStaged(const std::array<uint8_t, 32>& nodeID)
	{
		std::ifstream stageFile(stageFilePath, std::ios::binary);

		models::stageEntyDisk disk{};

		while (stageFile.read(reinterpret_cast<char*>(&disk), sizeof(models::stageEntyDisk)))
		{
			if (disk.nodeID == nodeID)
			{
				return true;
			}
		}

		stageFile.close();

		return false;
	}

	void add::AddService::updateStage() {
		std::ofstream stageFile(stageFilePath, std::ios::binary | std::ios::app);

		models::stageEntyDisk disk{};

		std::memcpy(
			disk.path.data(),
			fileIndexEntry.path.c_str(),
			std::min(fileIndexEntry.path.size(), disk.path.size() - 1)
		);
		disk.nodeID = fileIndexEntry.nodeID;

		stageFile.write(reinterpret_cast<const char*> (&disk), sizeof(models::stageEntyDisk));

		stageFile.close();
	}
}