#pragma once

#include "bspfile_shared.h"
#include "qmath.h"
#include <cstdint>
#include "remap.h"
#include "lump_names.h"


void LoadR1BSPFile(rbspHeader_t *header, const char *filename);
void WriteR1BSPFile(const char *filename);
void CompileR1BSPFile();


#define MASK_TEXTURE_DATA 0x01FF
#define MASK_DISCARD 0x4000


namespace Titanfall {
    void         SetupGameLump();
    void         BeginModel(entity_t &entity);
    void         EndModel();
    void         EmitEntity(const entity_t &e);
    void         EmitTriggerBrushPlaneKeyValues(entity_t &e);
    void         EmitPlane(const Plane3 &plane);
    void         EmitEntityPartitions();
    uint32_t     EmitTextureData(shaderInfo_t shader);
    uint32_t     EmitVertex(Vector3 &vertex);
    uint32_t     EmitVertexNormal(Vector3 &normal);
    void         EmitVertexUnlit(Shared::Vertex_t &vertex);
    void         EmitVertexLitFlat(Shared::Vertex_t &vertex);
    void         EmitVertexLitBump(Shared::Vertex_t &vertex);
    void         EmitVertexUnlitTS(Shared::Vertex_t &vertex);
    void         EmitVertexBlinnPhong(Shared::Vertex_t &vertex);
    void         EmitMeshes(const entity_t &e);
    std::size_t  EmitObjReferences(Shared::visNode_t &node);
    int          EmitVisChildrenOfTreeNode(Shared::visNode_t node);
    void         EmitVisTree();
    uint16_t     EmitMaterialSort(uint32_t index);
    void         EmitCollisionGrid(entity_t &e);
    void         EmitModelGridCell(entity_t &e);
    void         EmitBrush(brush_t &e);
    void         EmitGeoSet(MinMax minmax, int index, int flags);
    int          EmitUniqueContents(int flags);
    void         EmitLevelInfo();
    void         EmitStubs();
    void         EmitExtraEntity(entity_t &e);


    /* Lump Structs */
    // 0x02
    struct TextureData_t {
        Vector3   reflectivity;
        uint32_t  name_index;
        uint32_t  sizeX;
        uint32_t  sizeY;
        uint32_t  visibleX;
        uint32_t  visibleY;
        uint32_t  flags;
    };

    // 0x0F
    struct Model_t {
        MinMax minmax;
        uint32_t  firstMesh;
        uint32_t  meshCount;
    };

    // 0x23
    // GameLump is the only lump which isn't just a vector
    // The order in which these structs were defined is the order in which both our and the respawn compiler saves them
    struct GameLumpHeader_t {
        uint32_t  version;   // Always 1
        char      ident[4];  // "prps"
        uint32_t  gameConst; // r1: 786432; r2: 851968
        uint32_t  offset;
        uint32_t  length;
    };

    struct GameLumpPathHeader_t {
        uint32_t numPaths;
    };

    struct GameLumpPath_t {
        char path[128];
    };

    struct GameLumpPropHeader_t {
        uint32_t numProps;
        uint32_t unk0;
        uint32_t unk1;
    };

    struct GameLumpProp_t {
        Vector3   origin;
        Vector3   angles;
        float     scale;
        uint16_t  modelName;
        uint8_t   solidMode;
        uint8_t   flags;
        int8_t    unk[4];
        float     fadeScale;
        Vector3   lightingOrigin;
        int8_t    cpuLevel[2];
        int8_t    gpuLevel[2];
        int8_t    diffuseModulation[4];
        uint16_t  collisionFlags[2];
    };

    struct GameLumpUnknownHeader_t {
        uint32_t unknown;
    };

    // 0x42
    struct TricollTriangle_t{
        int32_t unk : 5;
        int32_t idx0 : 9;
        int32_t idx1 : 9;
        int32_t idx2 : 9;
    };

