/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r2 ( Titanfall 2 ) .bsp file.

   A lot of this is temporary and will be altered heavily with new information.
   If you know how to c++ better than me feel free to contribute or call me words
   in discord. :)

   - Fifty#8113, also known as Fifteen, Sixteen, Seventeen, Eigtheen and FORTY

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"
#include "bspfile_abstract.h"
#include <ctime>


/* constants */
#define LUMP_ENTITIES							0x00
#define LUMP_PLANES								0x01
#define LUMP_TEXTURE_DATA						0x02
#define LUMP_VERTICES							0x03
#define LUMP_LIGHTPROBE_PARENT_INFOS			0x04
#define LUMP_SHADOW_ENVIRONMENTS				0x05
#define LUMP_LIGHTPROBE_BSP_NODES				0x06
#define LUMP_LIGHTPROBE_BSP_REF_IDS				0x07
#define LUMP_MODELS								0x0E
#define LUMP_ENTITY_PARTITIONS					0x18
#define LUMP_PHYSICS_COLLIDE					0x1D
#define LUMP_VERTEX_NORMALS						0x1E
#define LUMP_GAME_LUMP							0x23
#define LUMP_LEAF_WATER_DATA					0x24
#define LUMP_PAKFILE							0x28
#define LUMP_CUBEMAPS							0x2A
#define LUMP_TEXTURE_DATA_STRING_DATA			0x2B
#define LUMP_TEXTURE_DATA_STRING_TABLE			0x2C
#define LUMP_WORLD_LIGHTS						0x36
#define LUMP_WORLD_LIGHT_PARENT_INFOS			0x37
#define LUMP_PHYSICS_LEVEL						0x3E
#define LUMP_TRICOLL_TRIS						0x42
#define LUMP_TRICOLL_NODES						0x44
#define LUMP_TRICOLL_HEADERS					0x45
#define LUMP_PHYSICS_TRIANGLES					0x46
#define LUMP_VERTEX_UNLIT						0x47
#define LUMP_VERTEX_LIT_FLAT					0x48
#define LUMP_VERTEX_LIT_BUMP					0x49
#define LUMP_VERTEX_UNLIT_TS					0x4A
#define LUMP_VERTEX_BLINN_PHONG					0x4B
#define LUMP_VERTEX_RESERVED_5					0x4C
#define LUMP_VERTEX_RESERVED_6					0x4D
#define LUMP_VERTEX_RESERVED_7					0x4E
#define LUMP_MESH_INDICES						0x4F
#define LUMP_MESHES								0x50
#define LUMP_MESH_BOUNDS						0x51
#define LUMP_MATERIAL_SORT						0x52
#define LUMP_LIGHTMAP_HEADERS					0x53
#define LUMP_CM_GRID							0x55
#define LUMP_CM_GRID_CELLS						0x56
#define LUMP_CM_GRID_SETS						0x57
#define LUMP_CM_GEO_SET_BOUNDS					0x58
#define LUMP_CM_PRIMITIVES						0x59
#define LUMP_CM_PRIMITIVE_BOUNDS				0x5A
#define LUMP_CM_UNIQUE_CONTENTS					0x5B
#define LUMP_CM_BRUSHES							0x5C
#define LUMP_CM_BRUSH_SIDE_PLANE_OFFSETS		0x5D
#define LUMP_CM_BRUSH_SIDE_PROPS				0x5E
#define LUMP_CM_BRUSH_TEX_VECS					0x5F
#define LUMP_TRICOLL_BEVEL_STARTS				0x60
#define LUMP_TRICOLL_BEVEL_INDICES				0x61
#define LUMP_LIGHTMAP_DATA_SKY					0x62
#define LUMP_CSM_AABB_NODES						0x63
#define LUMP_CSM_OBJ_REFERENCES					0x64
#define LUMP_LIGHTPROBES						0x65
#define LUMP_STATIC_PROP_LIGHTPROBE_INDICES		0x66
#define LUMP_LIGHTPROBE_TREE					0x67
#define LUMP_LIGHTPROBE_REFERENCES				0x68
#define LUMP_LIGHTMAP_DATA_REAL_TIME_LIGHTS		0x69
#define LUMP_CELL_BSP_NODES						0x6A
#define LUMP_CELLS								0x6B
#define LUMP_PORTALS							0x6C
#define LUMP_PORTAL_VERTICES					0x6D
#define LUMP_PORTAL_EDGES						0x6E
#define LUMP_PORTAL_VERTEX_EDGES				0x6F
#define LUMP_PORTAL_VERTEX_REFERENCES			0x70
#define LUMP_PORTAL_EDGE_REGERENCES				0x71
#define LUMP_PORTAL_EDGE_INTERSECT_EDGE			0x72
#define LUMP_PORTAL_EDGE_INTERSECT_AT_VERTEX	0x73
#define LUMP_PORTAL_EDGE_INTERSECT_HEADER		0x74
#define LUMP_OCCLUSION_MESH_VERTICES			0x75
#define LUMP_OCCLUSION_MESH_INDICES				0x76
#define LUMP_CELL_AABB_NODES					0x77
#define LUMP_OBJ_REFERENCES						0x78
#define LUMP_OBJ_REFERENCE_BOUNDS				0x79
#define LUMP_LIGHTMAP_DATA_RTL_PAGE				0x7A
#define LUMP_LEVEL_INFO							0x7B
#define LUMP_SHADOW_MESH_OPAQUE_VERTICES		0x7C
#define LUMP_SHADOW_MESH_ALPHA_VERTICES			0x7D
#define LUMP_SHADOW_MESH_INDICES				0x7E




