#pragma once

#include "qmath.h"
#include <cstdint>
#include "remap.h"

void                        LoadR1BSPFile(const char* filename);
void                        WriteR1BSPFile(const char* filename);
void						CompileR1BSPFile();


namespace Titanfall {
	uint32_t EmitVertex( Vector3 &vertex );
	uint32_t EmitVertexNormal( Vector3 &normal );

	// 0x02
	struct TextureData_t
	{
		Vector3 reflectivity;
		uint32_t name_index;
		uint32_t sizeX;
		uint32_t sizeY;
		uint32_t visibleX;
		uint32_t visibleY;
		uint32_t flags;
	};

	// 0x0F
	struct Model_t
	{
		MinMax minmax;
		uint32_t firstMesh;
		uint32_t meshCount;
	};

	// 0x49
	struct VertexLitBump_t
	{
		uint32_t vertexIndex;
		uint32_t normalIndex;
		Vector2 uv0;
		int32_t negativeOne;
		Vector2 uv1;
		uint32_t unk[4];
	};

	// 0x50
	struct Mesh_t
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


	namespace Bsp {
		inline std::vector<char> entities;
		inline std::vector<Plane3f> planes;
		inline std::vector<TextureData_t> textureData;
		inline std::vector<Vector3> vertices;
		inline std::vector<Model_t> models;
		inline std::vector<Vector3> vertexNormals;
		inline std::vector<VertexLitBump_t> vertexLitBumpVertices;
		inline std::vector<Mesh_t> meshes;
		inline std::vector<uint16_t> meshIndices;
	}
}