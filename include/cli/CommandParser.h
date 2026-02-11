#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include "ParsedCommand.h"
#include "cli/data/FlagRegister.h"

namespace cli {
 	
	/// <summary>
	/// Parses raw command input into a structured representation consisting of a
	/// command identifier, a list of arguemnts, and flags.
	/// </summary>
	class Parser {

	private:
		/// <summary>
		/// Vector to hold unprocessed command-line tokens.
		/// </summary>
		std::vector<std::string> tokens;

		/// <summary>
		/// Command-line tokens sorted by type (commandtype, args, flags).
		/// </summary>
		cli::ParsedCommand parsedCommand;  

		std::unordered_map<std::string, cli::FlagInfo> shortFlagMap = cli::buildShortFlagMap();  // Map of short flags to their info
		std::unordered_map<std::string, cli::FlagInfo> longFlagMap = cli::buildLongFlagMap();    // Map of long flags to their info

	public:
		/// <summary>
		/// Constructor, sets unprocessed token values.
		/// </summary>
		/// <param name="tokens :">unprocessed list of tokens from cli</param>
		Parser(const std::vector<std::string>& tokens);

		/// <summary>
		/// Parses the command-line tokens by type and returns a sorted `ParsedCommand` object.
		/// </summary>
		/// <returns>sorted `ParsedCommand` struct</returns>
		cli::ParsedCommand parse();

		/// <summary>
		/// Getter for sorted `ParsedCommand` struct
		/// </summary>
		/// <returns>sorted `ParsedCommand` struct</returns>
		cli::ParsedCommand getParsedCommand() const;
	};

}