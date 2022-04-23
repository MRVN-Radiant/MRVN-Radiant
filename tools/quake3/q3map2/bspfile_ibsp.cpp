/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"
#include "bspfile_abstract.h"
#include <ctime>




/* -------------------------------------------------------------------------------

   this file handles translating the bsp file format used by quake 3, rtcw, and ef
   into the abstracted bsp file used by q3map2.

   ------------------------------------------------------------------------------- */

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
struct ibspHeader_t
{
	char ident[ 4 ];
	int version;
	int mapVersion;
	int maxLump;

	bspLump_t lumps[ HEADER_LUMPS ];
};



/* brush sides */
struct ibspBrushSide_t
{
	int planeNum;
	int shaderNum;
	ibspBrushSide_t( const bspBrushSide_t& other ) :
		planeNum( other.planeNum ),
		shaderNum( other.shaderNum ){}
	operator bspBrushSide_t() const {
		return { planeNum, shaderNum, -1 };
	}
};



/* drawsurfaces */
struct ibspDrawSurface_t
{
	int shaderNum;
	int fogNum;
	int surfaceType;

	int firstVert;
	int numVerts;

	int firstIndex;
	int numIndexes;

	int lightmapNum;
	int lightmapX, lightmapY;
	int lightmapWidth, lightmapHeight;

	Vector3 lightmapOrigin;
	Vector3 lightmapVecs[ 3 ];

	int patchWidth;
	int patchHeight;
	ibspDrawSurface_t( const bspDrawSurface_t& other ) :
		shaderNum( other.shaderNum ),
		fogNum( other.fogNum ),
		surfaceType( other.surfaceType ),
		firstVert( other.firstVert ),
		numVerts( other.numVerts ),
		firstIndex( other.firstIndex ),
		numIndexes( other.numIndexes ),
		lightmapNum( other.lightmapNum[0] ),
		lightmapX( other.lightmapX[0] ),
		lightmapY( other.lightmapY[0] ),
		lightmapWidth( other.lightmapWidth ),
		lightmapHeight( other.lightmapHeight ),
		lightmapOrigin( other.lightmapOrigin ),
		lightmapVecs{ other.lightmapVecs[0], other.lightmapVecs[1], other.lightmapVecs[2] },
		patchWidth( other.patchWidth ),
		patchHeight( other.patchHeight ) {}
	operator bspDrawSurface_t() const {
		static_assert( MAX_LIGHTMAPS == 4 );
		return{
			shaderNum,
			fogNum,
			surfaceType,
			firstVert,
			numVerts,
			firstIndex,
			numIndexes,
			{ LS_NORMAL, LS_NONE, LS_NONE, LS_NONE },
			{ LS_NORMAL, LS_NONE, LS_NONE, LS_NONE },
			{ lightmapNum, -3, -3, -3 },
			{ lightmapX, 0, 0, 0 },
			{ lightmapY, 0, 0, 0 },
			lightmapWidth,
			lightmapHeight,
			lightmapOrigin,
			{ lightmapVecs[0], lightmapVecs[1], lightmapVecs[2] },
			patchWidth,
			patchHeight
		};
	}
};



/* drawverts */
struct ibspDrawVert_t
{
	Vector3 xyz;
	Vector2 st;
	Vector2 lightmap;
	Vector3 normal;
	Color4b color;
	ibspDrawVert_t( const bspDrawVert_t& other ) :
		xyz( other.xyz ),
		st( other.st ),
		lightmap( other.lightmap[0] ),
		normal( other.normal ),
		color( other.color[0] ) {}
	operator bspDrawVert_t() const {
		static_assert( MAX_LIGHTMAPS == 4 );
		return {
			xyz,
			st,
			{ lightmap, Vector2( 0, 0 ), Vector2( 0, 0 ), Vector2( 0, 0 ) },
			normal,
			{ color, Color4b( 0, 0, 0, 0 ), Color4b( 0, 0, 0, 0 ), Color4b( 0, 0, 0, 0 ) }
		};
	}
};



