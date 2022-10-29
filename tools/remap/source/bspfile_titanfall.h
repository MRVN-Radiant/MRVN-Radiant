#pragma once

#include "qmath.h"
#include <cstdint>
#include "remap.h"

void                        LoadR1BSPFile(const char* filename);
void                        WriteR1BSPFile(const char* filename);
void						CompileR1BSPFile();


namespace Titanfall {
	void EmitEntity( const entity_t &e );
	void EmitPlane( const Plane3 &plane );
	uint32_t EmitTextureData( shaderInfo_t shader );
	uint32_t EmitVertex( Vector3 &vertex );
	uint32_t EmitVertexNormal( Vector3 &normal );
	void BeginModel();
	void EndModel();
	void EmitEntityPartitions();
	void EmitMeshes( const entity_t &e );
	void EmitBrushes( const entity_t &e );
	void EmitCollisionGrid();
	uint16_t EmitMaterialSort( uint32_t index );
	void EmitLevelInfo();
	void EmitStubs();

	// 0x02
	struct TextureData_t {
		Vector3 reflectivity;
		uint32_t name_index;
		uint32_t sizeX;
		uint32_t sizeY;
		uint32_t visibleX;
		uint32_t visibleY;
		uint32_t flags;
	};

	// 0x0F
	struct Model_t {
		MinMax minmax;
		uint32_t firstMesh;
		uint32_t meshCount;
	};

	// 0x49
	struct VertexLitBump_t {
		uint32_t vertexIndex;
		uint32_t normalIndex;
		Vector2 uv0;
		int32_t negativeOne;
		Vector2 uv1;
		uint32_t unk[4];
	};

	// 0x50
	struct Mesh_t {
		uint32_t triOffset;
		uint16_t triCount;
		uint16_t firstVertex;
		uint16_t vertexCount;
		uint16_t unk0;
		uint32_t const0;
		uint16_t unk1;
		uint16_t unk2;
		uint16_t unk3;
		uint16_t materialOffset;
		uint32_t flags;
	};

	// 0x51
	struct MeshBounds_t {
		Vector3 origin;
		uint32_t unk0;
		Vector3 extents;
		uint32_t unk1;
	};

	// 0x52
	struct MaterialSort_t {
		uint16_t textureData;
		uint16_t lightmapHeader;
		uint16_t cubemap;
		uint16_t unk;
		uint32_t vertexOffset;
	};

	// 0x55
	struct CMGrid_t {
		float scale;
		int32_t xOffset;
		int32_t yOffset;
		int32_t xCount;
		int32_t yCount;
		int32_t unk2;
		uint32_t brushSidePlaneOffset;
	};

	// 0x56
	struct CMGridCell_t {
		uint16_t start;
		uint16_t count;
	};

	// 0x57
	struct CMGeoSet_t {
		uint16_t unknown0;
		uint16_t unknown1;
		uint32_t unknown2 : 8;
		uint32_t collisionShapeIndex : 16;
		uint32_t collisionShapeType : 8;
	};

	// 0x58 & 0x5A
	struct CMBound_t {
		BasicVector3<uint16_t> origin;
		uint8_t unknown0;
		uint8_t unknown1;
		BasicVector3<uint16_t> extents;
		uint8_t unknown2;
		uint8_t unknown3;
	};

	// 0x5C
	struct CMBrush_t {
		Vector3 origin;
		uint8_t unknown;
		uint8_t planeCount;
		uint16_t index;
		Vector3 extents;
		uint32_t sidePlaneIndex;
	};

	// 0x77
	struct CellAABBNode_t {
		Vector3 mins;
		uint8_t childCount;
		uint8_t objRefCount;
		uint16_t totalRefCount;
		Vector3 maxs;
		uint16_t firstChild;
		uint16_t objRef;
	};

	// 0x7B
	struct LevelInfo_t {
		uint32_t unk0;
		uint32_t unk1;
		uint32_t unk3;
		uint32_t propCount;
		Vector3 unk2;
	};

	// 0x79
	struct ObjReferenceBounds_t {
		Vector3 mins;
		uint32_t zero0;
		Vector3 maxs;
		uint32_t zero1;
	};


	// Gamelump Stub
	struct GameLump_Stub_t {
		uint32_t version = 1;
		char magic[4];
		uint32_t const0 = 786432;
		uint32_t offset;
		uint32_t length = 20;
		uint32_t zeros[5] = {0,0,0,0,0};
	};

	namespace Ent {
		inline std::vector<char> spawn;
	}

	namespace Bsp {
		inline std::vector<char> entities;
		inline std::vector<Plane3f> planes;
		inline std::vector<TextureData_t> textureData;
		inline std::vector<Vector3> vertices;
		inline std::vector<Model_t> models;
		inline std::vector<char> entityPartitions;
		inline std::vector<Vector3> vertexNormals;
		inline std::vector<char> textureDataData;
		inline std::vector<uint32_t> textureDataTable;
		inline std::vector<VertexLitBump_t> vertexLitBumpVertices;
		inline std::vector<uint16_t> meshIndices;
		inline std::vector<Mesh_t> meshes;
		inline std::vector<MeshBounds_t> meshBounds;
		inline std::vector<MaterialSort_t> materialSorts;
		inline std::vector<CMGrid_t> cmGrid;
		inline std::vector<CMGridCell_t> cmGridCells;
		inline std::vector<CMGeoSet_t> cmGeoSets;
		inline std::vector<CMBound_t> cmGeoSetBounds;
		inline std::vector<uint16_t> cmBrushSideProperties;
		inline std::vector<CMBrush_t> cmBrushes;
		inline std::vector<uint16_t> cmBrushSidePlaneOffsets;
		inline std::vector<CellAABBNode_t> cellAABBNodes;
		inline std::vector<uint16_t> objReferences;
		inline std::vector<ObjReferenceBounds_t> objReferenceBounds;
		// Only one entry! used a vector to keep load / save code simple
		inline std::vector<LevelInfo_t> levelInfo;

		// Stubs
		inline std::vector<uint8_t> entities_stub;
		inline std::vector<uint8_t> physicsCollide_stub;
		inline std::vector<uint8_t> gameLump_stub;
		inline std::vector<uint8_t> worldLights_stub;
		inline std::vector<uint8_t> lightmapHeaders_stub;
		inline std::vector<uint8_t> cmUniqueContents_stub;
		inline std::vector<uint8_t> lightMapDataSky_stub;
		inline std::vector<uint8_t> csmAABBNodes_stub;
		inline std::vector<uint8_t> cellBSPNodes_stub;
		inline std::vector<uint8_t> cells_stub;
	}
}