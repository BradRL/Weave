#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace cli {
struct ParsedCommand {	

	std::filesystem::path invocationPath;  // Path `Weave.exe` was called from.
	std::filesystem::path programPath;     // Absolute path to `Weave.exe` on disk.

	/// <summary>
	/// Command name to be executed (e.g. "commit", "init").
	/// </summary>
	std::string command;

	/// <summary>
	/// Vector of command arguments (e.g: `file1.py`, "dir/file2.java`).
	/// </summary>
	std::vector<std::string> args;

	/// <summary>
	/// Unordered map of any flags with values (e.g: `-m` -> `commit msg`)
	/// </summary>
	std::unordered_map<std::string, std::string> flags;

	/// <summary>
	/// Reflects validity of the commands, indicates any syntax errors encounted.
	/// </summary>
	bool valid = false;
};

}