#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "cli/data/ParsedCommand.h"
#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <filesystem>

#include "core/utils/picosha2.h"

namespace add {

	class AddService {

		struct stageData
		{
			std::string path;
			std::array<uint8_t, 32> nodeID;
			uint64_t dataOffset;
			uint32_t dataLength;
			uint16_t flags;  // fulltext, delta, compression
		};

		#pragma pack(push, 1)
		struct stageDataDisk 
		{
			std::array<unsigned char, 256> path{};
			std::array<uint8_t, 32> nodeID{};
			uint64_t dataOffset{};
			uint32_t dataLength{};
			uint16_t flags{};  // fulltext, delta, compression

			// 302 bytes total
			// 256 + 32 + 8 + 4 + 2 (removes +2 alignment bytes for arrays) using pack to ensure no padding
		};
		#pragma pack(pop)

		#pragma pack(push, 1)
		struct stageEntyDisk
		{
			std::array<unsigned char, 256> path{};
			std::array<uint8_t, 32> nodeID{};
		};
		#pragma pack(pop)

	private:

		cli::ParsedCommand commandData;
		std::string fileName;  // use as global accessor instead of passing down
		std::filesystem::path filePath;
		std::filesystem::path indexFilePath;
		std::filesystem::path dataFilePath;
		std::filesystem::path stageFilePath;

		stageData stageEntry;

	private:
		/// <summary>
		/// Checks if a file exists at the given path
		/// </summary>
		/// <param name="filePath">file path to check</param>
		/// <returns>if file exists</returns>
		bool validateAddFile();

		/// <summary>
		/// parses data from the file into stage data
		/// </summary>
		void createStageData();

		/// <summary>
		/// Sets file paths for .d, .i and stage files, creates them if they dont exist. also sets data offset for stage data.
		/// Sets file path as global variable for easier access in other functions.
		/// </summary>
		void resolveRevlogFiles();

		/// <summary>
		/// Creates new revision in revlog files based on stage data, appends to the end of the files.
		/// </summary>
		void appendRevision();

		/// <summary>
		/// Checks if a file with the same content (nodeID) is already staged, to prevent duplicates in staging and revlog files.
		/// </summary>
		/// <param name="nodeID">current hash nodeID</param>
		/// <returns>is that ID is present in staging</returns>
		bool isFileStaged(const std::array<uint8_t, 32>& nodeID);

		/// <summary>
		/// Adds staged file data to staging file, which is used as a temporary storage for files to be commited, until commit is made and stage is cleared. also adds file to revlog files.
		/// </summary>
		void updateStage();

	public:
		AddService(const cli::ParsedCommand& cmd);

		/// <summary>
		/// Adds given file to index staging
		/// </summary>
		/// <param name="file">File name / path</param>
		/// <returns>if file was found</returns>
		bool addFile(const std::string& file);
	};
}