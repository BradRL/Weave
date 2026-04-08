#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "core/utils/picosha2.h"
#include "cli/data/ParsedCommand.h"
#include "core/models/WeaveStructures.h"
#include <string>
#include <cstring>
#include <array>
#include <iostream>
#include <vector>
#include <filesystem>

namespace commit {

	class CommitService {

	private:
		cli::ParsedCommand commandData;
		std::filesystem::path stageFilePath;
		std::filesystem::path manifestFilePathIndex;
		std::filesystem::path manifestFilePathData;
		std::filesystem::path commitLogPath;
		std::vector<models::manifestDataEntryDisk> manifestEntries;
		std::vector<models::stageEntyDisk> stageEntries;
		bool changed;

	private:
		/// <summary>
		/// Checks if the repository stage is empty (no file changes staged).
		/// </summary>
		/// <returns>true if stage is empty</returns>
		bool isStageEmpty();

		/// <summary>
		/// Reads the repository stage file and parses into a vector of `stageEntryDisk` data structures.
		/// </summary>
		void readStageItems();

		/// <summary>
		/// Reads the most recent entry in the manifest (if exists) and parses into a vector of `manifestDataEntryDisk` data structures.
		/// </summary>
		void readPreviousManifest();

		/// <summary>
		/// Generates the new commit manifest based on the previous manifest and staged items.
		/// </summary>
		void generateManifest();

		/// <summary>
		/// Appends generated manifest data to the manifest file. Also appends necessary metadata to the manifest index file for future retrieval.
		/// </summary>
		/// <returns>false if no manifest items to append</returns>
		bool appendManifestEntry();

		void appendChangelogEntry(const models::manifestIndexEntryDisk& indexEntry);

		std::array<uint8_t, 32> computeManifestHash() const;

		void clearStage();

	public:
		CommitService(const cli::ParsedCommand& cmd);

		void commit();
	};
} 