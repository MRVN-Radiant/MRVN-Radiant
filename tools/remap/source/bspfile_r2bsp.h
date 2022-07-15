#pragma once

#include "qmath.h"
#include <cstdint>

void						WriteR2EntFiles(const char* filename);


void                        LoadR2BSPFile(const char* filename);
void                        WriteR2BSPFile(const char* filename);
void						CompileR2BSPFile();

/* -------------------------------------------------------------------------------

   abstracted bsp globals

   ------------------------------------------------------------------------------- */

namespace r2
{
	using bspVertex_t = Vector3;

	using bspVertexNormal_t = Vector3;

	using bspMeshIndex_t = uint16_t;

	using bspPlane_t = Plane3f;


	struct bspVertexLitBump_t
	{
		uint32_t vertexIndex;
		uint32_t normalIndex;
		Vector2 uv0;
		int32_t minusOne;
		Vector2 uv1;
		uint32_t unk[4];
	};


	struct bspMesh_t
	{
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

	/* From testing on r2 the whole lump can be zeroed as long as its parallel with bspMeshes */
	struct bspMeshBounds_t
	{
		Vector3 origin;
		uint32_t unk0;
		Vector3 extents;
		uint32_t unk1;
	};

	struct bspEntityPartitions_t
	{
		char partitions[27];
	};

	struct bspModel_t
	{
		MinMax minmax;
		uint32_t firstMesh;
		uint32_t meshCount;
	};

	struct bspObjReferenceBounds_t
	{
		Vector3 mins;
		uint32_t zero0;
		Vector3 maxs;
		uint32_t zero1;
	};

	using bspObjReferences_t = uint16_t;

	struct bspLevelInfo_t
	{
		uint32_t unk0;
		uint32_t unk1;
		uint32_t unk3;
		uint32_t propCount;
		Vector3 unk2;
	};

	struct CellAABBNode_t
	{
		Vector3 mins;
		uint8_t childCount; // number of direct children this node has
		uint8_t objRefCount; // number of obj_refs it indexes
		uint16_t totalChildCount; // nuber of children all direct children have
		Vector3 maxs;
		uint16_t firstChild; // first child index
		uint16_t objRef; // first obj_ref index
	};

	struct GameLump_Path
	{
		char path[128];
	};

	struct GameLump_Prop
	{
		Vector3 origin;
		Vector3 angles;
		float scale;
		uint16_t modelName;
		uint8_t solidMode;
		uint8_t flags;
		int8_t unk[4];
		float fadeScale;
		Vector3 lightingOrigin;
		int8_t cpuLevel[2];
		int8_t gpuLevel[2];
		int8_t diffuseModulation[4];
		uint16_t collisionFlags[2];
	};

	struct GameLump_t
	{
		uint32_t version; // 1
		char magic[4]; // "prps"
		uint32_t const0; // always 851968
		uint32_t offset; // offset to path_count
		uint32_t length;
		uint32_t pathCount;
		std::vector<GameLump_Path> paths;
		uint32_t propCount;
		uint32_t unk3; // From testing can be same as prop_count
		uint32_t unk4; // Same for this one
		std::vector<GameLump_Prop> props;
		uint32_t unk5;
	};

	struct bspBrush_t
	{
		Vector3 origin;
		uint16_t flags;
		uint16_t index;
		Vector3 extents;
		uint32_t sidePlane;
	};

	struct bspMaterialSort_t
	{
		uint16_t textureData;
		uint16_t lightmapHeader;
		uint16_t cubemap;
		uint16_t unk;
		uint32_t vertexOffset;
	};

	struct bspTextureData_t
	{
		Vector3 reflectivity;
		uint32_t name_index;
		uint32_t sizeX;
		uint32_t sizeY;
		uint32_t visibleX;
		uint32_t visibleY;
		uint32_t flags;
	};
	
	inline std::vector<char> bspEntities;

	inline std::vector<bspTextureData_t> bspTextureData;

	inline std::vector<bspVertex_t> bspVertices;

	inline std::vector<bspVertexNormal_t> bspVertexNormals;

	inline std::vector<bspVertexLitBump_t> bspVertexLitBump;

	inline std::vector<bspMeshIndex_t> bspMeshIndices;

	inline std::vector<bspMesh_t> bspMeshes;

	inline std::vector<bspMeshBounds_t> bspMeshBounds;

	inline std::vector<bspEntityPartitions_t> bspEntityPartitions;

	inline std::vector<bspModel_t> bspModels;

	inline std::vector<bspObjReferenceBounds_t> bspObjReferenceBounds;

	inline std::vector<bspObjReferences_t> bspObjReferences;

	inline std::vector<bspLevelInfo_t> bspLevelInfo;

	inline std::vector<uint16_t> bspBrushSidePlanes;

	inline std::vector<char> bspTextureDataData;

	inline std::vector<uint32_t> bspTextureDataTable;

	inline std::vector<bspMaterialSort_t> bspMaterialSorts;

	inline std::vector<bspPlane_t> bspPlanes;

	inline std::vector<bspBrush_t> bspBrushes;

	/* Stubs */
	inline std::vector<char> bspEntities_stub;

	inline std::vector<uint8_t> bspPlanes_stub;

	inline std::vector<uint8_t> bspTextureData_stub;

	inline std::vector<uint8_t> bspWorldLights_stub;

	inline std::vector<uint8_t> bspTricollTris_stub;

	inline std::vector<uint8_t> bspTricollNodes_stub;

	inline std::vector<uint8_t> bspTricollHeaders_stub;

	inline std::vector<uint8_t> bspLightMapHeaders_stub;

	inline std::vector<uint8_t> bspCMGrid_stub;

	inline std::vector<uint8_t> bspCMGridCells_stub;

	inline std::vector<uint8_t> bspCMGridSets_stub;

	inline std::vector<uint8_t> bspCMGeoSetBounds_stub;

	inline std::vector<uint8_t> bspCMPrimitives_stub;

	inline std::vector<uint8_t> bspCMPrimitiveBounds_stub;

	inline std::vector<uint8_t> bspCMUniqueContents_stub;

	inline std::vector<uint8_t> bspCMBrushes_stub;

	inline std::vector<uint8_t> bspCMBrushSidePlaneOffsets_stub;

	inline std::vector<uint8_t> bspCMBrushSideProps_stub;

	inline std::vector<uint8_t> bspCMBrushTexVecs_stub;

	inline std::vector<uint8_t> bspTricollBevelStarts_stub;

	inline std::vector<uint8_t> bspLightMapDataSky_stub;

	inline std::vector<uint8_t> bspCSMAABBNodes_stub;

	inline std::vector<uint8_t> bspCellBSPNodes_stub;

	inline std::vector<uint8_t> bspCells_stub;

	inline std::vector<CellAABBNode_t> bspCellAABBNodes_stub;

	inline GameLump_t GameLump;
}