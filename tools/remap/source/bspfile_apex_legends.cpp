/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r5 ( Apex ) .bsp file.

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
#define R5_LUMP_ENTITIES						0x00
#define R5_LUMP_PLANES							0x01
#define R5_LUMP_TEXTURE_DATA					0x02
#define R5_LUMP_VERTICES						0x03
#define R5_LUMP_LIGHTPROBE_PARENT_INFOS			0x04
#define R5_LUMP_SHADOW_ENVIRONMENTS				0x05
#define R5_LUMP_MODELS							0x0E
#define R5_LUMP_SURFACE_NAMES					0x0F
#define R5_LUMP_CONTENT_MASKS					0x10
#define R5_LUMP_SURFACE_PROPERTIES				0x11
#define R5_LUMP_BVH_NODES						0x12
#define R5_LUMP_BVH_LEAF_DATA					0x13
#define R5_LUMP_PACKED_VERTICES					0x14
#define R5_LUMP_ENTITY_PARTITIONS				0x18
#define R5_LUMP_VERTEX_NORMALS					0x1E
#define R5_LUMP_GAME_LUMP						0x23
#define R5_LUMP_UNKNOWN_37						0x25
#define R5_LUMP_UNKNOWN_38						0x26
#define R5_LUMP_UNKNOWN_39						0x27
#define R5_LUMP_PAKFILE							0x28
#define R5_LUMP_CUBEMAPS						0x2A
#define R5_LUMP_UNKNOWN_43						0x2B
#define R5_LUMP_WORLD_LIGHTS					0x36
#define R5_LUMP_WORLD_LIGHT_PARENT_INFOS		0x37
#define R5_LUMP_VERTEX_UNLIT					0x47
#define R5_LUMP_VERTEX_LIT_FLAT					0x48
#define R5_LUMP_VERTEX_LIT_BUMP					0x49
#define R5_LUMP_VERTEX_UNLIT_TS					0x4A
#define R5_LUMP_VERTEX_BLINN_PHONG				0x4B
#define R5_LUMP_VERTEX_RESERVED_5				0x4C
#define R5_LUMP_VERTEX_RESERVED_f				0x4D
#define R5_LUMP_VERTEX_RESERVED_7				0x4E
#define R5_LUMP_MESH_INDICES					0x4F
#define R5_LUMP_MESHES							0x50
#define R5_LUMP_MESH_BOUNDS						0x51
#define R5_LUMP_MATERIAL_SORT					0x52
#define R5_LUMP_LIGHTMAP_HEADERS				0x53
#define R5_LUMP_TWEAK_LIGHTS					0x55
#define R5_LUMP_UNKNOWN_97						0x61
#define R5_LUMP_LIGHTMAP_DATA_SKY				0x62
#define R5_LUMP_CSM_AABB_NODES					0x63
#define R5_LUMP_CSM_OBJ_REFERENCES				0x64
#define R5_LUMP_LIGHTPROBES						0x65
#define R5_LUMP_STATIC_PROP_LIGHTPROBE_INDICES	0x66
#define R5_LUMP_LIGHTPROBE_TREE					0x67
#define R5_LUMP_LIGHTPROBE_REFERENCES			0x68
#define R5_LUMP_CELL_BSP_NODES					0x6A
#define R5_LUMP_CELLS							0x6B
#define R5_LUMP_PORTALS							0x6C
#define R5_LUMP_PORTAL_VERTICES					0x6D
#define R5_LUMP_PORTAL_EDGES					0x6E
#define R5_LUMP_PORTAL_VERTEX_EDGES				0x6F
#define R5_LUMP_PORTAL_VERTEX_REFERENCES		0x70
#define R5_LUMP_PORTAL_EDGE_REFERENCES			0x71
#define R5_LUMP_PORTAL_EDGE_INTERSECT_EDGE		0x72
#define R5_LUMP_PORTAL_EDGE_INTERSECT_AT_VERTEX	0x73
#define R5_LUMP_PORTAL_EDGE_INTERSECT_HEADER	0x74
#define R5_LUMP_OCCLUSION_MESH_VERTICES			0x75
#define R5_LUMP_OCCLUSION_MESH_INDICES			0x76
#define R5_LUMP_CELL_AABB_NODES					0x77
#define R5_LUMP_OBJ_REFERENCES					0x78
#define R5_LUMP_OBJ_REFERENCE_BOUNDS			0x79
#define R5_LUMP_LIGHTMAP_DATA_RTL_PAGE			0x7A
#define R5_LUMP_LEVEL_INFO						0x7B
#define R5_LUMP_SHADOW_MESH_OPAQUE_VERTICES		0x7C
#define R5_LUMP_SHADOW_MESH_ALPHA_VERTICES		0x7D
#define R5_LUMP_SHADOW_MESH_INDICES				0x7E
#define R5_LUMP_SHADOW_MESHES					0x7F


