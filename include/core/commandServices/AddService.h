#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "cli/data/ParsedCommand.h"
#include "core/models/WeaveStructures.h"
#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <variant>
#include <filesystem>

#include "core/utils/picosha2.h"
#include "core/utils/revlogUtils.h"

namespace add {

	class AddService {

	private:

		cli::ParsedCommand commandData;
		std::string fileName;  // use as global accessor instead of passing down
		std::filesystem::path filePath;
		std::filesystem::path indexFilePath;
		std::filesystem::path dataFilePath;
		std::filesystem::path stageFilePath;
		std::filesystem::path revlogPath;

		revlogUtils::DeltaRevision deltaRevision;
		models::FileIndexEntryDisk fileIndexEntry;

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

		void appendRevision2(const std::string& file);

		/// <summary>
		/// Adds staged file data to staging file, which is used as a temporary storage for files to be commited, until commit is made and stage is cleared. also adds file to revlog files.
		/// </summary>
		void updateStage();

		bool hasRevision() const;

		void tryUpdateStage();

	public:
		AddService(const cli::ParsedCommand& cmd);

		/// <summary>
		/// Adds given file to index staging
		/// </summary>
		/// <param name="file">File name / path</param>
		/// <returns>if file was found</returns>
		bool addFile(const std::string& file);

		/// <summary>
		/// Updated file index staging, uses revlog deltas
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		bool addRevision(const std::string& file);
	};
}	