    // 0x45
    struct TricollHeader_t {
        int16_t flags;
        int16_t texinfoFlags;
        int16_t texdata;
        int16_t numVerts;
        int16_t numTris;
        int16_t numBevelIndexes;
        int32_t firstVert;
        int32_t firstTri;
        int32_t firstNode;
        int32_t firstBevelIndex;
        Vector3 org;
        float scale;
    };

    // 0x47
    struct VertexUnlit_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        uint32_t  unknown;
    };

    // 0x48
    struct VertexLitFlat_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        uint32_t  unknown0;
        uint32_t  unknown1;
        uint32_t  unknown2;
        uint32_t  unknown3;
        uint32_t  unknown4;
    };

    // 0x49
    struct VertexLitBump_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        int32_t   negativeOne;
        Vector2   uv1;
        uint32_t  unk[4];
    };

    // 0x4A
    struct VertexUnlitTS_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        Vector2   uv0;
        uint32_t  unknown0;
        uint32_t  unknown1;
        uint32_t  unknown2;
    };

    // 0x4B
    struct VertexBlinnPhong_t {
        uint32_t  vertexIndex;
        uint32_t  normalIndex;
        uint32_t  unknown0;
        uint32_t  unknown1;
    };

    // 0x50
    struct Mesh_t {
        uint32_t  triOffset;
        uint16_t  triCount;
        uint16_t  firstVertex;
        uint16_t  vertexCount;
        uint16_t  unk0;
        uint32_t  const0;
        uint16_t  unk1;
        uint16_t  unk2;
        uint16_t  unk3;
        uint16_t  materialOffset;
        uint32_t  flags;
    };

    // 0x51
    struct MeshBounds_t {
        Vector3   origin;
        uint32_t  unk0;
        Vector3   extents;
        uint32_t  unk1;
    };

    // 0x52
    struct MaterialSort_t {
        uint16_t  textureData;
        uint16_t  lightmapHeader;
        uint16_t  cubemap;
        uint16_t  unk;
        uint32_t  vertexOffset;
    };

    // 0x55
    struct CMGrid_t {
        float     scale;
        int32_t   xOffset;
        int32_t   yOffset;
        int32_t   xCount;
        int32_t   yCount;
        int32_t   straddleGroupCount;
        uint32_t  brushSidePlaneOffset;
    };

    // 0x56
    struct CMGridCell_t {
        uint16_t  start;
        uint16_t  count;
    };

    // 0x57
    struct CMGeoSet_t {
        uint16_t  straddleGroup;
        uint16_t  primitiveCount;
        uint32_t  uniqueContentsIndex : 8;
        uint32_t  collisionShapeIndex : 16;
        uint32_t  collisionShapeType : 8;
    };

    // 0x59
    struct CMPrimitive_t {
        uint32_t  uniqueContentsIndex : 8;
        uint32_t  collisionShapeIndex : 16;
        uint32_t  collisionShapeType : 8;
    };

    // 0x58 & 0x5A
    struct CMBound_t {
        BasicVector3<int16_t>  origin;
        uint8_t                unknown0;
        uint8_t                unknown1;
        BasicVector3<int16_t>  extents;
        uint8_t                unknown2;
        uint8_t                unknown3;
    };

    // 0x5C
    struct CMBrush_t {
        Vector3   origin;
        uint8_t   unknown;
        uint8_t   planeCount;
        uint16_t  index;
        Vector3   extents;
        uint32_t  sidePlaneIndex;
    };

    // 0x77
    struct CellAABBNode_t {
        Vector3   mins;
        uint8_t   childCount;
        uint8_t   objRefCount;
        uint16_t  totalRefCount;
        Vector3   maxs;
        uint16_t  firstChild;
        uint16_t  objRef;
    };

    // 0x7B
    struct LevelInfo_t {
        uint32_t  firstDecalMeshIndex;
        uint32_t  firstTransMeshIndex;
        uint32_t  firstSkyMeshIndex;
        uint32_t  propCount;
        Vector3   unk2;
    };

    // 0x79
    struct ObjReferenceBounds_t {
        Vector3   mins;
        uint32_t  zero0;
        Vector3   maxs;
        uint32_t  zero1;
    };

    namespace Ent {
        inline std::vector<char>      env;
        inline std::vector<char>      fx;
        inline std::vector<char>      script;
        inline std::vector<char>      snd;
        inline std::vector<char>      spawn;
        inline std::vector<char>      extra;
        inline std::vector<entity_t>  entities;
    }

    namespace Bsp {
        inline std::vector<char>                  entities;
        inline std::vector<Plane3f>               planes;
        inline std::vector<TextureData_t>         textureData;
        inline std::vector<Vector3>               vertices;
        inline std::vector<Model_t>               models;
        inline std::vector<char>                  entityPartitions;
        inline std::vector<Vector3>               vertexNormals;
        inline GameLumpHeader_t                   gameLumpHeader;
        inline GameLumpPathHeader_t               gameLumpPathHeader;
        inline std::vector<GameLumpPath_t>        gameLumpPaths;
        inline GameLumpPropHeader_t               gameLumpPropHeader;
        inline std::vector<GameLumpProp_t>        gameLumpProps;
        inline GameLumpUnknownHeader_t            gameLumpUnknownHeader;
        inline std::vector<char>                  textureDataData;
        inline std::vector<uint32_t>              textureDataTable;
        inline std::vector<TricollTriangle_t>     tricollTriangles;
        inline std::vector<TricollHeader_t>       tricollHeaders;
        inline std::vector<VertexUnlit_t>         vertexUnlitVertices;
        inline std::vector<VertexLitFlat_t>       vertexLitFlatVertices;
        inline std::vector<VertexLitBump_t>       vertexLitBumpVertices;
        inline std::vector<VertexUnlitTS_t>       vertexUnlitTSVertices;
        inline std::vector<VertexBlinnPhong_t>    vertexBlinnPhongVertices;
        inline std::vector<uint16_t>              meshIndices;
        inline std::vector<Mesh_t>                meshes;
        inline std::vector<MeshBounds_t>          meshBounds;
        inline std::vector<MaterialSort_t>        materialSorts;
        inline std::vector<CMGrid_t>              cmGrid;
        inline std::vector<CMGridCell_t>          cmGridCells;
        inline std::vector<CMGeoSet_t>            cmGeoSets;
        inline std::vector<CMBound_t>             cmGeoSetBounds;
        inline std::vector<CMPrimitive_t>         cmPrimitives;
        inline std::vector<CMBound_t>             cmPrimitiveBounds;
        inline std::vector<uint16_t>              cmBrushSideProperties;
        inline std::vector<int32_t>               cmUniqueContents;
        inline std::vector<CMBrush_t>             cmBrushes;
        inline std::vector<uint16_t>              cmBrushSidePlaneOffsets;
        inline std::vector<CellAABBNode_t>        cellAABBNodes;
        inline std::vector<uint16_t>              objReferences;
        inline std::vector<ObjReferenceBounds_t>  objReferenceBounds;
        inline std::vector<LevelInfo_t>           levelInfo;  // Only one entry! used a vector to keep load / save code simple

        // Stubs
        inline std::vector<uint8_t>  physicsCollide_stub;
        inline std::vector<uint8_t>  gameLump_stub;
        inline std::vector<uint8_t>  worldLights_stub;
        inline std::vector<uint8_t>  lightmapHeaders_stub;
        inline std::vector<uint8_t>  lightMapDataSky_stub;
        inline std::vector<uint8_t>  csmAABBNodes_stub;
        inline std::vector<uint8_t>  cellBSPNodes_stub;
        inline std::vector<uint8_t>  cells_stub;
    }
}
