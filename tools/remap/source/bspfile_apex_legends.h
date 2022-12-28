#pragma once

#include "qmath.h"
#include <cstdint>
#include "remap.h"

void                        LoadR5BSPFile(const char* filename);
void                        WriteR5BSPFile(const char* filename);
void						CompileR5BSPFile();


namespace ApexLegends {
	void EmitStubs();

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

	// GameLump Stub
	struct GameLump_Stub_t {
		uint32_t version = 1;
		char magic[4];
		uint32_t const0 = 3080192;
		uint32_t offset;
		uint32_t length = 20;
		uint32_t zeros[5] = { 0,0,0,0,0 };
	};

	namespace Bsp {
		inline std::vector<VertexLitBump_t> vertexLitBumpVertices;
		inline std::vector<Mesh_t> meshes;

		// Stubs
		inline std::vector<uint8_t> entities_stub;
		inline std::vector<uint8_t> textureData_stub;
		inline std::vector<uint8_t> vertices_stub;
		inline std::vector<uint8_t> lightprobeParentInfos_stub;
		inline std::vector<uint8_t> shadowEnvironments_stub;
		inline std::vector<uint8_t> models_stub;
		inline std::vector<uint8_t> surfaceNames_stub;
		inline std::vector<uint8_t> contentsMasks_stub;
		inline std::vector<uint8_t> surfaceProperties_stub;
		inline std::vector<uint8_t> bvhNodes_stub;
		inline std::vector<uint8_t> bvhNodeLeafData_stub;
		inline std::vector<uint8_t> entityPartitions_stub;
		inline std::vector<uint8_t> vertexNormals_stub;
		inline std::vector<uint8_t> gameLump_stub;
		inline std::vector<uint8_t> unknown25_stub;
		inline std::vector<uint8_t> unknown26_stub;
		inline std::vector<uint8_t> unknown27_stub;
		inline std::vector<uint8_t> cubemaps_stub;
		inline std::vector<uint8_t> worldLights_stub;
		inline std::vector<uint8_t> vertexUnlit_stub;
		inline std::vector<uint8_t> meshIndicies_stub;
		inline std::vector<uint8_t> meshes_stub;
		inline std::vector<uint8_t> meshBounds_stub;
		inline std::vector<uint8_t> materialSort_stub;
		inline std::vector<uint8_t> lightmapHeaders_stub;
		inline std::vector<uint8_t> tweakLights_stub;
		inline std::vector<uint8_t> lightmapDataSky_stub;
		inline std::vector<uint8_t> csmAABBNodes_stub;
		inline std::vector<uint8_t> cellBspNodes_stub;
		inline std::vector<uint8_t> cells_stub;
		inline std::vector<uint8_t> cellAABBNodes_stub;
		inline std::vector<uint8_t> objReferences_stub;
		inline std::vector<uint8_t> objReferenceBounds_stub;
		inline std::vector<uint8_t> lightmapDataRTLPage_stub;
		inline std::vector<uint8_t> levelInfo_stub;
	}
}