/* light grid */
struct ibspGridPoint_t
{
	Vector3b ambient;
	Vector3b directed;
	byte latLong[ 2 ];
	ibspGridPoint_t( const bspGridPoint_t& other ) :
		ambient( other.ambient[0] ),
		directed( other.directed[0] ),
		latLong{ other.latLong[0], other.latLong[1] } {}
	operator bspGridPoint_t() const {
		static_assert( MAX_LIGHTMAPS == 4 );
		return {
			{ ambient, ambient, ambient, ambient },
			{ directed, directed, directed, directed },
			{ LS_NORMAL, LS_NONE, LS_NONE, LS_NONE },
			{ latLong[0], latLong[1] }
		};
	}
};



/*
   LoadIBSPFile()
   loads a quake 3 bsp file into memory
 */

void LoadIBSPFile( const char *filename ){
	/* load the file */
	MemBuffer file = LoadFile( filename );

	ibspHeader_t    *header = file.data();

	/* swap the header (except the first 4 bytes) */
	SwapBlock( (int*) ( (byte*) header + 4 ), sizeof( *header ) - 4 );

	/* make sure it matches the format we're trying to load */
	if ( !force && memcmp( header->ident, g_game->bspIdent, 4 ) ) {
		Error( "%s is not a %s file", filename, g_game->bspIdent );
	}
	if ( !force && header->version != g_game->bspVersion ) {
		Error( "%s is version %d, not %d", filename, header->version, g_game->bspVersion );
	}

	/* load/convert lumps */
	//CopyLump( (bspHeader_t*) header, LUMP_SHADERS, bspShaders );
	//CopyLump( (bspHeader_t*) header, LUMP_MODELS, bspModels );
	//CopyLump( (bspHeader_t*) header, LUMP_PLANES, bspPlanes );
	//CopyLump( (bspHeader_t*) header, LUMP_LEAFS, bspLeafs );
	//CopyLump( (bspHeader_t*) header, LUMP_NODES, bspNodes );
	//CopyLump( (bspHeader_t*) header, LUMP_LEAFSURFACES, bspLeafSurfaces );
	//CopyLump( (bspHeader_t*) header, LUMP_LEAFBRUSHES, bspLeafBrushes );
	//CopyLump( (bspHeader_t*) header, LUMP_BRUSHES, bspBrushes );
	//CopyLump<bspBrushSide_t, ibspBrushSide_t>( (bspHeader_t*) header, LUMP_BRUSHSIDES, bspBrushSides );
	//CopyLump<bspDrawVert_t, ibspDrawVert_t>( (bspHeader_t*) header, LUMP_DRAWVERTS, bspDrawVerts );
	//CopyLump<bspDrawSurface_t, ibspDrawSurface_t>( (bspHeader_t*) header, LUMP_SURFACES, bspDrawSurfaces );
	//CopyLump( (bspHeader_t*) header, LUMP_FOGS, bspFogs );
	//CopyLump( (bspHeader_t*) header, LUMP_DRAWINDEXES, bspDrawIndexes );
	//CopyLump( (bspHeader_t*) header, LUMP_VISIBILITY, bspVisBytes );
	//CopyLump( (bspHeader_t*) header, LUMP_LIGHTMAPS, bspLightBytes );
	//CopyLump( (bspHeader_t*) header, LUMP_ENTITIES, bspEntData );
	//CopyLump<bspGridPoint_t, ibspGridPoint_t>( (bspHeader_t*) header, LUMP_LIGHTGRID, bspGridPoints );

	/* advertisements */
	if ( header->version == 47 && strEqual( g_game->arg, "quakelive" ) ) { // quake live's bsp version minus wolf, et, etut
		//CopyLump( (bspHeader_t*) header, LUMP_ADVERTISEMENTS, bspAds );
	}
	else{
		bspAds.clear();
	}
}

/*
   LoadIBSPorRBSPFilePartially()
   loads bsp file parts meaningful for autopacker
 */

