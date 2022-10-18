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

	inline GameLump_t GameLump;
}