/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2021, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file HL1MDLLoader.cpp
 *  @brief Implementation for the Half-Life 1 MDL loader.
 */

#include "R5MDLLoader.h"

#include <assimp/BaseImporter.h>
#include <assimp/StringUtils.h>
#include <assimp/ai_assert.h>
#include <assimp/qnan.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>

#include <iomanip>
#include <sstream>
#include <map>

namespace Assimp {
namespace MDL {
namespace RespawnR5 {

// ------------------------------------------------------------------------------------------------
R5MDLLoader::R5MDLLoader(
    aiScene *scene,
    IOSystem *io,
    const unsigned char *buffer,
    const std::string &file_path) :
    scene_(scene),
    io_(io),
    mdl_buffer_(buffer),
    file_path_(file_path),
    header_(nullptr),
    mdl_bodyparts_(nullptr),
    vg_buffer_(nullptr) {
    load_mdl_file();
    parse_mdl_file();
}

// ------------------------------------------------------------------------------------------------
R5MDLLoader::~R5MDLLoader() {
    release_resources();
}

// ------------------------------------------------------------------------------------------------
void R5MDLLoader::release_resources() {
    if(vg_buffer_) {
        delete[] vg_buffer_;
        vg_buffer_ = nullptr;
    }
}

// ------------------------------------------------------------------------------------------------
void R5MDLLoader::parse_mdl_file() {
    
}

// ------------------------------------------------------------------------------------------------
void R5MDLLoader::load_mdl_file() {
    header_ = (const studiohdr_t *)mdl_buffer_;

    if(!header_->numbodyparts)
        throw DeadlyImportError("Model has no bodyparts in mdl!");

    mdl_bodyparts_ = (const mstudiobodyparts_t*)(mdl_buffer_ + header_->bodypartindex);

    throw DeadlyImportError("nono zone");
}

// ------------------------------------------------------------------------------------------------
void R5MDLLoader::load_vg_file() {
    std::string vg_file_path =
            DefaultIOSystem::absolutePath(file_path_) +
            io_->getOsSeparator() +
            DefaultIOSystem::completeBaseName(file_path_) +
            ".vg";
    
    if(!io_->Exists(vg_file_path)) {
        throw DeadlyImportError("Missing VG file ", DefaultIOSystem::fileName(vg_file_path));
    }

    std::unique_ptr<IOStream> file(io_->Open(vg_file_path));

    if (file.get() == nullptr) {
        throw DeadlyImportError("Failed to open VG file ", DefaultIOSystem::fileName(vg_file_path));
    }

    const size_t file_size = file->FileSize();
    vg_buffer_ = new unsigned char[1 + file_size];
    file->Read((void *)vg_buffer_, 1, file_size);
    vg_buffer_[file_size] = '\0';
}

} // namespace HalfLife
} // namespace MDL
} // namespace Respawn