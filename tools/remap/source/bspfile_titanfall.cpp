/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r1 ( Titanfall1 ) .bsp file.

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
#define R1_LUMP_ENTITIES							0x00
#define R1_LUMP_PLANES								0x01
#define R1_LUMP_TEXTURE_DATA						0x02
#define R1_LUMP_VERTICES							0x03
#define R1_LUMP_MODELS								0x0E
#define R1_LUMP_ENTITY_PARTITIONS					0x18
#define R1_LUMP_PHYSICS_COLLIDE						0x1D
#define R1_LUMP_VERTEX_NORMALS						0x1E
#define R1_LUMP_GAME_LUMP							0x23
#define R1_LUMP_LEAF_WATER_DATA						0x24
#define R1_LUMP_PAKFILE								0x28
#define R1_LUMP_CUBEMAPS							0x2A
#define R1_LUMP_TEXTURE_DATA_STRING_DATA			0x2B
#define R1_LUMP_TEXTURE_DATA_STRING_TABLE			0x2C
#define R1_LUMP_WORLD_LIGHTS						0x36
#define R1_LUMP_PHYSICS_LEVEL						0x3E
#define R1_LUMP_TRICOLL_TRIS						0x42
#define R1_LUMP_TRICOLL_NODES						0x44
#define R1_LUMP_TRICOLL_HEADERS						0x45
#define R1_LUMP_PHYSICS_TRIANGLES					0x46
#define R1_LUMP_VERTEX_UNLIT						0x47
#define R1_LUMP_VERTEX_LIT_FLAT						0x48
#define R1_LUMP_VERTEX_LIT_BUMP						0x49
#define R1_LUMP_VERTEX_UNLIT_TS						0x4A
#define R1_LUMP_VERTEX_BLINN_PHONG					0x4B
#define R1_LUMP_VERTEX_RESERVED_5					0x4C
#define R1_LUMP_VERTEX_RESERVED_6					0x4D
#define R1_LUMP_VERTEX_RESERVED_7					0x4E
#define R1_LUMP_MESH_INDICES						0x4F
#define R1_LUMP_MESHES								0x50
#define R1_LUMP_MESH_BOUNDS							0x51
#define R1_LUMP_MATERIAL_SORT						0x52
#define R1_LUMP_LIGHTMAP_HEADERS					0x53
#define R1_LUMP_CM_GRID								0x55
#define R1_LUMP_CM_GRID_CELLS						0x56
#define R1_LUMP_CM_GRID_SETS						0x57
#define R1_LUMP_CM_GEO_SET_BOUNDS					0x58
#define R1_LUMP_CM_PRIMITIVES						0x59
#define R1_LUMP_CM_PRIMITIVE_BOUNDS					0x5A
#define R1_LUMP_CM_UNIQUE_CONTENTS					0x5B
#define R1_LUMP_CM_BRUSHES							0x5C
#define R1_LUMP_CM_BRUSH_SIDE_PLANE_OFFSETS			0x5D
#define R1_LUMP_CM_BRUSH_SIDE_PROPS					0x5E
#define R1_LUMP_CM_BRUSH_TEX_VECS					0x5F
#define R1_LUMP_TRICOLL_BEVEL_STARTS				0x60
#define R1_LUMP_TRICOLL_BEVEL_INDICES				0x61
#define R1_LUMP_LIGHTMAP_DATA_SKY					0x62
#define R1_LUMP_CSM_AABB_NODES						0x63
#define R1_LUMP_CSM_OBJ_REFERENCES					0x64
#define R1_LUMP_LIGHTPROBES							0x65
#define R1_LUMP_STATIC_PROP_LIGHTPROBE_INDICES		0x66
#define R1_LUMP_LIGHTPROBE_TREE						0x67
#define R1_LUMP_LIGHTPROBE_REFERENCES				0x68
#define R1_LUMP_LIGHTMAP_DATA_REAL_TIME_LIGHTS		0x69
#define R1_LUMP_CELL_BSP_NODES						0x6A
#define R1_LUMP_CELLS								0x6B
#define R1_LUMP_PORTALS								0x6C
#define R1_LUMP_PORTAL_VERTICES						0x6D
#define R1_LUMP_PORTAL_EDGES						0x6E
#define R1_LUMP_PORTAL_VERTEX_EDGES					0x6F
#define R1_LUMP_PORTAL_VERTEX_REFERENCES			0x70
#define R1_LUMP_PORTAL_EDGE_REGERENCES				0x71
#define R1_LUMP_PORTAL_EDGE_INTERSECT_AT_EDGE		0x72
#define R1_LUMP_PORTAL_EDGE_INTERSECT_AT_VERTEX		0x73
#define R1_LUMP_PORTAL_EDGE_INTERSECT_HEADER		0x74
#define R1_LUMP_OCCLUSION_MESH_VERTICES				0x75
#define R1_LUMP_OCCLUSION_MESH_INDICES				0x76
#define R1_LUMP_CELL_AABB_NODES						0x77
#define R1_LUMP_OBJ_REFERENCES						0x78
#define R1_LUMP_OBJ_REFERENCE_BOUNDS				0x79
#define R1_LUMP_LEVEL_INFO							0x7B
#define R1_LUMP_SHADOW_MESH_OPAQUE_VERTICES			0x7C
#define R1_LUMP_SHADOW_MESH_ALPHA_VERTICES			0x7D
#define R1_LUMP_SHADOW_MESH_INDICES					0x7E
#define R1_LUMP_SHADOW_MESHES						0x7F


