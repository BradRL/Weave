#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "core/models/RevlogDataStructures.h"
#include "cli/data/ParsedCommand.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

namespace revlog {

	class RepoInitializer {
		
	private:
		cli::ParsedCommand commandData;

	private:
		/// <summary>
		/// Validates that repository with same name or directory/subdirectory doesnt exist.
		/// </summary>
		/// <returns>True is valid repo</returns>
		bool validateInitializeRepository();

		/// <summary>
		/// Creates Weave repository store layout
		/// </summary>
		/// <param name="root"></param>
		void createRepoLayout(const std::filesystem::path& root);

		/// <summary>
		/// Creates and populates config file for a new repo
		/// </summary>
		/// <param name="root"></param>
		void createConfigFile(const std::filesystem::path& root);

		/// <summary>
		/// Creates changelog file for the repository and an initial (empty) commit.
		/// </summary>
		/// <param name="root"></param>
		void createChangelogFiles(const std::filesystem::path& root);

		/// <summary>
		/// Creates manifest file for the repository will empty first commit data.
		/// </summary>
		/// <param name="root"></param>
		void createManifestFiles(const std::filesystem::path& root);

	public:
		RepoInitializer(const cli::ParsedCommand& cmd);

		/// <summary>
		/// Initializes a new `Weave` repository, point of access.
		/// </summary>
		void initializeRepository();
	};
}