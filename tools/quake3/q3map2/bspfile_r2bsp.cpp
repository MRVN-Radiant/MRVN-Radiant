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
#define LUMP_CM_GEO_SETS						0x57
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

#define HEADER_LUMPS 128

/* types */
struct rbspHeader_t
{
	char ident[4];
	int version;
	int mapVersion;
	int maxLump;

	bspLump_t lumps[HEADER_LUMPS];
};


/*
   LoadR2BSPFile()
   loads a titanfall2 bsp file
 */
void LoadR2BSPFile(const char* filename)
{
}


/*
   WriteR2BSPFile()
   writes a titanfall2 bsp file
 */
void WriteR2BSPFile(const char* filename)
{
}