#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>

namespace utils {

	struct repoConfig {
		std::string name;
		std::filesystem::path root;
		std::time_t createdAt;
		std::string author;
	};

	/// <summary>
	/// Returns true if 2 file paths are the same or subdirectories
	/// </summary>
	/// <param name="dirA">File path 1</param>
	/// <param name="dirB">File path 2</param>
	/// <returns>is paths are same or subdirectories</returns>
	bool isSameOrSubdir(const std::filesystem::path& pathA, const std::filesystem::path& pathB);

	/// <summary>
	/// Reads data from a repo config file and returns it as `repoConfig` data structure.
	/// </summary>
	/// <param name="configFilePath">path to config file</param>
	/// <returns>populated repoconfig struct</returns>
	repoConfig readConfig(const std::filesystem::path& configPath);

	/// <summary>
	/// Returns file path to local %APPDATA% where repo files are located
	/// </summary>
	/// <returns></returns>
	std::filesystem::path getWeaveRoot();
}