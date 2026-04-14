#pragma once

#include "cli/commandHandlers/ICommandHandler.h"
#include "utils/Logger.h"
#include "core/utils/IOUtils.h"
#include "core/utils/revlogUtils.h"
#include "core/utils/difflib.h"
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <filesystem>
#include <variant>

namespace cli {

	/// <summary>
	/// Test command handler for testing purposes.
	/// </summary>
	class TestHandler : public ICommandHandler {

	public:

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

	public:
		/// <summary>
		/// Executes command. Creates a Weave repository.
		/// </summary>
		void execute() override;

		/// <summary>
		/// Sets command arguments.
		/// </summary>
		/// <param name="cmd">processed command args</param>
		void set(const ParsedCommand& cmd);

		void genDelta(const std::string& a, const std::string& b);
	};
}