/* funcs */
/*
   WriteR2EntFiles()
   writes the titanfall2 .ent files
 */
void WriteR2EntFiles(const char* filename)
{
	Sys_Printf("ents\n");
	
	auto path = StringOutputStream(256)(PathExtensionless(filename));
	
	/* _env */
	{
		auto name = StringOutputStream(256)(PathExtensionless(path), "_env.ent");
		FILE* file = SafeOpenWrite(name);
		char message[] = "ENTITIES01\n";
		SafeWrite(file, &message, sizeof(message));
		fclose(file);
	}

	/* _fx */
	{
		auto name = StringOutputStream(256)(PathExtensionless(path), "_fx.ent");
		FILE* file = SafeOpenWrite(name);
		char message[] = "ENTITIES01\n";
		SafeWrite(file, &message, sizeof(message));
		fclose(file);
	}

	/* _script */
	{
		auto name = StringOutputStream(256)(PathExtensionless(path), "_script.ent");
		FILE* file = SafeOpenWrite(name);
		char message[] = "ENTITIES01\n";
		SafeWrite(file, &message, sizeof(message));
		fclose(file);
	}

	/* _snd */
	{
		auto name = StringOutputStream(256)(PathExtensionless(path), "_snd.ent");
		FILE* file = SafeOpenWrite(name);
		char message[] = "ENTITIES01\n";
		SafeWrite(file, &message, sizeof(message));
		fclose(file);
	}

	/* _spawn */
	{
		auto name = StringOutputStream(256)(PathExtensionless(path), "_spawn.ent");
		FILE* file = SafeOpenWrite(name);
		/* this is just 2 dropship spawns at <0,0,0> */
		constexpr std::array<uint8_t, 821> data = {
			0x45, 0x4E, 0x54, 0x49, 0x54, 0x49, 0x45, 0x53, 0x30, 0x31, 0x0A, 0x7B, 0x0A, 0x22, 0x6D, 0x6F,
			0x64, 0x65, 0x6C, 0x22, 0x20, 0x22, 0x6D, 0x6F, 0x64, 0x65, 0x6C, 0x73, 0x2F, 0x64, 0x65, 0x76,
			0x2F, 0x6D, 0x70, 0x5F, 0x64, 0x72, 0x6F, 0x70, 0x73, 0x68, 0x69, 0x70, 0x5F, 0x73, 0x70, 0x61,
			0x77, 0x6E, 0x5F, 0x73, 0x74, 0x61, 0x72, 0x74, 0x2E, 0x6D, 0x64, 0x6C, 0x22, 0x0A, 0x22, 0x67,
			0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x74, 0x65, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A,
			0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x74, 0x64, 0x6D, 0x22, 0x20, 0x22,
			0x31, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x73, 0x75, 0x72,
			0x76, 0x69, 0x76, 0x61, 0x6C, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65,
			0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x6C, 0x74, 0x73, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x67,
			0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x65, 0x78, 0x66, 0x69, 0x6C, 0x22, 0x20, 0x22,
			0x31, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x63, 0x74, 0x74,
			0x22, 0x20, 0x22, 0x30, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F,
			0x63, 0x74, 0x66, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F,
			0x64, 0x65, 0x5F, 0x63, 0x70, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65,
			0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x62, 0x62, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A, 0x22, 0x67, 0x61,
			0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x61, 0x74, 0x64, 0x6D, 0x22, 0x20, 0x22, 0x30, 0x22,
			0x0A, 0x22, 0x73, 0x63, 0x61, 0x6C, 0x65, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x61, 0x6E,
			0x67, 0x6C, 0x65, 0x73, 0x22, 0x20, 0x22, 0x2D, 0x30, 0x20, 0x39, 0x30, 0x20, 0x30, 0x22, 0x0A,
			0x22, 0x6F, 0x72, 0x69, 0x67, 0x69, 0x6E, 0x22, 0x20, 0x22, 0x30, 0x20, 0x2D, 0x30, 0x20, 0x30,
			0x22, 0x0A, 0x22, 0x74, 0x65, 0x61, 0x6D, 0x6E, 0x75, 0x6D, 0x22, 0x20, 0x22, 0x32, 0x22, 0x0A,
			0x22, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x6E, 0x61, 0x6D, 0x65, 0x22, 0x20, 0x22, 0x69, 0x6E,
			0x66, 0x6F, 0x5F, 0x73, 0x70, 0x61, 0x77, 0x6E, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x5F, 0x64, 0x72,
			0x6F, 0x70, 0x73, 0x68, 0x69, 0x70, 0x5F, 0x73, 0x74, 0x61, 0x72, 0x74, 0x5F, 0x31, 0x22, 0x0A,
			0x22, 0x63, 0x6C, 0x61, 0x73, 0x73, 0x6E, 0x61, 0x6D, 0x65, 0x22, 0x20, 0x22, 0x69, 0x6E, 0x66,
			0x6F, 0x5F, 0x73, 0x70, 0x61, 0x77, 0x6E, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x5F, 0x64, 0x72, 0x6F,
			0x70, 0x73, 0x68, 0x69, 0x70, 0x5F, 0x73, 0x74, 0x61, 0x72, 0x74, 0x22, 0x0A, 0x7D, 0x0A, 0x7B,
			0x0A, 0x22, 0x6D, 0x6F, 0x64, 0x65, 0x6C, 0x22, 0x20, 0x22, 0x6D, 0x6F, 0x64, 0x65, 0x6C, 0x73,
			0x2F, 0x64, 0x65, 0x76, 0x2F, 0x6D, 0x70, 0x5F, 0x64, 0x72, 0x6F, 0x70, 0x73, 0x68, 0x69, 0x70,
			0x5F, 0x73, 0x70, 0x61, 0x77, 0x6E, 0x5F, 0x73, 0x74, 0x61, 0x72, 0x74, 0x2E, 0x6D, 0x64, 0x6C,
			0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x74, 0x65, 0x22, 0x20,
			0x22, 0x30, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x74, 0x64,
			0x6D, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65,
			0x5F, 0x73, 0x75, 0x72, 0x76, 0x69, 0x76, 0x61, 0x6C, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A, 0x22,
			0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x6C, 0x74, 0x73, 0x22, 0x20, 0x22, 0x31,
			0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x65, 0x78, 0x66, 0x69,
			0x6C, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65,
			0x5F, 0x63, 0x74, 0x74, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D,
			0x6F, 0x64, 0x65, 0x5F, 0x63, 0x74, 0x66, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x67, 0x61,
			0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x63, 0x70, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22,
			0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x62, 0x62, 0x22, 0x20, 0x22, 0x30, 0x22,
			0x0A, 0x22, 0x67, 0x61, 0x6D, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x61, 0x74, 0x64, 0x6D, 0x22,
			0x20, 0x22, 0x30, 0x22, 0x0A, 0x22, 0x73, 0x63, 0x61, 0x6C, 0x65, 0x22, 0x20, 0x22, 0x31, 0x22,
			0x0A, 0x22, 0x61, 0x6E, 0x67, 0x6C, 0x65, 0x73, 0x22, 0x20, 0x22, 0x2D, 0x30, 0x20, 0x2D, 0x39,
			0x30, 0x20, 0x30, 0x22, 0x0A, 0x22, 0x6F, 0x72, 0x69, 0x67, 0x69, 0x6E, 0x22, 0x20, 0x22, 0x2D,
			0x30, 0x20, 0x30, 0x20, 0x30, 0x22, 0x0A, 0x22, 0x74, 0x65, 0x61, 0x6D, 0x6E, 0x75, 0x6D, 0x22,
			0x20, 0x22, 0x33, 0x22, 0x0A, 0x22, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x6E, 0x61, 0x6D, 0x65,
			0x22, 0x20, 0x22, 0x69, 0x6E, 0x66, 0x6F, 0x5F, 0x73, 0x70, 0x61, 0x77, 0x6E, 0x70, 0x6F, 0x69,
			0x6E, 0x74, 0x5F, 0x64, 0x72, 0x6F, 0x70, 0x73, 0x68, 0x69, 0x70, 0x5F, 0x73, 0x74, 0x61, 0x72,
			0x74, 0x5F, 0x32, 0x22, 0x0A, 0x22, 0x63, 0x6C, 0x61, 0x73, 0x73, 0x6E, 0x61, 0x6D, 0x65, 0x22,
			0x20, 0x22, 0x69, 0x6E, 0x66, 0x6F, 0x5F, 0x73, 0x70, 0x61, 0x77, 0x6E, 0x70, 0x6F, 0x69, 0x6E,
			0x74, 0x5F, 0x64, 0x72, 0x6F, 0x70, 0x73, 0x68, 0x69, 0x70, 0x5F, 0x73, 0x74, 0x61, 0x72, 0x74,
			0x22, 0x0A, 0x7D, 0x0A, 0x00
		};
		SafeWrite(file, &data, sizeof(data));
		fclose(file);
	}
}

