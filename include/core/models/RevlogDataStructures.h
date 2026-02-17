#pragma once
    
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <ctime>

namespace revlog {

    /// <summary>
    /// Revlog index entry structure. 
    /// Can be re-used for all revlog based data structures: files, manifest and changelog
    /// </summary>
    struct RevlogIndexEntry {
        uint64_t dataOffset;        // byte offset into changelog.d (stored as 6 bytes on disk)
        uint16_t flags;             // delta/fulltext, compression, etc.
        uint32_t compressedLength;  // bytes stored in .d
        uint32_t uncompressedLength;  // bytes after reconstruction

        int32_t baseRev;            // base revision for delta (usually itself for changelog)
        int32_t linkRev;            // revision number (self for changelog)

        int32_t parent1;            // first parent commit rev
        int32_t parent2;            // second parent (-1 if none)

        std::array<uint8_t, 32> nodeId;  // commit hash (SHA-1 in Mercurial)
    };

    /// <summary>
    /// Represents a changelog entry (commit).
    /// </summary>
    struct ChangelogDataEntry {
        std::array<uint8_t, 32> manifestNodeId;  // Node ID of the manifest associated with this changelog entry
        std::string author;              // Author of the change
        time_t timestamp;           // Timestamp of the change
        std::string commitMessage;      // Commit message describing the change
        std::string branchName;
    };

    /// <summary>
    /// Represents a single file entry in the manifest.
    /// </summary>
    struct ManifestFileEntry {
        std::string filePath; // Path of the file in the manifest
        std::array<uint8_t, 32> manifestNodeId;  // Node ID (hash) of the file content
    };

    /// <summary>
    /// Represents a manifest entry containing multiple file entries.
    /// </summary>
    struct ManifestDataEntry {
        std::vector<ManifestFileEntry> files;  // List of files in the manifest
    };

    /// <summary>
    /// Represents a single file's raw data entry.
    /// </summary>
    struct FileDataEntry {
    std::vector<uint8_t> content;  // File content as a byte array
    };
}