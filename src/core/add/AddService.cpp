#include "core/add/AddService.h"

namespace add {

	add::AddService::AddService(const cli::ParsedCommand& cmd) : commandData(cmd) {};

	bool add::AddService::addFile(const std::string& file) 
	{
		this->fileName = file;

		if (!validateAddFile()) {
			return false;
		}

		createStageData();

		if (!isFileStaged(stageEntry.nodeID)) 
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
		stageEntry.path = fileName;

		// byte file length
		fileContents.seekg(0, std::ios::end);
		stageEntry.dataLength = fileContents.tellg();
		fileContents.seekg(0, std::ios::beg);

		// .d byte offset + revlog file check

		if (std::filesystem::exists(dataFilePath) && std::filesystem::is_regular_file(dataFilePath))
		{
			std::ifstream dataFileIn(dataFilePath, std::ios::binary);
			dataFileIn.seekg(0, std::ios::end);
			stageEntry.dataOffset = dataFileIn.tellg();
			dataFileIn.close();
		}
		else 
		{
			stageEntry.dataOffset = 0;
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
		std::copy(hash.begin(), hash.end(), stageEntry.nodeID.begin());

		std::string hex1 = picosha2::bytes_to_hex_string(hash.begin(), hash.end());

		/////

		const char* hexChars = "0123456789abcdef";

		std::string hex;
		hex.reserve(32 * 2);

		for (uint8_t b : stageEntry.nodeID)
		{
			hex.push_back(hexChars[b >> 4]);
			hex.push_back(hexChars[b & 0x0F]);
		}

		// Flags TODO
		stageEntry.flags = 0;

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

		std::vector<char> buffer(static_cast<size_t>(stageEntry.dataLength));

		fileContents.seekg(0, std::ios::beg);
		fileContents.read(buffer.data(), stageEntry.dataLength);
		dataFile.write(buffer.data(), stageEntry.dataLength);

		dataFile.flush();

		dataFile.close();
		fileContents.close();

		// .i
		stageDataDisk disk{};

		std::memcpy(
			disk.path.data(),
			stageEntry.path.c_str(),
			std::min(stageEntry.path.size(), disk.path.size() - 1)
		);

		disk.dataOffset = stageEntry.dataOffset;
		disk.dataLength = stageEntry.dataLength;
		disk.nodeID = stageEntry.nodeID;
		disk.flags = stageEntry.flags;

		std::ofstream indexFile(indexFilePath, std::ios::binary | std::ios::app);

		indexFile.write(reinterpret_cast<const char*> (&disk), sizeof(stageDataDisk));

		indexFile.close();
	}

	bool add::AddService::isFileStaged(const std::array<uint8_t, 32>& nodeID)
	{
		std::ifstream stageFile(stageFilePath, std::ios::binary);

		stageEntyDisk disk{};

		while (stageFile.read(reinterpret_cast<char*>(&disk), sizeof(stageEntyDisk))) 
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

		stageEntyDisk disk{};

		std::memcpy(
			disk.path.data(),
			stageEntry.path.c_str(),
			std::min(stageEntry.path.size(), disk.path.size() - 1)
		);
		disk.nodeID = stageEntry.nodeID;

		stageFile.write(reinterpret_cast<const char*> (&disk), sizeof(stageEntyDisk));

		stageFile.close();
	}
}