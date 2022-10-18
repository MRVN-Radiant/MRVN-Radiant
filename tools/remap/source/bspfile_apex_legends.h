#pragma once

#include "qmath.h"
#include <cstdint>
#include "remap.h"

void                        LoadR5BSPFile(const char* filename);
void                        WriteR5BSPFile(const char* filename);
void						CompileR5BSPFile();


namespace ApexLegends {
	void EmitMeshes( const entity_t& e );


	using Vertex_t = Vector3;

	using VertexNormal_t = Vector3;

	using MeshIndex_t = uint16_t;

    struct VertexLitBump_t {
		uint32_t vertexIndex;
		uint32_t normalIndex;
		Vector2 uv0;
		int32_t negativeOne;
		Vector2 uv1;
		char color[4];
	};

	struct Mesh_t
	{
		uint32_t triangleOffset;
		uint16_t triangleCount;
		uint16_t unknown[8];
		uint16_t materialOffset;
		uint32_t flags;
	};

	namespace Bsp {
		inline std::vector<VertexLitBump_t> vertexLitBumpVertices;
		inline std::vector<Mesh_t> meshes;
	}
}