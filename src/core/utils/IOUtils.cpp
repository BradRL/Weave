#include "core/utils/IOUtils.h"

namespace utils {

	bool utils::isSameOrSubdir(const std::filesystem::path& pathA, const std::filesystem::path& pathB) {
		std::filesystem::path canonicalPathA = std::filesystem::weakly_canonical(pathA);
		std::filesystem::path canonicalPathB = std::filesystem::weakly_canonical(pathB);

		if (canonicalPathA == canonicalPathB) {
			return true;  // File paths are the same.
		}

		auto [itA, itB] = std::mismatch(canonicalPathB.begin(), canonicalPathB.end(), canonicalPathA.begin());
		return itA == canonicalPathB.end();
	}

	repoConfig utils::readConfig(const std::filesystem::path& configPath) 
	{
		std::ifstream configFile(configPath);
		repoConfig configStruct;
		std::string line;
		int splitIndex = 0;

		while (getline(configFile, line)) {
			for (int i = 0; i <= line.size(); i++) {
				if (line[i] == '=') {
					splitIndex = i;
					break;
				}
			}

			std::string key = line.substr(0, splitIndex);
			std::string value = line.substr(splitIndex + 1);

			if (key == "name") {
				configStruct.name = value;
			}
			else if (key == "root") {
				if (!value.empty() && value.front() == '"' && value.back() == '"') {
					value = value.substr(1, value.size() - 2);
				}
				configStruct.root = std::filesystem::path(value);
			}
			else if (key == "createdAt") {
				//configStruct.createdAt = std::ctime(value);
			}
			else if (key == "author") {
				configStruct.author = value;
			}
		}
		configFile.close();

		return configStruct;
	}

	std::filesystem::path getWeaveRoot() 
	{
		const char* localAppData = std::getenv("LOCALAPPDATA");
		if (!localAppData) {
			throw std::runtime_error("LOCALAPPDATA environment variable not set");
		}

		std::filesystem::path root = std::filesystem::path(localAppData) / "Weave" / "repos";

		std::filesystem::create_directories(root); 
		return root;
	}
}