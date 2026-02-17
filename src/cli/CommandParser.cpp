#include "cli/CommandParser.h"

namespace cli
{
	cli::Parser::Parser(const std::vector<std::string>& tk) : tokens(tk) {}

	cli::ParsedCommand cli::Parser::parse() {
		cli::ParsedCommand result;  // new ParsedCommand object to write to
		
		result.invocationPath = std::filesystem::current_path();  // Absolute path to the directory `Weave.exe` was called from.
		result.programPath = std::filesystem::canonical(tokens[0]);  // Absolute path to `Weave.exe` on disk.

		if (tokens.size() == 1) {
			utils::logError("<Error> | Parser::parse() > 'No command provided'");
			result.command = "N/A";
			return result;  // Return an invalid ParsedCommand
		} 
		else {
			result.command = tokens[1];
		}

		for (size_t i = 2; i < tokens.size(); ++i) 
		{
			std::string token = tokens[i];

			if (longFlagMap.contains(token)) 
			{
				if (token == "--verbose") { utils::enable(); }  // Enable logging if verbose flag is present

				cli::FlagInfo flagInfo = longFlagMap[token];
				if (flagInfo.type == cli::FlagType::Required) 
				{
					++i;
					if (i >= tokens.size()) {
						utils::logError("<Error> | Parser::parse() > 'Expected value for flag '" + token + "' not provided.'");
						return result;  // Return an invalid ParsedCommand
					}
					else {
						result.flags[token] = tokens[i];  // Store the flag value
					}
				}
				else
				{
					result.flags[token] = "";  // No value needed for this flag
				}
			}

			else if (shortFlagMap.contains(token)) 
			{
				if (token == "-v") { utils::enable(); }  // Enable logging if verbose flag is present

				cli::FlagInfo flagInfo = shortFlagMap[token];
				if (flagInfo.type == cli::FlagType::Required) 
				{
					++i;
					if (i >= tokens.size()) {
						utils::logError("<Error> | Parser::parse() > 'Expected value for flag '" + token + "' not provided.'");
						return result;  // Return an invalid ParsedCommand
					}
					else {
						result.flags[token] = tokens[i];  // Store the flag value
					}
				}
				else
				{
					result.flags[token] = "";  // No value needed for this flag
				}
			}

			else 
			{
				result.args.push_back(token);  // Store as a command argument
			}
		}
		result.valid = true;
		return result;
	}

	cli::ParsedCommand cli::Parser::getParsedCommand() const { return parsedCommand; }
};