void LoadIBSPorRBSPFilePartially( const char *filename ){
	/* load the file */
	MemBuffer file = LoadFile( filename );

	ibspHeader_t    *header = file.data();

	/* swap the header (except the first 4 bytes) */
	SwapBlock( (int*) ( (byte*) header + 4 ), sizeof( *header ) - 4 );

	/* make sure it matches the format we're trying to load */
	if ( !force && memcmp( header->ident, g_game->bspIdent, 4 ) ) {
		Error( "%s is not a %s file", filename, g_game->bspIdent );
	}
	if ( !force && header->version != g_game->bspVersion ) {
		Error( "%s is version %d, not %d", filename, header->version, g_game->bspVersion );
	}

	/* load/convert lumps */
	//CopyLump( (bspHeader_t*) header, LUMP_SHADERS, bspShaders );
	//if( g_game->load == LoadIBSPFile )
		//CopyLump<bspDrawSurface_t, ibspDrawSurface_t>( (bspHeader_t*) header, LUMP_SURFACES, bspDrawSurfaces );
	//else
		//CopyLump( (bspHeader_t*) header, LUMP_SURFACES, bspDrawSurfaces );

	//CopyLump( (bspHeader_t*) header, LUMP_FOGS, bspFogs );
	//CopyLump( (bspHeader_t*) header, LUMP_ENTITIES, bspEntData );
}

/*
   WriteIBSPFile()
   writes an id bsp file
 */

void WriteIBSPFile( const char *filename ){
	ibspHeader_t header{};

	//%	Swapfile();

	/* set up header */
	memcpy( header.ident, g_game->bspIdent, 4 );
	header.version = LittleLong( g_game->bspVersion );
	header.mapVersion = 30;
	header.maxLump = 127;

	/* write initial header */
	FILE *file = SafeOpenWrite( filename );
	SafeWrite( file, &header, sizeof( header ) );    /* overwritten later */


	
	char message[64] = "Built with love using r2radiant :)";
	SafeWrite( file, &message, sizeof(message));

	/* add lumps */
	//AddLump( file, header.lumps[LUMP_SHADERS], bspShaders );
	//AddLump( file, header.lumps[LUMP_PLANES], bspPlanes );
	//AddLump( file, header.lumps[LUMP_LEAFS], bspLeafs );
	//AddLump( file, header.lumps[LUMP_NODES], bspNodes );
	//AddLump( file, header.lumps[LUMP_BRUSHES], bspBrushes );
	AddLump( file, header.lumps[LUMP_VERTICES], bspVertices );
	AddLump( file, header.lumps[LUMP_ENTITY_PARTITIONS], bspEntityPartitions );
	AddLump( file, header.lumps[LUMP_MESH_INDICES], bspMeshIndices );
	AddLump( file, header.lumps[LUMP_MESHES], bspMeshes );
	//AddLump( file, header.lumps[LUMP_BRUSHSIDES], std::vector<ibspBrushSide_t>( bspBrushSides.begin(), bspBrushSides.end() ) );
	//AddLump( file, header.lumps[LUMP_LEAFSURFACES], bspLeafSurfaces );
	//AddLump( file, header.lumps[LUMP_LEAFBRUSHES], bspLeafBrushes );
	//AddLump( file, header.lumps[LUMP_MODELS], bspModels );
	//AddLump( file, header.lumps[LUMP_VERTICES], std::vector<ibspDrawVert_t>( bspDrawVerts.begin(), bspDrawVerts.end() ) );
	//AddLump( file, header.lumps[LUMP_SURFACES], std::vector<ibspDrawSurface_t>( bspDrawSurfaces.begin(), bspDrawSurfaces.end() ) );
	//AddLump( file, header.lumps[LUMP_VISIBILITY], bspVisBytes );
	//AddLump( file, header.lumps[LUMP_LIGHTMAPS], bspLightBytes );
	//AddLump( file, header.lumps[LUMP_LIGHTGRID], std::vector<ibspGridPoint_t>( bspGridPoints.begin(), bspGridPoints.end() ) );
	//AddLump( file, header.lumps[LUMP_ENTITIES], bspEntData );
	//AddLump( file, header.lumps[LUMP_FOGS], bspFogs );
	//AddLump( file, header.lumps[LUMP_DRAWINDEXES], bspDrawIndexes );

	/* advertisements */
	//AddLump( file, header.lumps[LUMP_ADVERTISEMENTS], bspAds );

	/* emit bsp size */
	const int size = ftell( file );
	Sys_Printf( "Wrote %.1f MB (%d bytes)\n", (float) size / ( 1024 * 1024 ), size );

	/* write the completed header */
	fseek( file, 0, SEEK_SET );
	SafeWrite( file, &header, sizeof( header ) );

	/* close the file */
	fclose( file );
}
