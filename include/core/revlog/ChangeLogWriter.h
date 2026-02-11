#pragma once

#include <iostream>
#include <string>
#include <fstream>
//#include <openssl/sha.h>
#include "core/revlog/RevlogDataStructures.h"

namespace revlog {
	class ChangeLogWriter
	{
	public:
		ChangeLogWriter();
		~ChangeLogWriter();

		int addCommit();
	private:
		std::string indexFilePath;
		std::ofstream indexFile;

		std::string dataFilePath;
		std::ofstream dataFile;

		revlog::ChangelogDataEntry currentDataEntry;
		revlog::RevlogIndexEntry currentIndexEntry;
	};
}