/*
   LoadR2BSPFile()
   loads a titanfall2 bsp file
 */
void LoadR2BSPFile(const char* filename)
{
	Sys_FPrintf(SYS_VRB, "cock\n");
	/* load the file */
	MemBuffer file = LoadFile(filename);

	rbspHeader_t* header = file.data();

	/* swap the header(except the first 4 bytes) */
	SwapBlock((int*)((byte*)header + 4), sizeof(*header) - 4);

	/* make sure it matches the format we're trying to load */
	if (!force && memcmp(header->ident, g_game->bspIdent, 4)) {
		Error("%s is not a %s file", filename, g_game->bspIdent);
	}
	if (!force && header->version != g_game->bspVersion) {
		Error("%s is version %d, not %d", filename, header->version, g_game->bspVersion);
	}

	/*
		Load lumps
		We only load lumps we can use for conversion to .map
		I dont plan on supporting bsp merging, shifting, ...
	*/
	CopyLump( (rbspHeader_t*) header, LUMP_PLANES, bspPlanes );
	CopyLump( (rbspHeader_t*) header, LUMP_CM_BRUSHES, bspBrushes );
	CopyLump( (rbspHeader_t*) header, LUMP_CM_BRUSH_SIDE_PLANE_OFFSETS, bspBrushSidePlanes);

}


