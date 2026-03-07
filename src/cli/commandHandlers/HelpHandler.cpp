#include "cli/commandHandlers/HelpHandler.h"

namespace cli {

	bool cli::HelpHandler::validateCommand()
	{
		if (commandData.args.size() > 1)
		{
			utils::logError("[Help] ERROR | Too many arguments provided. Command syntax: `weave help [command]`");
			return false;  // Invalid if more than 1 arg provided
		}

		for (const auto& [key, value] : commandData.flags)
		{
			utils::logError("[Help] ERROR | Invalid flag '" + key + "' provided. Command syntax: `weave help [command]`");
			return false;  // Invalid if any flags provided
		}

		return true;
	}

	void cli::HelpHandler::execute()
	{
		if (!validateCommand()) {
			return;  // Exit if validation fails
		}

        if (commandData.args.size() == 0)
        {
            std::cout << "Weave Version 1.0.0\n\n";
            std::cout << "Usage: weave <command> [args] [flags]\n\n";
            std::cout << "Available commands:\n";
            std::cout << "  init    Initializes a new repository\n";
            std::cout << "  add     Stages files for the next commit\n";
            std::cout << "  rm      Removes files from staging\n";
            std::cout << "  commit  Commits staged changes to the repository\n";
            std::cout << "  log     Shows commit history\n\n";
            std::cout << "Use `weave help <command>` for detailed information on a specific command.\n";
        }
        else
        {
            std::string command = commandData.args[0];

            if (command == "init")
            {
                std::cout << "Usage: weave init <repository name> [flags]\n\n";
                std::cout << "Initializes a new repository in the current directory with the given name.\n\n";

                std::cout << "Args:\n";
                std::cout << "  <repository name>    Required parameter, sets the repo's name for easy identification.\n";
                std::cout << "                       Directory path is assumed to be same as invocation path.\n\n";

                std::cout << "Flags:\n";
                std::cout << "  -a, --author         Sets the author name for the repository (defaults to local user)\n";
                std::cout << "  -v, --verbose        Enables verbose output during initialization\n";
            }
            else if (command == "add")
            {
                std::cout << "Usage: weave add <file> [file(s)]\n\n";
                std::cout << "Stages files for the next commit.\n\n";

                std::cout << "Args:\n";
                std::cout << "  <file>               Required parameter, stages this file.\n";
                std::cout << "  [file(s)]            Optional parameters for additional files. Files can include paths.\n\n";

                std::cout << "Flags:\n";
                std::cout << "  -v, --verbose        Enables verbose output during staging\n";
            }
            else if (command == "rm" || command == "remove")
            {
                std::cout << "Usage: weave rm <file> [file(s)]\n\n";
                std::cout << "Removes files from staging.\n\n";

                std::cout << "Args:\n";
                std::cout << "  <file>               Required parameter, removes this file from staging.\n";
                std::cout << "  [file(s)]            Optional parameters for additional files. Files can include paths.\n\n";

                std::cout << "Flags:\n";
                std::cout << "  -v, --verbose        Enables verbose output during removal\n";
            }
            else
            {
                std::cerr << "[Help] ERROR | No help information found for command '" << command << "'\n";
            }
        }
	}
}