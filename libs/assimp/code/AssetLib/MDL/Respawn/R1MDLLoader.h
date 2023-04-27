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

/** @file HL1MDLLoader.h
 *  @brief Declaration of the Half-Life 1 MDL loader.
 */

#ifndef AI_R1MDLLOADER_INCLUDED
#define AI_R1MDLLOADER_INCLUDED


#include "R1MDLFileData.h"

#include <memory>
#include <string>

#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>
#include <assimp/Exceptional.h>

namespace Assimp {
namespace MDL {
namespace Respawn {

class R1MDLLoader {
public:
    R1MDLLoader() = delete;
    R1MDLLoader(const R1MDLLoader &) = delete;

    /** See variables descriptions at the end for more details. */
    R1MDLLoader(
        aiScene *scene,
        IOSystem *io,
        const unsigned char *buffer,
        const std::string &file_path);

    ~R1MDLLoader();

protected:
    /** \brief Validate the header data structure of a Half-Life 1 MDL file.
     * \param[in] header Input header to be validated.
     * \param[in] is_texture_header Whether or not we are reading an MDL
     *   texture file.
     */
    void load_mdl_file();
    void load_vtx_file();
    void load_vvd_file();
    void parse_mdl_file();
    //void validate_header(const Header_HL1 *header, bool is_texture_header);
    void release_resources();
private:
    /** Output scene to be filled */
    aiScene *scene_;

    /** Output I/O handler. Required for additional IO operations. */
    IOSystem *io_;

    /** Buffer from MDLLoader class. */
    const unsigned char *mdl_buffer_;

    /** The full file path to the MDL file we are trying to load.
     * Used to locate other MDL files since MDL may store resources
     * in external MDL files. */
    const std::string &file_path_;

    /** Main MDL header. */
    const studiohdr_t *header_;

    /** MDL header. */
    const studiohdr2_t *header2_;

    /** MDL bodyparts. */
    const mstudiobodyparts_t *mdl_bodyparts_;

    /** MDL stringtable */
    const char *mdl_stringtable_;

    /** VTX file buffer class. */
    unsigned char *vtx_buffer_;

    /** VTX header */
    const FileHeader_t *vtx_header_;

    /** VVD file buffer class. */
    unsigned char *vvd_buffer_;

    /** VVD header */
    const vertexFileHeader_t *vvd_header_;

    /** VVD vertices */
    const mstudiovertex_t *vertices_;

    /** VVD Tangents */
    const vec4_t *tangents_;
};

} // namespace Respawn
} // namespace MDL
} // namespace Assimp

#endif // AI_R1MDLLOADER_INCLUDED