/*
   WriteR2BSPFile()
   writes a titanfall2 bsp file and it's .ent files
 */
void WriteR2BSPFile(const char* filename)
{
	rbspHeader_t header{};


	/* set up header */
	memcpy(header.ident, g_game->bspIdent, 4);
	header.version = LittleLong(g_game->bspVersion);
	header.mapVersion = 30;
	header.maxLump = 127;

	/* This should be automated maybe */
	header.lumps[0x01].lumpVer = 1;
	header.lumps[0x02].lumpVer = 1;
	header.lumps[0x36].lumpVer = 3;
	header.lumps[0x42].lumpVer = 2;
	header.lumps[0x44].lumpVer = 1;
	header.lumps[0x45].lumpVer = 1;
	header.lumps[0x49].lumpVer = 1;
	header.lumps[0x53].lumpVer = 1;

	/* write initial header */
	FILE* file = SafeOpenWrite(filename);
	SafeWrite(file, &header, sizeof(header));    /* overwritten later */


	/* :) */
	{
		char message[64] = "Built with love using r2radiant :)";
		SafeWrite(file, &message, sizeof(message));
	}
	{
		char message[64];
		strncpy(message,StringOutputStream(64)("Version:        ", Q3MAP_VERSION).c_str(),64);
		SafeWrite(file, &message, sizeof(message));
	}
	{
		time_t t;
		time(&t);
		char message[64];
		strncpy(message,StringOutputStream(64)("Time:           ", asctime(localtime(&t))).c_str(),64);
		SafeWrite(file, &message, sizeof(message));
	}
	
	/* Write lumps */
	AddLump(file, header.lumps[LUMP_ENTITIES],							bspEntities_stub);
	AddLump(file, header.lumps[LUMP_PLANES],							bspPlanes_stub);
	AddLump(file, header.lumps[LUMP_TEXTURE_DATA],						bspTextureData);
	AddLump(file, header.lumps[LUMP_VERTICES],							bspVertices);
	AddLump(file, header.lumps[LUMP_MODELS],							bspModels_new);
	AddLump(file, header.lumps[LUMP_VERTEX_NORMALS],					bspVertexNormals);
	AddLump(file, header.lumps[LUMP_ENTITY_PARTITIONS],					bspEntityPartitions);
	/* Game Lump */
	{
		std::size_t start = ftell(file);
		header.lumps[LUMP_GAME_LUMP].offset = start;
		header.lumps[LUMP_GAME_LUMP].length = 36 + GameLump.path_count * sizeof(GameLump_Path) + GameLump.prop_count * sizeof(GameLump_Prop);
		GameLump.offset = start + 20;
		GameLump.length = 16 + GameLump.path_count * sizeof(GameLump_Path) + GameLump.prop_count * sizeof(GameLump_Prop);
		SafeWrite(file, &GameLump, sizeof(GameLump));
		/* need to write vectors separately */
		/* paths */
		fseek(file, start + 24, SEEK_SET);
		SafeWrite(file, GameLump.paths.data(), 128 * GameLump.path_count);
		/* :) */
		SafeWrite(file, &GameLump.prop_count, 4);
		SafeWrite(file, &GameLump.prop_count, 4);
		SafeWrite(file, &GameLump.prop_count, 4);
		/* props */
		SafeWrite(file, GameLump.props.data(), 64 * GameLump.prop_count);
	}
	AddLump(file, header.lumps[LUMP_TEXTURE_DATA_STRING_DATA],			bspTextureDataData);
	AddLump(file, header.lumps[LUMP_TEXTURE_DATA_STRING_TABLE],			bspTextureDataTable);
	AddLump(file, header.lumps[LUMP_WORLD_LIGHTS],						bspWorldLights_stub);
	AddLump(file, header.lumps[LUMP_TRICOLL_TRIS],						bspTricollTris_stub);
	AddLump(file, header.lumps[LUMP_TRICOLL_NODES],						bspTricollNodes_stub);
	AddLump(file, header.lumps[LUMP_TRICOLL_HEADERS],					bspTricollHeaders_stub);
	AddLump(file, header.lumps[LUMP_VERTEX_LIT_BUMP],					bspVertexLitBump);
	AddLump(file, header.lumps[LUMP_MESH_INDICES],						bspMeshIndices);
	AddLump(file, header.lumps[LUMP_MESHES],							bspMeshes);
	AddLump(file, header.lumps[LUMP_MESH_BOUNDS],						bspMeshBounds);
	AddLump(file, header.lumps[LUMP_MATERIAL_SORT],						bspMaterialSorts);
	AddLump(file, header.lumps[LUMP_LIGHTMAP_HEADERS],					bspLightMapHeaders_stub);
	AddLump(file, header.lumps[LUMP_CM_GRID],							bspCMGrid_stub);
	AddLump(file, header.lumps[LUMP_CM_GRID_CELLS],						bspCMGridCells_stub);
	AddLump(file, header.lumps[LUMP_CM_GRID_SETS],						bspCMGridSets_stub);
	AddLump(file, header.lumps[LUMP_CM_GEO_SET_BOUNDS],					bspCMGeoSetBounds_stub);
	AddLump(file, header.lumps[LUMP_CM_PRIMITIVES],						bspCMPrimitives_stub);
	AddLump(file, header.lumps[LUMP_CM_PRIMITIVE_BOUNDS],				bspCMPrimitiveBounds_stub);
	AddLump(file, header.lumps[LUMP_CM_UNIQUE_CONTENTS],				bspCMUniqueContents_stub);
	AddLump(file, header.lumps[LUMP_CM_BRUSHES],						bspCMBrushes_stub);
	AddLump(file, header.lumps[LUMP_CM_BRUSH_SIDE_PLANE_OFFSETS],		bspCMBrushSidePlaneOffsets_stub);
	AddLump(file, header.lumps[LUMP_CM_BRUSH_SIDE_PROPS],				bspCMBrushSideProps_stub);
	AddLump(file, header.lumps[LUMP_CM_BRUSH_TEX_VECS],					bspCMBrushTexVecs_stub);
	AddLump(file, header.lumps[LUMP_TRICOLL_BEVEL_STARTS],				bspTricollBevelStarts_stub);
	AddLump(file, header.lumps[LUMP_LIGHTMAP_DATA_SKY],					bspLightMapDataSky_stub);
	AddLump(file, header.lumps[LUMP_CSM_AABB_NODES],					bspCSMAABBNodes_stub);
	AddLump(file, header.lumps[LUMP_CELL_BSP_NODES],					bspCellBSPNodes_stub);
	AddLump(file, header.lumps[LUMP_CELLS],								bspCells_stub);
	AddLump(file, header.lumps[LUMP_PORTALS],							bspPortals_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_VERTICES],					bspPortalVertices_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_EDGES],						bspPortalEdges_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_VERTEX_EDGES],				bspPortalVertexEdges_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_VERTEX_REFERENCES],			bspPortalVertexReferences_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_EDGE_REGERENCES],			bspPortalEdgeReferences_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_EDGE_INTERSECT_EDGE],		bspPortalEdgeIntersectEdge_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_EDGE_INTERSECT_AT_VERTEX],	bspPortalEdgeIntersectAtVertex_stub);
	AddLump(file, header.lumps[LUMP_PORTAL_EDGE_INTERSECT_HEADER],		bspPortalEdgeIntersectHeader_stub);
	AddLump(file, header.lumps[LUMP_CELL_AABB_NODES],					bspCellAABBNodes_stub);
	AddLump(file, header.lumps[LUMP_OBJ_REFERENCES],					bspObjReferences);
	AddLump(file, header.lumps[LUMP_OBJ_REFERENCE_BOUNDS],				bspObjReferenceBounds);
	AddLump(file, header.lumps[LUMP_LEVEL_INFO],						bspLevelInfo);


	/* emit bsp size */
	const int size = ftell(file);
	Sys_Printf("Wrote %.1f MB (%d bytes)\n", (float)size / (1024 * 1024), size);

	/* write the completed header */
	fseek(file, 0, SEEK_SET);
	SafeWrite(file, &header, sizeof(header));

	/* close the file */
	fclose(file);

	WriteR2EntFiles( filename );
}

/*
   CompileR2BSPFile()
   writes a titanfall2 bsp file and it's .ent files
 */
void CompileR2BSPFile()
{
	SetUpGameLump();

	for (size_t entityNum = 0; entityNum < entities.size(); ++entityNum)
	{
		/* get entity */
		entity_t& entity = entities[entityNum];
		const char* classname = entity.classname();

		/* visible geo */
		if ( strcmp( classname,"worldspawn" ) == 0 )
		{

			/* generate bsp meshes from map brushes */
			EmitMeshes(entity);

			EmitBrushes(entity);

			/* Generate Model lump */
			EmitModels();

		}
		/* props for gamelump */
		else if ( strcmp( classname, "misc_model" ) == 0)
		{
			EmitProp( entity );
		}
	}

	/* */
	EmitEntityPartitions();

	/**/
	EmitObjReferences();

	/* Emit LevelInfo */
	EmitLevelInfo();

	/* Generate unknown lumps */
	EmitStubs();
}