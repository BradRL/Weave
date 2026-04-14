#pragma once

#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "core/models/WeaveStructures.h"
#include "core/utils/difflib.h"
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <filesystem>
#include <sstream>
#include <variant>

namespace revlogUtils
{
	#pragma pack(push, 1)
	struct EqualOp
	{
		uint32_t length{};
		uint32_t offset{};
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct InsertOp
	{
		uint32_t length{};
		std::vector<uint8_t> data{};
	};
	#pragma pack(pop)

	using DeltaOp = std::variant<EqualOp, InsertOp>;

	struct DeltaRevision
	{
		std::vector<DeltaOp> ops{};
		uint32_t baseRevision{};
		bool useSnapshot = false;  // flag to indicate if we should write the revision as a pure snapshot (no delta)
	};

#pragma pack(push, 1)
	struct fileIndexEntry
	{
		std::array<unsigned char, 128> path{};  // reduced size for redundacny
		std::array<uint8_t, 32> nodeID{};
		uint64_t dataOffset{};
		uint32_t dataLength{};
		uint32_t baseRevision;  // previous revision (will be linear)
		uint8_t flags{};  // 1 for snapshot, 0 for revision
	};
#pragma pack(pop)

	inline void writeEqualOp(std::ofstream& file, const EqualOp& op)
	{
		uint8_t opcode = 0;
		file.write(reinterpret_cast<char*>(&opcode), sizeof(opcode));
		file.write(reinterpret_cast<const char*>(&op.length), sizeof(op.length));
		file.write(reinterpret_cast<const char*>(&op.offset), sizeof(op.offset));

		std::ostringstream ss;
		ss << "[Add] DEBUG | Delta[Equals] | Length: " << op.length
			<< ", Offset: '" << op.offset << "'"
			<< ", size: " << sizeof(EqualOp);

		std::string logTxt = ss.str();
		utils::log(logTxt);
	}

	inline void writeInsertOp(std::ofstream& file, const InsertOp& op)
	{
		uint8_t opcode = 1;
		file.write(reinterpret_cast<char*>(&opcode), sizeof(opcode));
		file.write(reinterpret_cast<const char*>(&op.length), sizeof(op.length));
		file.write(reinterpret_cast<const char*>(op.data.data()), op.data.size());

		std::string data(op.data.begin(), op.data.end());
		std::ostringstream ss;
		ss << "[Add] DEBUG | Delta[Insert] | Length: " << op.length
			<< ", Data: '" << data << "'"
			<< ", size: " << (op.data.size() + 4);

		std::string logTxt = ss.str();
		utils::log(logTxt);
	}

	inline std::string applyOps(const std::string& base, const std::vector<DeltaOp>& ops)
	{
		std::string result;
		result.reserve(base.size() * 2);

		for (const auto& op : ops)
		{
			if (std::holds_alternative<EqualOp>(op))
			{
				const EqualOp& equalOp = std::get<EqualOp>(op);
				std::string part = base.substr(equalOp.offset, equalOp.length);
				result.append(part);
				std::cout << "ADDED eql '" << part << "'" << std::endl;
			}
			else if (std::holds_alternative<InsertOp>(op))
			{
				const InsertOp& insertOp = std::get<InsertOp>(op);
				std::string part(insertOp.data.begin(), insertOp.data.end());
				result.append(part);
				std::cout << "ADDED ins '" << part << "'" << std::endl;
			}
		}
		return result;
	}

	inline std::string reconstructSnapshot(const models::FileIndexEntryDisk& revision, const std::filesystem::path& dataFilePath)
	{
		std::ifstream dataFile(dataFilePath, std::ios::binary);
		dataFile.seekg(revision.dataOffset, std::ios::beg);
		std::vector<char> buffer(static_cast<size_t>(revision.dataLength));
		dataFile.read(buffer.data(), revision.dataLength);
		dataFile.close();
		return std::string(buffer.data(), buffer.size());
	}

	inline std::vector<DeltaOp> parseDeltaRevision(const std::vector<char>& revData)
	{
		int pointer = 0;
		std::vector<DeltaOp> ops;

		while (pointer < revData.size())
		{
			uint8_t opcode = revData[pointer];
			pointer++;

			if (opcode == 0)  // Equals 
			{
				EqualOp op{};

				std::memcpy(&op.length, &revData[pointer], 4);
				pointer += 4;

				std::memcpy(&op.offset, &revData[pointer], 4);
				pointer += 4;

				ops.push_back(op);
			}
			else						// Insert
			{
				InsertOp op{};

				std::memcpy(&op.length, &revData[pointer], 4);
				pointer += 4;

				op.data.resize(op.length);
				std::memcpy(op.data.data(), &revData[pointer], op.length);
				pointer += op.length;

				ops.push_back(op);
			}
		}

		return ops;
	}

