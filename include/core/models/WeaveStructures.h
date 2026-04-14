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
		uint32_t baseRevision;  // previous revision (will be linear)
		uint16_t flags;  // fulltext, delta, compression
	};

	#pragma pack(push, 1)
	struct FileIndexEntryDisk
	{
		std::array<unsigned char, 256> path{};
		std::array<uint8_t, 32> nodeID{};
		uint64_t dataOffset{};
		uint32_t dataLength{};
		uint32_t baseRevision{};
		uint16_t flags{};  // fulltext, delta (1/0)
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct stageEntyDisk
	{
		std::array<unsigned char, 256> path{};
		std::array<uint8_t, 32> nodeID{};
		uint16_t flags{};
		// add flag for force manifest removal
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct manifestDataEntryDisk
	{
		std::array<unsigned char, 256> path{};
		std::array<uint8_t, 32> nodeID{};
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct manifestIndexEntryDisk
	{
		std::array<uint8_t, 32> nodeID{};
		uint64_t dataOffset{};
		uint32_t dataLength{};  // Note length will always be divisible by n blocks of 288 bytes (size of file data entry)
		uint16_t flags{};  // NO IMPLEMENTATION YET, but could be used for future optimizations like compression or delta encoding
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct changelogEntryDisk
	{
		std::array<uint8_t, 32> commitNodeID{};
		std::array<uint8_t, 32> parentNodeID{};
		char timestamp[20]{};
		std::array<unsigned char, 256> message{};
		uint64_t dataOffset{};
		uint32_t dataLength{};
	};
	#pragma pack(pop)
}