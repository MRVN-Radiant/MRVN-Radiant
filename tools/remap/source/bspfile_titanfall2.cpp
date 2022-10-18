/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r2 ( Titanfall 2 ) .bsp file.

   A lot of this is temporary and will be altered heavily with new information.
   If you know how to c++ better than me feel free to contribute or call me words
   in discord. :)

   - Fifty#8113, also known as Fifteen, Sixteen, Seventeen, Eigtheen and FORTY

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "remap.h"
#include "bspfile_abstract.h"
#include <ctime>


/* constants */
#define R2_LUMP_ENTITIES							0x00
#define R2_LUMP_PLANES								0x01
#define R2_LUMP_TEXTURE_DATA						0x02
#define R2_LUMP_VERTICES							0x03
#define R2_LUMP_LIGHTPROBE_PARENT_INFOS				0x04
#define R2_LUMP_SHADOW_ENVIRONMENTS					0x05
#define R2_LUMP_LIGHTPROBE_BSP_NODES				0x06
#define R2_LUMP_LIGHTPROBE_BSP_REF_IDS				0x07
#define R2_LUMP_MODELS								0x0E
#define R2_LUMP_ENTITY_PARTITIONS					0x18
#define R2_LUMP_PHYSICS_COLLIDE						0x1D
#define R2_LUMP_VERTEX_NORMALS						0x1E
#define R2_LUMP_GAME_LUMP							0x23
#define R2_LUMP_LEAF_WATER_DATA						0x24
#define R2_LUMP_PAKFILE								0x28
#define R2_LUMP_CUBEMAPS							0x2A
#define R2_LUMP_TEXTURE_DATA_STRING_DATA			0x2B
#define R2_LUMP_TEXTURE_DATA_STRING_TABLE			0x2C
#define R2_LUMP_WORLD_LIGHTS						0x36
#define R2_LUMP_WORLD_LIGHT_PARENT_INFOS			0x37
#define R2_LUMP_PHYSICS_LEVEL						0x3E
#define R2_LUMP_TRICOLL_TRIS						0x42
#define R2_LUMP_TRICOLL_NODES						0x44
#define R2_LUMP_TRICOLL_HEADERS						0x45
#define R2_LUMP_PHYSICS_TRIANGLES					0x46
#define R2_LUMP_VERTEX_UNLIT						0x47
#define R2_LUMP_VERTEX_LIT_FLAT						0x48
#define R2_LUMP_VERTEX_LIT_BUMP						0x49
#define R2_LUMP_VERTEX_UNLIT_TS						0x4A
#define R2_LUMP_VERTEX_BLINN_PHONG					0x4B
#define R2_LUMP_VERTEX_RESERVED_5					0x4C
#define R2_LUMP_VERTEX_RESERVED_6					0x4D
#define R2_LUMP_VERTEX_RESERVED_7					0x4E
#define R2_LUMP_MESH_INDICES						0x4F
#define R2_LUMP_MESHES								0x50
#define R2_LUMP_MESH_BOUNDS							0x51
#define R2_LUMP_MATERIAL_SORT						0x52
#define R2_LUMP_LIGHTMAP_HEADERS					0x53
#define R2_LUMP_CM_GRID								0x55
#define R2_LUMP_CM_GRID_CELLS						0x56
#define R2_LUMP_CM_GRID_SETS						0x57
#define R2_LUMP_CM_GEO_SET_BOUNDS					0x58
#define R2_LUMP_CM_PRIMITIVES						0x59
#define R2_LUMP_CM_PRIMITIVE_BOUNDS					0x5A
#define R2_LUMP_CM_UNIQUE_CONTENTS					0x5B
#define R2_LUMP_CM_BRUSHES							0x5C
#define R2_LUMP_CM_BRUSH_SIDE_PLANES				0x5D
#define R2_LUMP_CM_BRUSH_SIDE_PROPS					0x5E
#define R2_LUMP_CM_BRUSH_TEX_VECS					0x5F
#define R2_LUMP_TRICOLL_BEVEL_STARTS				0x60
#define R2_LUMP_TRICOLL_BEVEL_INDICES				0x61
#define R2_LUMP_LIGHTMAP_DATA_SKY					0x62
#define R2_LUMP_CSM_AABB_NODES						0x63
#define R2_LUMP_CSM_OBJ_REFERENCES					0x64
#define R2_LUMP_LIGHTPROBES							0x65
#define R2_LUMP_STATIC_PROP_LIGHTPROBE_INDICES		0x66
#define R2_LUMP_LIGHTPROBE_TREE						0x67
#define R2_LUMP_LIGHTPROBE_REFERENCES				0x68
#define R2_LUMP_LIGHTMAP_DATA_REAL_TIME_LIGHTS		0x69
#define R2_LUMP_CELL_BSP_NODES						0x6A
#define R2_LUMP_CELLS								0x6B
#define R2_LUMP_PORTALS								0x6C
#define R2_LUMP_PORTAL_VERTICES						0x6D
#define R2_LUMP_PORTAL_EDGES						0x6E
#define R2_LUMP_PORTAL_VERTEX_EDGES					0x6F
#define R2_LUMP_PORTAL_VERTEX_REFERENCES			0x70
#define R2_LUMP_PORTAL_EDGE_REGERENCES				0x71
#define R2_LUMP_PORTAL_EDGE_INTERSECT_EDGE			0x72
#define R2_LUMP_PORTAL_EDGE_INTERSECT_AT_VERTEX		0x73
#define R2_LUMP_PORTAL_EDGE_INTERSECT_HEADER		0x74
#define R2_LUMP_OCCLUSION_MESH_VERTICES				0x75
#define R2_LUMP_OCCLUSION_MESH_INDICES				0x76
#define R2_LUMP_CELL_AABB_NODES						0x77
#define R2_LUMP_OBJ_REFERENCES						0x78
#define R2_LUMP_OBJ_REFERENCE_BOUNDS				0x79
#define R2_LUMP_LIGHTMAP_DATA_RTL_PAGE				0x7A
#define R2_LUMP_LEVEL_INFO							0x7B
#define R2_LUMP_SHADOW_MESH_OPAQUE_VERTICES			0x7C
#define R2_LUMP_SHADOW_MESH_ALPHA_VERTICES			0x7D
#define R2_LUMP_SHADOW_MESH_INDICES					0x7E
#define R2_LUMP_SHADOW_MESHES						0x7F




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
	CopyLump( (rbspHeader_t*) header, R2_LUMP_ENTITIES,				bspEntities );
	CopyLump( (rbspHeader_t*) header, R2_LUMP_PLANES,				bspPlanes );
	CopyLump( (rbspHeader_t*) header, R2_LUMP_CM_BRUSHES,			bspBrushes );
	//CopyLump( (rbspHeader_t*) header, R2_LUMP_CM_BRUSH_SIDE_PLANES, Titanfall2::bspBrushSidePlanes);
	

	/*
		Check if bsp references .ent files, theoratically it can reference whatever it wants
		but we only read the ones respawn uses
	*/
	if ( header->lumps[0x18].length )
	{
		/* Spawn */
		{
			auto name = StringOutputStream(256)(PathExtensionless(filename), "_spawn.ent");
			LoadEntFile( name.c_str(), bspEntities );
		}
		/* Snd */
		{
			auto name = StringOutputStream(256)(PathExtensionless(filename), "_snd.ent");
			LoadEntFile( name.c_str(), bspEntities );
		}
		/* Script */
		{
			auto name = StringOutputStream(256)(PathExtensionless(filename), "_script.ent");
			LoadEntFile(name.c_str(), bspEntities);
		}
		/* Fx */
		{
			auto name = StringOutputStream(256)(PathExtensionless(filename), "_fx.ent");
			LoadEntFile(name.c_str(), bspEntities);
		}
		/* Env */
		{
			auto name = StringOutputStream(256)(PathExtensionless(filename), "_env.ent");
			LoadEntFile(name.c_str(), bspEntities);
		}
	}


	/*
		Load Entities
	*/
	ParseEntities();

	/*
		Convert loaded lumps into the entities vector
	*/
	if ( entities.size() == 0 )
		Error( "No entities" );

	for (std::size_t i = 0; i < bspBrushes.size(); i++)
	{
		bspBrush_t& bspBrush = bspBrushes.at(i);


		Vector3 mins = bspBrush.origin - bspBrush.extents;
		Vector3 maxs = bspBrush.origin + bspBrush.extents;

		/* Create the base 6 planes from the brush AABB */
		std::vector<Plane3> planes;

		{
			Vector3 vertices[8];
			vertices[0] = maxs;
			vertices[1] = Vector3(maxs.x(), maxs.y(), mins.z());
			vertices[2] = Vector3(maxs.x(), mins.y(), maxs.z());
			vertices[3] = Vector3(mins.x(), maxs.y(), maxs.z());
			vertices[4] = mins;
			vertices[5] = Vector3(mins.x(), mins.y(), maxs.z());
			vertices[6] = Vector3(mins.x(), maxs.y(), mins.z());
			vertices[7] = Vector3(maxs.x(), mins.y(), mins.z());


			Plane3& plane0 = planes.emplace_back();
			PlaneFromPoints(plane0, vertices[0], vertices[1], vertices[2]);
			Plane3& plane1 = planes.emplace_back();
			PlaneFromPoints(plane1, vertices[0], vertices[2], vertices[3]);
			Plane3& plane2 = planes.emplace_back();
			PlaneFromPoints(plane2, vertices[0], vertices[3], vertices[1]);
			Plane3& plane3 = planes.emplace_back();
			PlaneFromPoints(plane3, vertices[6], vertices[5], vertices[4]);
			Plane3& plane4 = planes.emplace_back();
			PlaneFromPoints(plane4, vertices[7], vertices[6], vertices[4]);
			Plane3& plane5 = planes.emplace_back();
			PlaneFromPoints(plane5, vertices[5], vertices[7], vertices[4]);
		}

		/* Make brush and fiil it with sides */

		brush_t &brush = entities.data()[0].brushes.emplace_back();

		for ( Plane3 plane : planes )
		{
			side_t& side = brush.sides.emplace_back();
			side.plane = plane;
		}
	}
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
		char message[64] = "Built with love using MRVN-radiant :)";
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
	
	AddLump(file, header.lumps[R2_LUMP_ENTITIES],							Titanfall::Bsp::entities);
	AddLump(file, header.lumps[R2_LUMP_TEXTURE_DATA],						Titanfall::Bsp::textureData);
	AddLump(file, header.lumps[R2_LUMP_VERTICES],							Titanfall::Bsp::vertices);
	AddLump(file, header.lumps[R2_LUMP_MODELS],								Titanfall::Bsp::models);
	AddLump(file, header.lumps[R2_LUMP_VERTEX_NORMALS],						Titanfall::Bsp::vertexNormals);
	AddLump(file, header.lumps[R2_LUMP_ENTITY_PARTITIONS],					Titanfall2::bspEntityPartitions);
	/* Game Lump */
	{
		std::size_t start = ftell(file);
		header.lumps[R2_LUMP_GAME_LUMP].offset = start;
		header.lumps[R2_LUMP_GAME_LUMP].length = 36 + Titanfall2::GameLump.pathCount * sizeof(Titanfall2::GameLump_Path) + Titanfall2::GameLump.propCount * sizeof(Titanfall2::GameLump_Prop);
		Titanfall2::GameLump.offset = start + 20;
		Titanfall2::GameLump.length = 16 + Titanfall2::GameLump.pathCount * sizeof(Titanfall2::GameLump_Path) + Titanfall2::GameLump.propCount * sizeof(Titanfall2::GameLump_Prop);
		SafeWrite(file, &Titanfall2::GameLump, sizeof(Titanfall2::GameLump));
		/* need to write vectors separately */
		/* paths */
		fseek(file, start + 24, SEEK_SET);
		SafeWrite(file, Titanfall2::GameLump.paths.data(), 128 * Titanfall2::GameLump.pathCount);
		/* :) */
		SafeWrite(file, &Titanfall2::GameLump.propCount, 4);
		SafeWrite(file, &Titanfall2::GameLump.propCount, 4);
		SafeWrite(file, &Titanfall2::GameLump.propCount, 4);
		/* props */
		SafeWrite(file, Titanfall2::GameLump.props.data(), 64 * Titanfall2::GameLump.propCount);
		SafeWrite(file, &Titanfall2::GameLump.unk5, 4);
	}
	AddLump(file, header.lumps[R2_LUMP_TEXTURE_DATA_STRING_DATA],			Titanfall2::bspTextureDataData);
	AddLump(file, header.lumps[R2_LUMP_TEXTURE_DATA_STRING_TABLE],			Titanfall2::bspTextureDataTable);
	AddLump(file, header.lumps[R2_LUMP_WORLD_LIGHTS],						Titanfall2::bspWorldLights_stub);
	AddLump(file, header.lumps[R2_LUMP_TRICOLL_TRIS],						Titanfall2::bspTricollTris_stub);
	AddLump(file, header.lumps[R2_LUMP_TRICOLL_NODES],						Titanfall2::bspTricollNodes_stub);
	AddLump(file, header.lumps[R2_LUMP_TRICOLL_HEADERS],					Titanfall2::bspTricollHeaders_stub);
	AddLump(file, header.lumps[R2_LUMP_VERTEX_LIT_BUMP],					Titanfall::Bsp::vertexLitBumpVertices);
	AddLump(file, header.lumps[R2_LUMP_MESH_INDICES],						Titanfall::Bsp::meshIndices);
	AddLump(file, header.lumps[R2_LUMP_MESHES],								Titanfall::Bsp::meshes);
	AddLump(file, header.lumps[R2_LUMP_MESH_BOUNDS],						Titanfall2::bspMeshBounds);
	AddLump(file, header.lumps[R2_LUMP_MATERIAL_SORT],						Titanfall2::bspMaterialSorts);
	AddLump(file, header.lumps[R2_LUMP_LIGHTMAP_HEADERS],					Titanfall2::bspLightMapHeaders_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_GRID],							Titanfall2::bspCMGrid_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_GRID_CELLS],						Titanfall2::bspCMGridCells_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_GRID_SETS],						Titanfall2::bspCMGridSets_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_GEO_SET_BOUNDS],					Titanfall2::bspCMGeoSetBounds_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_PRIMITIVES],						Titanfall2::bspCMPrimitives_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_PRIMITIVE_BOUNDS],				Titanfall2::bspCMPrimitiveBounds_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_UNIQUE_CONTENTS],					Titanfall2::bspCMUniqueContents_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_BRUSHES],							Titanfall2::bspCMBrushes_stub);
	AddLump(file, header.lumps[R2_LUMP_CM_BRUSH_SIDE_PROPS],				Titanfall2::bspCMBrushSideProps_stub);
	AddLump(file, header.lumps[R2_LUMP_TRICOLL_BEVEL_STARTS],				Titanfall2::bspTricollBevelStarts_stub);
	AddLump(file, header.lumps[R2_LUMP_LIGHTMAP_DATA_SKY],					Titanfall2::bspLightMapDataSky_stub);
	AddLump(file, header.lumps[R2_LUMP_CSM_AABB_NODES],						Titanfall2::bspCSMAABBNodes_stub);
	AddLump(file, header.lumps[R2_LUMP_CELL_BSP_NODES],						Titanfall2::bspCellBSPNodes_stub);
	AddLump(file, header.lumps[R2_LUMP_CELLS],								Titanfall2::bspCells_stub);
	AddLump(file, header.lumps[R2_LUMP_CELL_AABB_NODES],					Titanfall2::bspCellAABBNodes);
	AddLump(file, header.lumps[R2_LUMP_OBJ_REFERENCES],						Titanfall2::bspObjReferences);
	AddLump(file, header.lumps[R2_LUMP_OBJ_REFERENCE_BOUNDS],				Titanfall2::bspObjReferenceBounds);
	AddLump(file, header.lumps[R2_LUMP_LEVEL_INFO],							Titanfall2::bspLevelInfo);


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

		EmitEntity( entity );

		/* visible geo */
		if ( striEqual( classname,"worldspawn" ) )
		{

			/* generate bsp meshes from map brushes */
			Shared::MakeMeshes(entity);
			EmitMeshes(entity);

			EmitBrushes(entity);

			/* Generate Model lump */
			EmitModels();

		}
		/* props for gamelump */
		else if ( striEqual( classname, "misc_model" ) )
		{
			EmitProp( entity );
		}
	}

	/* */
	EmitEntityPartitions();

	/**/
	Shared::MakeVisReferences();
	Shared::visRoot = Shared::MakeVisTree( Shared::visRefs, 1e30f );
	EmitVisTree();

	/* Emit LevelInfo */
	EmitLevelInfo();

	/* Generate unknown lumps */
	EmitStubs();
}