/*
   LoadR2BSPFile()
   loads a r1 bsp file
 */
void LoadR1BSPFile(const char* filename)
{

}


/*
   WriteR2BSPFile()
   writes a r1 bsp file
 */
void WriteR1BSPFile( const char* filename ) {
	rbspHeader_t header{};

	// Set up header
	memcpy( header.ident, g_game->bspIdent, 4 );
	header.version = LittleLong( g_game->bspVersion );
	header.mapVersion = 30;
	header.maxLump = 127;

	// write initial header
	FILE* file = SafeOpenWrite( filename );
	SafeWrite( file, &header, sizeof(header) );


	/* :) */
	{
		char message[64] = "Built with love using MRVN-radiant :)";
		SafeWrite( file, &message, sizeof(message) );
	}
	{
		char message[64];
		strncpy( message,StringOutputStream(64)("Version:        ", Q3MAP_VERSION).c_str(), 64 );
		SafeWrite( file, &message, sizeof(message) );
	}
	{
		time_t t;
		time( &t );
		char message[64];
		strncpy( message,StringOutputStream(64)("Time:           ", asctime(localtime(&t))).c_str(), 64 );
		SafeWrite( file, &message, sizeof(message) );
	}
	
	/* Write lumps */
	AddLump(file, header.lumps[R1_LUMP_ENTITIES],							Titanfall::Bsp::entities);
	AddLump(file, header.lumps[R1_LUMP_VERTICES],							Titanfall::Bsp::vertices);
	AddLump(file, header.lumps[R1_LUMP_VERTEX_NORMALS],						Titanfall::Bsp::vertexNormals);
	AddLump(file, header.lumps[R1_LUMP_TEXTURE_DATA_STRING_DATA],			Titanfall::Bsp::textureDataData);
	AddLump(file, header.lumps[R1_LUMP_TEXTURE_DATA_STRING_TABLE],			Titanfall::Bsp::textureDataTable);
	AddLump(file, header.lumps[R1_LUMP_VERTEX_LIT_BUMP],					Titanfall::Bsp::vertexLitBumpVertices);
	AddLump(file, header.lumps[R1_LUMP_MESH_INDICES],						Titanfall::Bsp::meshIndices);
	AddLump(file, header.lumps[R1_LUMP_MESHES],								Titanfall::Bsp::meshes);
	AddLump(file, header.lumps[R1_LUMP_MESH_BOUNDS],						Titanfall::Bsp::meshBounds);
	AddLump(file, header.lumps[R1_LUMP_MATERIAL_SORT],						Titanfall::Bsp::materialSorts);


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
	CompileR1BSPFile
	Compiles a v29 bsp file
*/
void CompileR1BSPFile() {
	for ( std::size_t entityNum = 0; entityNum < entities.size(); ++entityNum )
	{
		// Get entity
		entity_t& entity = entities[entityNum];
		const char* classname = entity.classname();

		EmitEntity( entity );

		// Visible geo
		if ( striEqual( classname,"worldspawn" ) )
		{
			// Generate bsp meshes from map brushes
			Shared::MakeMeshes( entity );
			Titanfall::EmitMeshes( entity );
		}
	}
}

/*
	EmitTextureData()
	writes the entitiy partitions
*/
void Titanfall::EmitTextureData( shaderInfo_t shader ) {
	std::string tex;
	std::size_t index;
	tex = shader.shader.c_str();

	savedTextures.push_back( tex );

	tex.erase(tex.begin(), tex.begin() + 9);
	std::replace(tex.begin(), tex.end(), '/', '\\');

	index = tex.find('\\');
	if (index != std::string::npos)
	{
		std::replace(tex.begin(), tex.begin() + index, '_', '\\');
	}

	/* Check if it's already saved */
	std::string table = std::string( Titanfall::Bsp::textureDataData.begin(), Titanfall::Bsp::textureDataData.end() );
	index = table.find(tex);
	if (index != std::string::npos)
		return;

	/* Add to Table */
	StringOutputStream data;
	data << tex.c_str();
	std::vector<char> str = { data.begin(), data.end() + 1 };

	Titanfall::Bsp::textureDataTable.emplace_back( Titanfall::Bsp::textureDataData.size() );
	Titanfall::Bsp::textureDataData.insert( Titanfall::Bsp::textureDataData.end(), str.begin(), str.end() );

	Titanfall::TextureData_t &td = Titanfall::Bsp::textureData.emplace_back();
	td.name_index = Titanfall::Bsp::textureDataTable.size() - 1;
	td.sizeX = shader.shaderImage->width;
	td.sizeY = shader.shaderImage->height;
	td.visibleX = shader.shaderImage->width;
	td.visibleY = shader.shaderImage->height;
	td.flags = 512; // This should be the same as the mesh indexing this textureData
}

/*
	EmitVertex
	Saves a vertex into Titanfall::vertices and returns its index
*/
uint32_t Titanfall::EmitVertex( Vector3 &vertex ) {
	for( uint32_t i = 0; i < (uint32_t)Titanfall::Bsp::vertices.size(); i++ ) {
		if( VectorCompare( vertex, Titanfall::Bsp::vertices.at( i ) ) )
			return i;
	}

	Titanfall::Bsp::vertices.emplace_back( vertex );
	return (uint32_t)Titanfall::Bsp::vertices.size() - 1;
}

/*
	EmitVertexNormal
	Saves a vertex normal into Titanfall::vertexNormals and returns its index
*/
uint32_t Titanfall::EmitVertexNormal( Vector3 &normal ) {
	for( uint32_t i = 0; i < (uint32_t)Titanfall::Bsp::vertexNormals.size(); i++ ) {
		if( VectorCompare( normal, Titanfall::Bsp::vertexNormals.at( i ) ) )
			return i;
	}

	Titanfall::Bsp::vertexNormals.emplace_back( normal );
	return (uint32_t)Titanfall::Bsp::vertexNormals.size() - 1;
}

/*
	EmitEntityPartitions()
	Writes entitiy partitions respawn uses
*/
void Titanfall::EmitEntityPartitions() {
	std::string partitions = "01* env fx script snd spawn";
	Titanfall::Bsp::entityPartitions = { partitions.begin(), partitions.end() };
}

/*
	EmitMeshes()
	writes the mesh list to the bsp
*/
void Titanfall::EmitMeshes( const entity_t &e ) {
	for ( const Shared::mesh_t &mesh : Shared::meshes ) {
		Titanfall::Mesh_t &bm = Titanfall::Bsp::meshes.emplace_back();
		bm.const0 = 4294967040; // :)
		bm.flags = 1051136;
		bm.firstVertex = Titanfall::Bsp::vertexLitBumpVertices.size();
		bm.vertexCount = mesh.vertices.size();
		bm.triOffset = Titanfall::Bsp::meshIndices.size();
		bm.triCount = mesh.triangles.size() / 3;


		// Emit textrue related structs
		Titanfall::EmitTextureData( *mesh.shaderInfo );

		bm.materialOffset = Titanfall::EmitMaterialSort( mesh.shaderInfo->shader.c_str() );

		MinMax aabb;

		// Save vertices and vertexnormals
		for ( std::size_t i = 0; i < mesh.vertices.size(); i++ ) {
			Shared::vertex_t vertex = mesh.vertices.at( i );
			// Check against aabb
			aabb.extend( vertex.xyz );

			Titanfall::VertexLitBump_t &lv = Titanfall::Bsp::vertexLitBumpVertices.emplace_back();
			lv.uv0 = vertex.st;
			lv.negativeOne = -1;
			lv.vertexIndex = Titanfall::EmitVertex( vertex.xyz );
			lv.normalIndex = Titanfall::EmitVertexNormal( vertex.normal );
		}

		// Save triangles
		for ( uint16_t triangle : mesh.triangles ) {
			for ( uint32_t j = 0; j < Titanfall::Bsp::vertexLitBumpVertices.size(); j++ ) {
				if ( !VectorCompare( Titanfall::Bsp::vertices.at( Titanfall::Bsp::vertexLitBumpVertices.at( j ).vertexIndex ),mesh.vertices.at( triangle ).xyz ) )
					continue;
				
				if ( !VectorCompare( Titanfall::Bsp::vertexNormals.at( Titanfall::Bsp::vertexLitBumpVertices.at( j ).normalIndex ), mesh.vertices.at( triangle ).normal ) )
					continue;
				
				uint16_t& index = Titanfall::Bsp::meshIndices.emplace_back();
				index = j;
				break;
			}
		}

		// Save MeshBounds
		Titanfall::MeshBounds_t& mb = Titanfall::Bsp::meshBounds.emplace_back();
		mb.origin = ( aabb.maxs + aabb.mins ) / 2;
		mb.extents = ( aabb.maxs - aabb.mins ) / 2;
	}
}

/*
	EmitMaterialSort()
	Tries to create a material sort of the last texture
*/
uint16_t Titanfall::EmitMaterialSort( const char* texture ) {
	std::string tex = texture;

	std::string textureData = { Titanfall::Bsp::textureDataData.begin(), Titanfall::Bsp::textureDataData.end() };

	/* Find the texture path in the textureData lump */
	uint16_t index = 0;
	for ( std::string &path : savedTextures ) {
		if ( path == tex )
			break;

		index++;
	}

	/* Check if the material sort we need already exists */
	std::size_t pos = 0;
	for ( Titanfall::MaterialSort_t &ms : Titanfall::Bsp::materialSorts ) {
		if ( ms.textureData == index )
			return pos;

		pos++;
	}
	
	
	Titanfall::MaterialSort_t &ms = Titanfall::Bsp::materialSorts.emplace_back();
	ms.textureData = index;

	return Titanfall::Bsp::materialSorts.size() - 1;
}
