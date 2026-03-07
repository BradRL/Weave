#include "core/commandServices/InitService.h"

namespace init {

	init::InitService::InitService(const cli::ParsedCommand& cmd) : commandData(cmd) {}

	void init::InitService::initializeRepository()
	{
		if (!validateInitializeRepository()) { return; }

		std::filesystem::path root = utils::getWeaveRoot();

		createRepoLayout(root);
		createConfigFile(root);
		createStageFile(root);
		createChangelogFiles(root);
		createManifestFiles(root);
	}

	bool init::InitService::validateInitializeRepository()
	{
		std::string repoName = commandData.args[0];
		std::filesystem::path configPath = commandData.invocationPath;
		std::filesystem::path programRepoPath = utils::getWeaveRoot();
		std::filesystem::path targetRepoPath = programRepoPath / repoName;

		for (const auto& eachPath : std::filesystem::directory_iterator(programRepoPath))
		{
			// Checks if repository with same name already exists.
			if (utils::isSameOrSubdir(eachPath, targetRepoPath))
			{
				utils::logError("[Init] ERROR | Repository named `" + repoName + "` aready exists");
				return false;
			}

			// Checks if repository contains same or sub directory of current invocation path.
			utils::repoConfig repoConfig = utils::readConfig(eachPath.path() / ".weave" / "config");

			if (utils::isSameOrSubdir(repoConfig.root, configPath))
			{
				utils::logError("[Init] ERROR | Repository location overlaps with existing repository");
				return false;
			}
		}

		return true;
	}

	void init::InitService::createRepoLayout(const std::filesystem::path& root)
	{
		std::filesystem::create_directories(root / commandData.args[0] / ".weave" / "store" / "data");
	}

	void init::InitService::createConfigFile(const std::filesystem::path& root)
	{
		std::ofstream configFile(root / commandData.args[0] / ".weave" / "config");

		std::time_t now = std::time(nullptr);
		std::tm tm = *std::localtime(&now);

		char timeStr[20];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M", &tm);

		const char* username = std::getenv("USERNAME"); // Windows standard
		std::string author = std::string(username);

		configFile << "name=" << commandData.args[0] << std::endl;
		configFile << "root=" << commandData.invocationPath.string() << std::endl;
		configFile << "created=" << timeStr << std::endl;

		if (commandData.flags.contains("-a"))
		{
			configFile << "author=" << commandData.flags["-a"] << std::endl;
		}
		else if (commandData.flags.contains("--author"))
		{
			configFile << "author=" << commandData.flags["--author"] << std::endl;
		}
		else
		{
			configFile << "author=" << author << std::endl;
		}

		configFile.close();
	}

	void init::InitService::createStageFile(const std::filesystem::path& root) 
	{
		std::ofstream stageFile(root / commandData.args[0] / ".weave" / "stage");
		stageFile.close();
	}

	void init::InitService::createChangelogFiles(const std::filesystem::path& root)
	{
		std::ofstream changelogFileD(root / commandData.args[0] / ".weave" / "store" / "changelog.d");
		changelogFileD.close();

		std::ofstream changelogFileI(root / commandData.args[0] / ".weave" / "store" / "changelog.i");
		changelogFileI.close();
	}

	void init::InitService::createManifestFiles(const std::filesystem::path& root)
	{
		std::ofstream manifestFileD(root / commandData.args[0] / ".weave" / "store" / "manifest.d");
		manifestFileD.close();

		std::ofstream manifestFileI(root / commandData.args[0] / ".weave" / "store" / "manifest.i");
		manifestFileI.close();
	}
}