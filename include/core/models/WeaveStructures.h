#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <ctime>

namespace models {

	struct FileIndexEntry
	{
		std::string path;
		std::array<uint8_t, 32> nodeID;
		uint64_t dataOffset;
		uint32_t dataLength;
		uint16_t flags;  // fulltext, delta, compression
	};

	#pragma pack(push, 1)
	struct FileIndexEntryDisk
	{
		std::array<unsigned char, 256> path{};
		std::array<uint8_t, 32> nodeID{};
		uint64_t dataOffset{};
		uint32_t dataLength{};
		uint16_t flags{};  // fulltext, delta, compression

		// 302 bytes total
		// 256 + 32 + 8 + 4 + 2 (removes +2 alignment bytes for arrays) using pack to ensure no padding
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct stageEntyDisk
	{
		std::array<unsigned char, 256> path{};
		std::array<uint8_t, 32> nodeID{};
	};
	#pragma pack(pop)
}