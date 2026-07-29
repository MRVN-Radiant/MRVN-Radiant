/*
   Copyright (C) 2026 MRVN-Radiant contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of MRVN-Radiant.

   MRVN-Radiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   MRVN-Radiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRVN-Radiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

// FIXME: VPK reading should be a static lib so remap can also do it

#include <vector>

#include <lzma.h>

#include "bytestreamutils.h"
#include "iarchive.h"
#include "debugging/debugging.h"
#include "stream/filestream.h"
#include "string/string.h"

#if defined( __BIG_ENDIAN__ )
#error "archivevpk was written only for little endian!"
#endif

// TODO: separate vpk reading into a static lib so remap can do it too
// TODO: archivevpk should just be a wrapper for radiant module system
// TODO: make code good
// TODO: test

struct VPKHeader_t
{
	uint32_t signature; // 0x55aa1234
	uint16_t versionMajor; // 2
	uint16_t versionMinor; // 3
	uint32_t treeSize;
	uint32_t unk;
};
static_assert(sizeof(VPKHeader_t) == 16);

#pragma pack(push, 1)
struct VPKDirectoryEntry_t
{
	uint32_t crc;
	uint16_t unk;
};
#pragma pack(pop)
static_assert(sizeof(VPKDirectoryEntry_t) == 6);

#pragma pack(push, 1)
struct VPKEntryDesc_t
{
	char unk[6];
	uint64_t offset;
	uint64_t lengthCompressed;
	uint64_t lengthUncompressed;
};
#pragma pack(pop)
static_assert(sizeof(VPKEntryDesc_t) == 30);

struct VPKRuntimeEntryDesc_t
{
	uint16_t m_archiveIndex;
	VPKEntryDesc_t m_desc;
};

struct VPKRuntimeFile_t
{
	std::string m_filename;
	std::string m_fullpath;
	std::vector<VPKRuntimeEntryDesc_t> m_entries;

	bool operator==(const std::string_view& other) const;
};

bool VPKRuntimeFile_t::operator==(const std::string_view& other) const
{
	return m_filename == other;
}

struct VPKRuntimeDir_t
{
	std::string m_name;
	std::vector<VPKRuntimeDir_t> m_dirs;
	std::vector<VPKRuntimeFile_t> m_files;
	size_t					m_depth;

	bool operator==(const std::string_view& other) const;

	void expandSelf(std::vector<VPKRuntimeDir_t*>& dirs);
};

bool VPKRuntimeDir_t::operator==(const std::string_view& other) const
{
	return m_name == other;
}

void VPKRuntimeDir_t::expandSelf(std::vector<VPKRuntimeDir_t*>& dirs)
{
	for (auto& dir : m_dirs) {
		dirs.push_back(&dir);
	}

	for (auto& dir : m_dirs) {
		dir.expandSelf(dirs);
	}
}

class VPKArchiveFile final : public ArchiveFile
{
public:
	VPKArchiveFile(const VPKRuntimeFile_t& file);

	void release() override;
	std::size_t size() const override;
	const char* getName() const override;
	InputStream& getInputStream() override;
private:
	VPKRuntimeFile_t m_file;
};

VPKArchiveFile::VPKArchiveFile(const VPKRuntimeFile_t& file)
	: m_file(file)
{
}
void VPKArchiveFile::release()
{
	delete this;
}
std::size_t VPKArchiveFile::size() const
{
	return 0;
}
const char* VPKArchiveFile::getName() const
{
	return m_file.m_fullpath.c_str();
}
InputStream& VPKArchiveFile::getInputStream()
{
}

class VPKArchive final : public Archive
{
public:
	VPKArchive(const std::vector<std::string>& files, const std::vector<VPKRuntimeDir_t>& fileTree);

	void release() override;
	ArchiveFile* openFile(const char* name) override;
	ArchiveTextFile* openTextFile(const char* name) override;
	bool containsFile(const char* name) override;
	void forEachFile(VisitorFunc visitor, const char* root) override;
private:
	std::vector<std::string> m_files;
	std::vector<VPKRuntimeDir_t> m_fileTree;
};

VPKArchive::VPKArchive(const std::vector<std::string>& files, const std::vector<VPKRuntimeDir_t>& fileTree)
	: m_files(files)
	, m_fileTree(fileTree)
{

}

void VPKArchive::release()
{
	delete this;
}

ArchiveFile* VPKArchive::openFile(const char* name)
{
	std::string path_copy(name);
	std::vector<std::string> decomposed_path;
	while (true) {
		const std::string::size_type separator_idx = path_copy.find('/');
		if (separator_idx == std::string::npos) {
			break;
		}

		const std::string dir_name = path_copy.substr(0, separator_idx);
		path_copy.erase(0, separator_idx + 1);

		decomposed_path.emplace_back(dir_name);
	}
	// Skip filename from path
	// decomposed_path.emplace_back(path_copy);

	VPKRuntimeDir_t* runtime_dir = nullptr;
	std::vector<VPKRuntimeDir_t>* current_dir = &m_fileTree;
	for (const auto& dir_name : decomposed_path) {
		const std::vector<VPKRuntimeDir_t>::iterator dir_it = std::find(current_dir->begin(), current_dir->end(), dir_name);
		if (dir_it != current_dir->end()) {
			runtime_dir = &*dir_it;
			current_dir = &runtime_dir->m_dirs;
		}
		else {
			return nullptr;
		}
	}

	if (runtime_dir == nullptr) {
		return nullptr;
	}

	std::vector<VPKRuntimeFile_t>::iterator file_it = std::find(runtime_dir->m_files.begin(), runtime_dir->m_files.end(), path_copy);

	if (file_it == runtime_dir->m_files.end()) {
		return nullptr;
	}

	const VPKRuntimeFile_t& file = *file_it;

	return nullptr;
}

ArchiveTextFile* VPKArchive::openTextFile(const char* name)
{
	return nullptr;
}

bool VPKArchive::containsFile(const char* name)
{
	for (const auto& file : m_files) {
		if (file == name) {
			return true;
		}
	}
	return false;
}

void VPKArchive::forEachFile(VisitorFunc visitor, const char* root) // FIXME: What if root is ""
{
	std::string root_copy(root);

	ASSERT_MESSAGE(root_copy.back() == '/', "Expected root path to end with '/'!");
	root_copy.erase(root_copy.length() - 1);

	std::vector<std::string> decomposed_path;
	while (true) {
		const std::string::size_type separator_idx = root_copy.find('/');
		if (separator_idx == std::string::npos) {
			break;
		}

		const std::string dir_name = root_copy.substr(0, separator_idx);
		root_copy.erase(0, separator_idx + 1);

		decomposed_path.emplace_back(dir_name);
	}
	decomposed_path.emplace_back(root_copy);

	VPKRuntimeDir_t* root_dir = nullptr;
	std::vector<VPKRuntimeDir_t>* current_dir = &m_fileTree;
	for (const auto& dir_name : decomposed_path) {
		const std::vector<VPKRuntimeDir_t>::iterator dir_it = std::find(current_dir->begin(), current_dir->end(), dir_name);
		if (dir_it != current_dir->end()) {
			root_dir = &*dir_it;
			current_dir = &root_dir->m_dirs;
		}
		else {
			return;
		}
	}

	if (root_dir == nullptr) {
		return;
	}

	std::vector<VPKRuntimeDir_t*> dirs;
	dirs.push_back(root_dir);
	root_dir->expandSelf(dirs);

	for (const auto& runtime_dir : dirs) {
		for (const auto& dir : runtime_dir->m_dirs) {
			visitor.directory(dir.m_name.c_str(), runtime_dir->m_depth);
		}

		for (const auto& file : runtime_dir->m_files) {
			visitor.file(file.m_fullpath.c_str());
		}
	}

}

Archive* OpenArchive( const char* name )
{
	if (! string_equal_suffix(name, "_dir.vpk")) {
		return nullptr;
	}

	FileInputStream istream(name);

	if (istream.failed()) {
		return nullptr;
	}
	VPKHeader_t header;
	if (! istream_read_raw_safe(istream, header)) {
		return nullptr;
	}

	if (header.signature != 0x55AA1234) {
		return nullptr;
	}

	if ((header.versionMajor != 2) || (header.versionMinor != 3)) {
		return nullptr;
	}

	std::vector<std::string> files;
	std::vector<VPKRuntimeDir_t> file_tree;

	while (true) {
		std::string type;
		if (! istream_read_string_safe(istream, type)) {
			return nullptr;
		}

		if (type.empty()) {
			break;
		}

		while (true) {
			std::string path;
			if (! istream_read_string_safe(istream, path)) {
				return nullptr;
			}

			if (path.empty()) {
				break;
			}

			std::string path_copy(path);
			std::vector<std::string> decomposed_path;
			while (true) {
				const std::string::size_type separator_idx = path_copy.find('/');
				if (separator_idx == std::string::npos) {
					break;
				}

				const std::string dir_name = path_copy.substr(0, separator_idx);
				path_copy.erase(0, separator_idx + 1);

				decomposed_path.emplace_back(dir_name);
			}
			decomposed_path.emplace_back(path_copy);

			VPKRuntimeDir_t* runtime_dir = nullptr;
			std::vector<VPKRuntimeDir_t>* current_dir = &file_tree;
			for (size_t ii = 0; ii < decomposed_path.size(); ++ii) {
				const std::string& dir_name = decomposed_path[ii];
				const std::vector<VPKRuntimeDir_t>::iterator dir_it = std::find(current_dir->begin(), current_dir->end(), dir_name);
				if (dir_it != current_dir->end()) {
					runtime_dir = &*dir_it;
					current_dir = &runtime_dir->m_dirs;
				}
				else {
					runtime_dir = &current_dir->emplace_back();
					runtime_dir->m_name = dir_name;
					runtime_dir->m_depth = ii;
					current_dir = &runtime_dir->m_dirs;
				}
			}

			while (true) {
				std::string filename;
				if (! istream_read_string_safe(istream, filename)) {
					return nullptr;
				}

				if (filename.empty()) {
					break;
				}

				const std::string full_path = path + "/" + filename + "." + type;
				files.emplace_back(full_path);

				VPKRuntimeFile_t runtimeFile = {
					.m_filename = filename + "." + type,
					.m_fullpath = full_path,
				};

				runtime_dir->m_files.push_back(runtimeFile);

				VPKDirectoryEntry_t entry;
				if (! istream_read_raw_safe(istream, entry)) {
					return nullptr;
				}

				while (true) {
					uint16_t archiveIndex;
					if (! istream_read_raw_safe(istream, archiveIndex)) {
						return nullptr;
					}

					if (archiveIndex == 0xFFFF) {
						break;
					}

					VPKEntryDesc_t descriptor;
					if (! istream_read_raw_safe(istream, descriptor)) {
						return nullptr;
					}

					VPKRuntimeEntryDesc_t runtimeDesc = {
						.m_archiveIndex = archiveIndex,
						.m_desc = descriptor,
					};
					runtime_dir->m_files.back().m_entries.push_back(runtimeDesc);
				}
			}
		}
	}

	return new VPKArchive(files, file_tree);
}
