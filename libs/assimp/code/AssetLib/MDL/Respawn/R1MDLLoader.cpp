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

#include "R1MDLLoader.h"

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
namespace Respawn {

// ------------------------------------------------------------------------------------------------
R1MDLLoader::R1MDLLoader(
    aiScene *scene,
    IOSystem *io,
    const unsigned char *buffer,
    const std::string &file_path) {
    load_file();
}

// ------------------------------------------------------------------------------------------------
R1MDLLoader::~R1MDLLoader() {
    
}

// ------------------------------------------------------------------------------------------------
void R1MDLLoader::load_file() {

}

} // namespace HalfLife
} // namespace MDL
} // namespace Respawn