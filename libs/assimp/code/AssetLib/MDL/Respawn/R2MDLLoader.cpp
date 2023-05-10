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

#include "R2MDLLoader.h"

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
namespace RespawnR2 {

// ------------------------------------------------------------------------------------------------
R2MDLLoader::R2MDLLoader(
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
    vtx_buffer_(nullptr),
    vvd_buffer_(nullptr) {
    load_mdl_file();
    parse_mdl_file();
}

// ------------------------------------------------------------------------------------------------
R2MDLLoader::~R2MDLLoader() {
    release_resources();
}

// ------------------------------------------------------------------------------------------------
void R2MDLLoader::release_resources() {
}

// ------------------------------------------------------------------------------------------------
void R2MDLLoader::parse_mdl_file() {
    if(header_->numbodyparts != vtx_header_->numBodyParts)
        throw DeadlyImportError("MDL and VTX num bodyparts mismatch!");
    
    // Craete aiMaterials we want to use
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
    int iCurrentMesh = 0;
    for(int i = 0; i < vtx_header_->numBodyParts; i++) {
        const BodyPartHeader_t *vtx_bodypart = (const BodyPartHeader_t *)(vtx_buffer_ + vtx_header_->bodyPartOffset + sizeof(BodyPartHeader_t) * i);
        for(int j = 0; j < vtx_bodypart->numModels; j++) {
            const ModelHeader_t *vtx_model = (const ModelHeader_t *)((const char *)vtx_bodypart + vtx_bodypart->modelOffset + sizeof(ModelHeader_t) * j);
            for(int k = 0; k < 1 /*vtx_model->numLODs*/; k++) {
                const ModelLODHeader_t *vtx_lod_header = (const ModelLODHeader_t *)((const char *)vtx_model + vtx_model->lodOffset + sizeof(ModelLODHeader_t) * k);
                for(int l = 0; l < vtx_lod_header->numMeshes; l++) {
                    const MeshHeader_t *mesh = (const MeshHeader_t *)((const char *)vtx_lod_header + vtx_lod_header->meshOffset + sizeof(MeshHeader_t) * l);
                    iCurrentMesh += mesh->numStripGroups;
                }
            }
        }
    }

    // Allocate space for our meshes
    ASSIMP_LOG_DEBUG("NumMeshes: ", iCurrentMesh);
    scene_->mNumMeshes = iCurrentMesh;
    scene_->mMeshes = new aiMesh *[iCurrentMesh];
    iCurrentMesh = 0;

    // Build the scene tree and build the meshes it uses
    aiNode *pRootNode = new aiNode();
    pRootNode->mNumChildren = vtx_header_->numBodyParts;
    pRootNode->mChildren = new aiNode *[vtx_header_->numBodyParts];
    scene_->mRootNode = pRootNode;
    // Populate the meshes
    for(int i = 0; i < vtx_header_->numBodyParts; i++) {
        const BodyPartHeader_t *vtx_bodypart = (const BodyPartHeader_t *)(vtx_buffer_ + vtx_header_->bodyPartOffset + sizeof(BodyPartHeader_t) * i);
        const mstudiobodyparts_t *mdl_bodypart = (const mstudiobodyparts_t *)(mdl_buffer_ + header_->bodypartindex + sizeof(mstudiobodyparts_t) * i);

        aiNode *pBodyPartNode = new aiNode();
        pBodyPartNode->mNumChildren = vtx_bodypart->numModels;
        pBodyPartNode->mChildren = new aiNode *[vtx_bodypart->numModels];
        pRootNode->mChildren[i] = pBodyPartNode;
        pBodyPartNode->mParent = pRootNode;

        for(int j = 0; j < vtx_bodypart->numModels; j++) {
            const ModelHeader_t *vtx_model = (const ModelHeader_t *)((const char *)vtx_bodypart + vtx_bodypart->modelOffset + sizeof(ModelHeader_t) * j);
            const mstudiomodel_t *mdl_model = (const mstudiomodel_t *)((const char*)mdl_bodypart + mdl_bodypart->modelindex + sizeof(mstudiomodel_t) * j);

            aiNode *pModelNode = new aiNode();
            pModelNode->mNumChildren = 1 /*vtx_model->numLODs*/;
            pModelNode->mChildren = new aiNode *[1 /*vtx_model->numLODs*/];
            pBodyPartNode->mChildren[j] = pModelNode;
            pModelNode->mParent = pBodyPartNode;

            for(int k = 0; k < 1 /*vtx_model->numLODs*/; k++) {
                const ModelLODHeader_t *vtx_lod_header = (const ModelLODHeader_t *)((const char *)vtx_model + vtx_model->lodOffset + sizeof(ModelLODHeader_t) * k);
                
                aiNode *pModelLODHeaderNode = new aiNode();
                pModelLODHeaderNode->mNumChildren = vtx_lod_header->numMeshes;
                pModelLODHeaderNode->mChildren = new aiNode *[vtx_lod_header->numMeshes];
                pModelNode->mChildren[k] = pModelLODHeaderNode;
                pModelLODHeaderNode->mParent = pModelNode;
                
                for(int l = 0; l < vtx_lod_header->numMeshes; l++) {
                    const MeshHeader_t *vtx_mesh = (const MeshHeader_t *)((const char *)vtx_lod_header + vtx_lod_header->meshOffset + sizeof(MeshHeader_t) * l);
                    const mstudiomesh_t *mdl_mesh = (const mstudiomesh_t*)((const char*)mdl_model + mdl_model->meshindex + sizeof(mstudiomesh_t) * l);
                    
                    aiNode *pMeshNode = new aiNode();
                    pModelLODHeaderNode->mChildren[l] = pMeshNode;
                    pMeshNode->mParent = pModelLODHeaderNode;
                    pMeshNode->mNumMeshes = vtx_mesh->numStripGroups;
                    pMeshNode->mMeshes = new unsigned int[vtx_mesh->numStripGroups];

                    for(int m = 0; m < vtx_mesh->numStripGroups; m++) {
                        const StripGroupHeader_t *stripGroupHeader = (const StripGroupHeader_t *)((const char *)vtx_mesh + vtx_mesh->stripGroupHeaderOffset + sizeof(StripGroupHeader_t) * m);
                        pMeshNode->mMeshes[m] = iCurrentMesh;

                        // Build te aiMesh
                        aiMesh *pMesh = new aiMesh();

                        pMesh->mMaterialIndex = mdl_mesh->material;

                        // Build vertices
                        pMesh->mNumVertices = stripGroupHeader->numVerts;
                        pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
                        pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
                        pMesh->mTextureCoords[0] = new aiVector3D[pMesh->mNumVertices];
                        pMesh->mNumUVComponents[0] = 2;
                        for(unsigned int v = 0; v < pMesh->mNumVertices; v++) {
                            const Vertex_t *vert = (const Vertex_t *)((const char *)stripGroupHeader + stripGroupHeader->vertOffset + sizeof(Vertex_t) * v);
                            const mstudiovertex_t *stdioVert = reinterpret_cast<const mstudiovertex_t* const>((char*)vertices_ + mdl_model->vertexindex + ((mdl_mesh->vertexoffset + vert->origMeshVertID) * sizeof(mstudiovertex_t)));
                            pMesh->mVertices[v] = aiVector3D(stdioVert->m_vecPosition[0], stdioVert->m_vecPosition[1], stdioVert->m_vecPosition[2]);
                            pMesh->mNormals[v] = aiVector3D(stdioVert->m_vecNormal[0], stdioVert->m_vecNormal[1], stdioVert->m_vecNormal[2]);
                            pMesh->mTextureCoords[0][v] = aiVector3D(stdioVert->m_vecTexCoord[0], stdioVert->m_vecTexCoord[1], 0.0);
                        }

                        // Build faces
                        pMesh->mNumFaces = stripGroupHeader->numIndices / 3;
                        pMesh->mFaces = new aiFace[pMesh->mNumFaces];
                        const uint16_t *indices = (const uint16_t *)((const char *)stripGroupHeader + stripGroupHeader->indexOffset);
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
void R2MDLLoader::load_mdl_file() {
    header_ = (const studiohdr_t *)mdl_buffer_;

    if(!header_->numbodyparts)
        throw DeadlyImportError("Model has no bodyparts in mdl!");

    mdl_bodyparts_ = (const mstudiobodyparts_t*)(mdl_buffer_ + header_->bodypartindex);

    vtx_buffer_ = (const unsigned char *)(mdl_buffer_ + header_->vtxindex);

    vtx_header_ = (const FileHeader_t *)vtx_buffer_;

    if(vtx_header_->version != 7)
        throw DeadlyImportError("Unknown VTX header version!");

    vvd_buffer_ = (const unsigned char *)(mdl_buffer_ + header_->vvdindex);

    vvd_header_ = (const vertexFileHeader_t *)vvd_buffer_;

    //ASSIMP_LOG_DEBUG("VVD id: ", vvd_header_->id);
    if(vvd_header_->id != 1448297545)
        throw DeadlyImportError("Unknown VVD header id!");
    
    if(vvd_header_->version != 4)
        throw DeadlyImportError("Unknown VVD header version!");
    
    vertices_ = (const mstudiovertex_t *)(vvd_buffer_ + vvd_header_->vertexDataStart);
    tangents_ = (const vec4_t *)(vvd_buffer_ + vvd_header_->tangentDataStart);
}

} // namespace HalfLife
} // namespace MDL
} // namespace Respawn