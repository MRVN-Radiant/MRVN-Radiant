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
    vg_buffer_(nullptr),
    vg_header_(nullptr) {
    load_vg_file();
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
void UnpackNormal(vec3_t normal, const PackedNormalTangent_t *TBN)
{
    // normal 
    int normalSign = TBN->norm_sign ? -255 : 255;

    float norm1 = TBN->norm1  + -255.5;
    float norm2 = TBN->norm2  + -255.5;

    // remove 1.0 / if issues occur

    float compressFactor = 1.0 / sqrt(((norm2 * norm2) + (255.0 * 255.0)) + (norm1 * norm1));

    float valueNorm1, valueNorm2, valueDropped;

    valueNorm1 = norm1 * compressFactor; // (1.0 / compressFactor) if issues occur
    valueNorm2 = norm2 * compressFactor;
    valueDropped = normalSign * compressFactor;

    // check which axis was dropped
    int axis = TBN->norm_dropped; //  case label value exceeds maximum value for type 
    switch(axis)
    {
        case 0:
            normal[0] = valueDropped;
            normal[1] = valueNorm2;
            normal[2] = valueNorm1;
            break;
        case 1:
            normal[0] = valueNorm1;
            normal[1] = valueDropped;
            normal[2] = valueNorm2;
            break;
        case 2:
            normal[0] = valueNorm2;
            normal[1] = valueNorm1;
            normal[2] = valueDropped;
            break;
        default:
            break;
    };
};

// ------------------------------------------------------------------------------------------------
void R5MDLLoader::parse_mdl_file() {
    // Craete aiMaterials we want to use
    if(header_->numtextures == 0)
        throw DeadlyImportError("Model has no textures!");
    
    scene_->mNumMaterials = header_->numtextures;
    scene_->mMaterials = new aiMaterial *[scene_->mNumMaterials];
    for(unsigned int i = 0; i < scene_->mNumMaterials; i++) {
        const mstudiotexture_t *texture = (const mstudiotexture_t *)(mdl_buffer_ + (int)header_->textureindex + sizeof(mstudiotexture_t) * i);
        const char* sztexture = (const char *)((const char *)texture + texture->sznameindex);

        aiString *aiTex = new aiString(sztexture);

        scene_->mMaterials[i] = new aiMaterial();
        scene_->mMaterials[i]->AddProperty(aiTex, AI_MATKEY_NAME);
    }

    // Count meshes
    // We need to know the total num of meshes so we can alloc space for them
    if(vg_header_->numLODs == 0)
        throw DeadlyImportError("VG has no LOD Headers!");
    
    int iCurrentMesh = 0;
    for(int k = 0; k < vg_header_->numLODs; k++) {
        const ModelLODHeader_t *lod = (const ModelLODHeader_t *)(vg_buffer_ + vg_header_->lodOffset + sizeof(ModelLODHeader_t) * k);
        for(int l = 0; l < lod->numMeshes; l++) {
            const MeshHeader_VG_t *vg_mesh = (const MeshHeader_VG_t *)(vg_buffer_ + vg_header_->meshOffset + sizeof(MeshHeader_VG_t) * l + lod->meshOffset * sizeof(MeshHeader_VG_t));
            iCurrentMesh += vg_mesh->numStrips;
        }
    }

    // Allocate space for our meshes
    ASSIMP_LOG_DEBUG("NumMeshes: ", iCurrentMesh);
    scene_->mNumMeshes = iCurrentMesh;
    scene_->mMeshes = new aiMesh *[iCurrentMesh];
    iCurrentMesh = 0;

    // Build the scene tree and build the meshes it uses
    aiNode *pRootNode = new aiNode();
    pRootNode->mNumChildren = header_->numbodyparts;
    pRootNode->mChildren = new aiNode *[header_->numbodyparts];
    scene_->mRootNode = pRootNode;
    // Populate the meshes
    for(int i = 0; i < header_->numbodyparts; i++) {
        const mstudiobodyparts_t *bodypart = (const mstudiobodyparts_t *)(mdl_buffer_ + header_->bodypartindex + sizeof(mstudiobodyparts_t) * i);

        aiNode *pBodyPartNode = new aiNode();
        pBodyPartNode->mNumChildren = bodypart->nummodels;
        pBodyPartNode->mChildren = new aiNode *[bodypart->nummodels];
        pRootNode->mChildren[i] = pBodyPartNode;
        pBodyPartNode->mParent = pRootNode;

        for(int j = 0; j < bodypart->nummodels; j++) {
            const mstudiomodel_t *model = (const mstudiomodel_t *)((const char*)bodypart + bodypart->modelindex + sizeof(mstudiomodel_t) * j);

            aiNode *pModelNode = new aiNode();
            pModelNode->mNumChildren = vg_header_->numLODs;
            pModelNode->mChildren = new aiNode *[vg_header_->numLODs];
            pBodyPartNode->mChildren[j] = pModelNode;
            pModelNode->mParent = pBodyPartNode;

            for(int k = 0; k < vg_header_->numLODs; k++) {
                const ModelLODHeader_t *lod = (const ModelLODHeader_t *)(vg_buffer_ + vg_header_->lodOffset + sizeof(ModelLODHeader_t) * k);

                aiNode *pModelLODNode = new aiNode();
                pModelLODNode->mNumChildren = model->nummeshes;
                pModelLODNode->mChildren = new aiNode *[model->nummeshes];
                pModelNode->mChildren[k] = pModelLODNode;
                pModelLODNode->mParent = pModelNode;

                for(int l = 0; l < lod->numMeshes; l++) {
                    // FSeek(vhdr.meshOffset + (j * sizeof(MeshHeader_VG_t)) + (lod.meshOffset * sizeof(MeshHeader_VG_t)));
                    const MeshHeader_VG_t *vg_mesh = (const MeshHeader_VG_t *)(vg_buffer_ + vg_header_->meshOffset + sizeof(MeshHeader_VG_t) * l + lod->meshOffset * sizeof(MeshHeader_VG_t));
                    const mstudiomesh_t *mdl_mesh = (const mstudiomesh_t*)((const char*)model + model->meshindex + sizeof(mstudiomesh_t) * (k % lod->numMeshes));

                    aiNode *pMeshNode = new aiNode();
                    pModelLODNode->mChildren[l] = pMeshNode;
                    pMeshNode->mParent = pModelLODNode;
                    pMeshNode->mNumMeshes = vg_mesh->numStrips;
                    pMeshNode->mMeshes = new unsigned int[vg_mesh->numStrips];

                    for(int m = 0; m < vg_mesh->numStrips; m++) {
                        // FSeek(vhdr.stripOffset + ((mesh.stripOffset + n) * sizeof(StripHeader_t)));
                        const StripHeader_t *stripHeader = (const StripHeader_t *)(vg_buffer_ + vg_header_->stripOffset + (vg_mesh->stripOffset + m) * sizeof(StripHeader_t) );
                        pMeshNode->mMeshes[m] = iCurrentMesh;

                        // Build te aiMesh
                        aiMesh *pMesh = new aiMesh();

                        pMesh->mMaterialIndex = mdl_mesh->material;

                        // Build vertices
                        pMesh->mNumVertices = stripHeader->numVerts;
                        pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
                        pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
                        pMesh->mTextureCoords[0] = new aiVector3D[pMesh->mNumVertices];
                        pMesh->mNumUVComponents[0] = 2;
                        for(unsigned int v = 0; v < pMesh->mNumVertices; v++) {
                            const void *vertex = (const void *)(vg_buffer_ + vg_header_->vertOffset + vg_mesh->vertOffset + (vg_mesh->vertCacheSize * v));
                            int shift = 0;
                            if(vg_mesh->flags & VG_POSITION) {
                                const Position_t *pos = (const Position_t *)vertex;
                                pMesh->mVertices[v] = aiVector3D(pos->xyz[0], pos->xyz[1], pos->xyz[2]);
                                shift += sizeof(Position_t);
                            } else if(vg_mesh->flags & VG_PACKED_POSITION) {
                                const PackedPosition_t *pos = (const PackedPosition_t *)vertex;
                                float x, y, z;
                                x = pos->x * 0.0009765625 - 1024.0f;
                                y = pos->y * 0.0009765625 - 1024.0f;
                                z = pos->z * 0.0009765625 - 2048.0f;
                                pMesh->mVertices[v] = aiVector3D(x, y, z);
                                shift += sizeof(PackedPosition_t);
                            }

                            if(vg_mesh->flags & VG_PACKED_WEIGHTS)
                                shift += 8;
                            
                            const PackedNormalTangent_t *tbn = (const PackedNormalTangent_t *)((const char *)vertex + shift);
                            shift += sizeof(PackedNormalTangent_t);
                            vec3_t normal;
                            UnpackNormal(normal, tbn);
                            pMesh->mNormals[v] = aiVector3D(normal[0], normal[1], normal[2]);

                            if(vg_mesh->flags & VG_VERTEX_COLOR)
                                shift += sizeof(uint8_t) * 4;
                            
                            const TexCoord_t *texCoord = (const TexCoord_t *)((const char *)vertex + shift);

                            pMesh->mTextureCoords[0][v] = aiVector3D(texCoord->st[0], texCoord->st[1], 0.0);
                        }
                        
                        // Build faces
                        pMesh->mNumFaces = stripHeader->numIndices / 3;
                        pMesh->mFaces = new aiFace[pMesh->mNumFaces];
                        const uint16_t *indices = (const uint16_t *)((const char *)vg_buffer_ + vg_header_->indexOffset + (vg_mesh->indexOffset * 2) + (stripHeader->indexOffset * 2));
                        for(unsigned int f = 0; f < pMesh->mNumFaces; f++) {
                            pMesh->mFaces[f].mNumIndices = 3;
                            pMesh->mFaces[f].mIndices = new unsigned int[3];
                            pMesh->mFaces[f].mIndices[0] = *(uint16_t*)((const char *)indices + sizeof(uint16_t) * (f * 3 + 2) );
                            pMesh->mFaces[f].mIndices[1] = *(uint16_t*)((const char *)indices + sizeof(uint16_t) * (f * 3 + 1) );
                            pMesh->mFaces[f].mIndices[2] = *(uint16_t*)((const char *)indices + sizeof(uint16_t) * (f * 3 + 0) );
                        }

                        scene_->mMeshes[iCurrentMesh] = pMesh;
                        iCurrentMesh++;
                    }
                }
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------
void R5MDLLoader::load_mdl_file() {
    header_ = (const studiohdr_t *)mdl_buffer_;

    if(!header_->numbodyparts)
        throw DeadlyImportError("Model has no bodyparts in mdl!");

    mdl_bodyparts_ = (const mstudiobodyparts_t*)(mdl_buffer_ + header_->bodypartindex);

    //throw DeadlyImportError("nono zone");
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

    vg_header_ = (const VertexGroupHeader_t*)vg_buffer_;

    if(vg_header_->id != 0x47567430)
        throw DeadlyImportError("Unknown VG header id!");
    
    if(vg_header_->version != 1)
        throw DeadlyImportError("Unknown VG header version!");
}

} // namespace HalfLife
} // namespace MDL
} // namespace Respawn