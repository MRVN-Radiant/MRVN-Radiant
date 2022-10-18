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

namespace Titanfall2
{



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
		uint16_t totalRefCount; // Amount of refs referenced by this node + it's children
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
	

	inline std::vector<bspMeshBounds_t> bspMeshBounds;

	inline std::vector<bspEntityPartitions_t> bspEntityPartitions;

	inline std::vector<bspObjReferenceBounds_t> bspObjReferenceBounds;

	inline std::vector<bspObjReferences_t> bspObjReferences;

	inline std::vector<bspLevelInfo_t> bspLevelInfo;

	inline std::vector<uint16_t> bspBrushSidePlanes;

	inline std::vector<char> bspTextureDataData;

	inline std::vector<uint32_t> bspTextureDataTable;

	inline std::vector<bspMaterialSort_t> bspMaterialSorts;

	inline std::vector<bspBrush_t> bspBrushes;

	inline std::vector<CellAABBNode_t> bspCellAABBNodes;

	/* Stubs */

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