/*
   LoadR2BSPFile()
   loads a titanfall2 bsp file
 */
void LoadR5BSPFile(const char* filename)
{

}


/*
   WriteR2BSPFile()
   writes a apex bsp file
 */
void WriteR5BSPFile(const char* filename)
{
	rbspHeader_t header{};

	/* set up header */
	memcpy(header.ident, g_game->bspIdent, 4);
	header.version = LittleLong(g_game->bspVersion);
	header.mapVersion = 30;
	header.maxLump = 127;

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
	AddLump(file, header.lumps[R5_LUMP_ENTITIES],			Titanfall::Bsp::entities);
	AddLump(file, header.lumps[R5_LUMP_VERTICES],			Titanfall::Bsp::vertices);
	AddLump(file, header.lumps[R5_LUMP_VERTEX_NORMALS],		Titanfall::Bsp::vertexNormals);
	AddLump(file, header.lumps[R5_LUMP_VERTEX_LIT_BUMP],	ApexLegends::vertexLitBumpVertices);
	AddLump(file, header.lumps[R5_LUMP_MESHES],				ApexLegends::meshes);
	AddLump(file, header.lumps[R5_LUMP_MESH_INDICES],		Titanfall::Bsp::meshIndices);


	/* emit bsp size */
	const int size = ftell(file);
	Sys_Printf("Wrote %.1f MB (%d bytes)\n", (float)size / (1024 * 1024), size);

	/* write the completed header */
	fseek(file, 0, SEEK_SET);
	SafeWrite(file, &header, sizeof(header));

	/* close the file */
	fclose(file);
}

/*
   CompileR5BSPFile()
   Compiles a v47 bsp file
*/
void CompileR5BSPFile()
{
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
			ApexLegends::EmitMeshes(entity);
		}
	}
}

namespace ApexLegends {
	void EmitMeshes( const entity_t& e ) {
		for ( const Shared::mesh_t &mesh : Shared::meshes ) {
			ApexLegends::Mesh_t &bm = ApexLegends::meshes.emplace_back();
			bm.flags = 0x200;
			bm.triangleOffset = Titanfall::Bsp::meshIndices.size();
			bm.triangleCount = mesh.triangles.size() / 3;


			// Save vertices and vertexnormals
			for ( std::size_t i = 0; i < mesh.vertices.size(); i++ )
			{
				Shared::vertex_t vertex = mesh.vertices.at( i );

				ApexLegends::VertexLitBump_t &lv = ApexLegends::vertexLitBumpVertices.emplace_back();
				lv.uv0 = vertex.st;

				// Save vert
				for ( uint16_t j = 0; j < Titanfall::Bsp::vertices.size(); j++ )
				{
					if ( VectorCompare( vertex.xyz, Titanfall::Bsp::vertices.at( j ) ) )
					{
						lv.vertexIndex = j;
						goto normal;
					}
				}

				{
					lv.vertexIndex = Titanfall::Bsp::vertices.size();
					Titanfall::Bsp::vertices.emplace_back( vertex.xyz );
				}

				normal:;

				for ( uint16_t j = 0; j < Titanfall::Bsp::vertexNormals.size(); j++ )
				{
					if ( VectorCompare( vertex.normal, Titanfall::Bsp::vertexNormals.at( j ) ) )
					{
						lv.normalIndex = j;
						goto end;
					}
				}

				{
					lv.normalIndex = Titanfall::Bsp::vertexNormals.size();
					Titanfall::Bsp::vertexNormals.emplace_back( vertex.normal );
				}

				end:;
			}

			// Save triangles
			for ( uint16_t triangle : mesh.triangles )
			{
				for ( uint32_t j = 0; j < ApexLegends::vertexLitBumpVertices.size(); j++ )
				{
					if ( VectorCompare( Titanfall::Bsp::vertices.at( ApexLegends::vertexLitBumpVertices.at( j ).vertexIndex ), mesh.vertices.at( triangle ).xyz ) )
					{
						if ( VectorCompare( Titanfall::Bsp::vertexNormals.at( ApexLegends::vertexLitBumpVertices.at( j ).normalIndex ), mesh.vertices.at( triangle ).normal ) )
						{
							ApexLegends::MeshIndex_t& index = Titanfall::Bsp::meshIndices.emplace_back();
							index = j;
							break;
						}
					}
				}
			}
		}
	}
}