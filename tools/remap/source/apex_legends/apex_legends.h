/* -------------------------------------------------------------------------------

   Copyright (C) 2022-2023 MRVN-Radiant and contributors.
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
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ------------------------------------------------------------------------------- */

#pragma once

#include "../titanfall2/titanfall2.h"
#include "qmath.h"
#include <cstdint>
#include "../remap.h"
#include "../lump_names.h"

void LoadR5BSPFile(rbspHeader_t *header, const char *filename);
void WriteR5BSPFile(const char *filename);
void CompileR5BSPFile();

// BVH Node types
#define BVH_CHILD_NODE 0
#define BVH_CHILD_NONE 1

namespace ApexLegends {
    void        EmitStubs();

    void        SetupGameLump();
    void        EmitStaticProp(entity_t &e);
    void        EmitEntity(const entity_t &e);
    void        BeginModel(entity_t &entity);
    void        EndModel();
    void        EmitBVHNode();
    int         EmitBVHDataleaf();
    int         EmitContentsMask( int mask );
    void        EmitMeshes(const entity_t &e);
    uint32_t    EmitTextureData(shaderInfo_t shader);
    uint16_t    EmitMaterialSort(uint32_t index, int offset, int count);
    void        EmitVertexUnlit(Shared::Vertex_t &vertex);
    void        EmitVertexLitFlat(Shared::Vertex_t &vertex);
    void        EmitVertexLitBump(Shared::Vertex_t &vertex);
    void        EmitVertexUnlitTS(Shared::Vertex_t &vertex);
    std::size_t EmitObjReferences(Shared::visNode_t &node);
    int         EmitVisChildrenOfTreeNode(Shared::visNode_t node);
    void        EmitVisTree();
    void        EmitLevelInfo();

    using Vertex_t = Vector3;

    using VertexNormal_t = Vector3;

    // 0x02
    struct TextureData_t {
        uint32_t  surfaceIndex;
        uint32_t  sizeX;
        uint32_t  sizeY;
        uint32_t  flags;
    };

    // 0x0F
    struct Model_t {
        MinMax    minmax;
        int32_t   meshIndex;
        int32_t   meshCount;
        int32_t   bvhNodeIndex;
        int32_t   bvhLeafIndex;
        int32_t   vertexIndex;
        int32_t   vertexFlags;
        float     unk0;
        float     unk1;
        float     unk2;
        int32_t   unk3;
    };

    // 0x12
    struct BVHNode_t {
        int16_t   bounds[24];

        int32_t   cmIndex : 8;
        int32_t   index0 : 24;

        int32_t   padding : 8;
        int32_t   index1 : 24;

        int32_t   childType0 : 4;
        int32_t   childType1 : 4;
        int32_t   index2 : 24;

        int32_t   childType2 : 4;
        int32_t   childType3 : 4;
        int32_t   index3 : 24;
    };

    // 0x47
    struct VertexUnlit_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        int32_t   negativeOne;
    };

    // 0x48
    struct VertexLitFlat_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        int32_t   unknown0;
    };

    // 0x49
    struct VertexLitBump_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        int32_t   negativeOne;
        Vector2   uv1;
        uint8_t   color[4];
    };

    // 0x4A
    struct VertexUnlitTS_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        int32_t   unknown0;
        int32_t   unknown1;
    };

    // 0x4B
    struct VertexBlinnPhong_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        Vector2   uv1;
    };

    // 0x50
    struct Mesh_t {
        uint32_t  triOffset;
        uint16_t  triCount;
        uint16_t  unknown[8];
        uint16_t  materialOffset;
        uint32_t  flags;
    };

    // 0x52
    struct MaterialSort_t {
        uint16_t  textureData;
        int16_t  lightmapIndex;
        uint16_t  unknown0;
        uint16_t  unknown1;
        uint32_t  vertexOffset;
    };

    // 0x77
    struct CellAABBNode_t {
        Vector3   mins;
        uint32_t  childCount : 8;
        uint32_t  firstChild : 16;
        uint32_t  childFlags : 8;
        Vector3   maxs;
        uint32_t  objRefCount : 8;
        uint32_t  objRefOffset : 16;
        uint32_t  objRefFlags : 8;
    };

    // 0x7B
    struct LevelInfo_t {
        int32_t  unk0;
        int32_t  unk1;
        int32_t  unk2;
        int32_t  unk3;
        int32_t  unk4;
        float    unk5[3];
        int32_t  modelCount;
    };


    // GameLump Stub
    struct GameLump_Stub_t {
        uint32_t  version = 1;
        char      magic[4];
        uint32_t  const0 = 3080192;
        uint32_t  offset;
        uint32_t  length = 20;
        uint32_t  zeros[5] = {0, 0, 0, 0, 0};
    };

    namespace Bsp {
        inline std::vector<TextureData_t>       textureData;
        inline std::vector<Model_t>             models;
        inline std::vector<int32_t>             contentsMasks;
        inline std::vector<BVHNode_t>           bvhNodes;
        inline std::vector<int32_t>             bvhLeafDatas;
        inline std::vector<VertexUnlit_t>       vertexUnlitVertices;
        inline std::vector<VertexLitFlat_t>     vertexLitFlatVertices;
        inline std::vector<VertexLitBump_t>     vertexLitBumpVertices;
        inline std::vector<VertexUnlitTS_t>     vertexUnlitTSVertices;
        inline std::vector<VertexBlinnPhong_t>  vertexBlinnPhongVertices;
        inline std::vector<uint16_t>            meshIndices;
        inline std::vector<Mesh_t>              meshes;
        inline std::vector<MaterialSort_t>      materialSorts;
        inline std::vector<CellAABBNode_t>      cellAABBNodes;
        inline std::vector<int32_t>             objReferences;
        inline std::vector<LevelInfo_t>         levelInfo;

        // Stubs
        inline std::vector<uint8_t>  lightprobeParentInfos_stub;
        inline std::vector<uint8_t>  shadowEnvironments_stub;
        inline std::vector<uint8_t>  surfaceProperties_stub;
        inline std::vector<uint8_t>  unknown25_stub;
        inline std::vector<uint8_t>  unknown26_stub;
        inline std::vector<uint8_t>  unknown27_stub;
        inline std::vector<uint8_t>  cubemaps_stub;
        inline std::vector<uint8_t>  worldLights_stub;
        inline std::vector<uint8_t>  lightmapHeaders_stub;
        inline std::vector<uint8_t>  tweakLights_stub;
        inline std::vector<uint8_t>  lightmapDataSky_stub;
        inline std::vector<uint8_t>  csmAABBNodes_stub;
        inline std::vector<uint8_t>  lightmapDataRTLPage_stub;
    }
}