	inline std::string reconstructRevision(const std::string& filePath, const std::filesystem::path& revlogPath, const std::array<uint8_t, 32>& hash)
	{
		std::filesystem::path indexFilePath = revlogPath / (filePath + ".i");
		std::filesystem::path dataFilePath = revlogPath / (filePath + ".d");

		models::FileIndexEntryDisk diskIndexEntry{};
		std::ifstream indexFile(indexFilePath, std::ios::binary);

		std::vector<models::FileIndexEntryDisk> revisions{};
		int targetRevision = -1;
		int revisionIndex = 0;

		while (indexFile.read(reinterpret_cast<char*>(&diskIndexEntry), sizeof(models::FileIndexEntryDisk)))
		{
			if (std::equal(diskIndexEntry.nodeID.begin(), diskIndexEntry.nodeID.end(), hash.begin()))
			{
				targetRevision = revisionIndex;
			}
			revisions.push_back(diskIndexEntry);
			revisionIndex++;
		}

		if (targetRevision < 0) {
			targetRevision = static_cast<int>(revisions.size()) - 1;  // default to last revision if not found
		}

		if (revisions.size() == 0)
		{
			std::cout << "No Previous Revision Found for file '" << filePath << std::endl;
			return "";
		}

		indexFile.close();

		// Return full snapshot if flag is set, otherwise reconstruct using deltas
		if (revisions[targetRevision].flags == 1)
		{
			std::cout << "Previous Revision is a Snapshot, no need to apply deltas. '" << targetRevision << "'" << std::endl;
			return reconstructSnapshot(revisions[targetRevision], dataFilePath);
		}

		std::vector<uint32_t> deltaChain;
		int chainIndex = targetRevision;
		deltaChain.push_back(chainIndex);

		std::cout << revisions.size() << " total revisions found for file '" << filePath << "'" << std::endl;
		int a = 0;
		for (const auto& r : revisions) 
		{
			std::cout << "Revision "<< a << " | base: " << r.baseRevision << " | flags: " << r.flags << std::endl;
			a++;
		}

		while (revisions[chainIndex].flags != 1)
		{
			chainIndex = revisions[chainIndex].baseRevision - 1;
			
			deltaChain.push_back(chainIndex);
		}

		

		std::reverse(deltaChain.begin(), deltaChain.end());

		std::string result = reconstructSnapshot(revisions[deltaChain[0]], dataFilePath);

		for (size_t i = 1; i < deltaChain.size(); i++)
		{
			const auto& entry = revisions[deltaChain[i]];

			std::ifstream dataFile(dataFilePath, std::ios::binary);
			dataFile.seekg(entry.dataOffset);

			std::vector<char> buffer(entry.dataLength);
			dataFile.read(buffer.data(), entry.dataLength);

			auto ops = parseDeltaRevision(buffer);

			result = applyOps(result, ops);
		}

		return result;
	}

	inline std::string readFile(const std::filesystem::path& filePath) 
	{
		std::ifstream file(filePath, std::ios::binary);

		file.seekg(0, std::ios::end);
		size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::string buffer(size, '\0');
		file.read(buffer.data(), size);

		return buffer;
	}

	inline uint32_t getRevisionCount(const std::string& filePath, const std::filesystem::path& revlogPath)
	{
		std::filesystem::path indexFilePath = revlogPath / (filePath + ".i");

		models::FileIndexEntryDisk diskIndexEntry{};
		std::ifstream indexFile(indexFilePath, std::ios::binary);

		uint32_t revCount= 0;

		while (indexFile.read(reinterpret_cast<char*>(&diskIndexEntry), sizeof(models::FileIndexEntryDisk)))
		{
			revCount++;
		}
		return revCount;
	}

	inline DeltaRevision generateDeltas(const std::string& filePath, const std::filesystem::path& revlogPath, const std::array<uint8_t, 32>& hash)
	{
		DeltaRevision revision{};

		revision.baseRevision = getRevisionCount(filePath, revlogPath);
		std::string a = reconstructRevision(filePath, revlogPath, hash);
		std::string b = readFile(filePath);
		std::cout << "Reconstructed Base Revision: \n'\n" << a << "\n'" << std::endl;

		auto matcher = difflib::MakeSequenceMatcher(a, b);
		auto opcodes = matcher.get_opcodes();
		int compressedSize = 0;

		for (const auto& op : opcodes) {
			size_t i1, i2, j1, j2;
			std::string tag;
			std::tie(tag, i1, i2, j1, j2) = op;

			if (tag == "equal") {
				EqualOp deltaOp{};
				deltaOp.offset = i1;
				deltaOp.length = i2 - i1;
				revision.ops.push_back(deltaOp);
				compressedSize += sizeof(EqualOp) + 1;
			}
			else if (tag == "insert" || tag == "replace")
			{
				InsertOp deltaOp{};
				deltaOp.length = j2 - j1;
				deltaOp.data.insert(deltaOp.data.end(), b.begin() + j1, b.begin() + j2);
				revision.ops.push_back(deltaOp);
				compressedSize += deltaOp.data.size() + 4 + 1;
			}
		}

		if (b.size() <= compressedSize)
		{
			revision.useSnapshot = true;
			return revision;
		}

		return revision;
	}
}