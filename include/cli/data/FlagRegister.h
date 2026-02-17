#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace cli {

	enum class FlagType {
		Required,   // Flag must be provided with a value (e.g. `-m "commit message"`)
		None        // Flag does not take a value (e.g. `--verbose`)
	};

	struct FlagInfo {
		std::string shortName;
		std::string longName;
		FlagType type;
	};

	inline const std::vector<FlagInfo>& getFlagInfo() {
		static std::vector<FlagInfo> flagInfoList = {
			{"-v", "--verbose", FlagType::None},
			{"-a", "--author", FlagType::Required}
		};
		return flagInfoList;
	};

	inline const std::unordered_map<std::string, FlagInfo>& buildShortFlagMap() {
		static std::unordered_map<std::string, FlagInfo> shortFlagMap;
		for (auto& f : getFlagInfo()) {
			shortFlagMap[f.shortName] = f;
		}
		return shortFlagMap;
	}

	inline const std::unordered_map<std::string, FlagInfo>& buildLongFlagMap() {
		static std::unordered_map<std::string, FlagInfo> longFlagMap;
		for (auto& f : getFlagInfo()) {
			longFlagMap[f.longName] = f;
		}
		return longFlagMap;
	}
}