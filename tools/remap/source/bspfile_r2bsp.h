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

namespace r2
{
	using bspVertex_t = Vector3;

	using bspVertexNormal_t = Vector3;

	using bspMeshIndex_t = uint16_t;

	using bspPlane_t = Plane3f;


	struct bspVertexLitBump_t
	{
		uint32_t vertex_index;
		uint32_t normal_index;
		Vector2 uv0;
		int32_t minus_one;
		Vector2 uv1;
		uint32_t unk[4];
	};


	struct bspMesh_t
	{
		uint32_t tri_offset;
		uint16_t tri_count;
		uint16_t first_vertex;
		uint16_t vertex_count;
		uint16_t unk0;
		uint32_t const0;
		uint16_t unk1;
		uint16_t unk2;
		uint16_t unk3;
		uint16_t material_offset;
		uint32_t flags;
	};

	/* From testing on r2 the whole lump can be zeroed as long as its parallel with bspMeshes */
	struct bspMeshBounds_t
	{
		Vector3 origin;
		uint32_t unk0;
		Vector3 extents;
		uint32_t unk1;
	};

	struct bspEntityPartitions_t
	{
		char partitions[27];
	};

	struct bspModel_t_new
	{
		MinMax minmax;
		uint32_t first_mesh;
		uint32_t mesh_count;
	};

	struct bspObjReferenceBounds_t
	{
		Vector3 mins;
		uint32_t zero0;
		Vector3 maxs;
		uint32_t zero1;
	};

	using bspObjReferences_t = uint16_t;

	struct bspLevelInfo_t
	{
		uint32_t unk0;
		uint32_t unk1;
		uint32_t unk3;
		uint32_t prop_count;
		Vector3 unk2;
	};

	struct CellAABBNode_t
	{
		Vector3 mins;
		uint8_t child_count; // number of direct children this node has
		uint8_t obj_ref_count; // number of obj_refs it indexes
		uint16_t total_child_count; // nuber of children all direct children have
		Vector3 maxs;
		uint16_t first_child; // first child index
		uint16_t obj_ref; // first obj_ref index
	};

	struct GameLump_Path
	{
		char path[128];
	};

	struct GameLump_Prop
	{
		Vector3 Origin;
		Vector3 Angles;
		float scale;
		uint16_t model_name;
		uint8_t solid_mode;
		uint8_t flags;
		int8_t unk[4];
		float fade_scale;
		Vector3 lighting_origin;
		int8_t cpu_level[2];
		int8_t gpu_level[2];
		int8_t diffuse_modulation[4];
		uint16_t collision_flags[2];
	};

	struct GameLump_t
	{
		uint32_t version; // 1
		char magic[4]; // "prps"
		uint32_t const0; // always 851968
		uint32_t offset; // offset to path_count
		uint32_t length;
		uint32_t path_count;
		std::vector<GameLump_Path> paths;
		uint32_t prop_count;
		uint32_t unk3; // From testing can be same as prop_count
		uint32_t unk4; // Same for this one
		std::vector<GameLump_Prop> props;
	};

	struct bspBrush_t
	{
		Vector3 origin;
		uint16_t flags;
		uint16_t index;
		Vector3 extents;
		uint32_t side_plane;
	};

	struct bspMaterialSort_t
	{
		uint16_t texture_data;
		uint16_t lightmap_header;
		uint16_t cubemap;
		uint16_t unk;
		uint32_t vertex_offset;
	};

	struct bspTextureData_t
	{
		Vector3 reflectivity;
		uint32_t name_index;
		uint32_t size_x;
		uint32_t size_y;
		uint32_t visible_x;
		uint32_t visible_y;
		uint32_t flags;
	};

	
	inline std::vector<bspTextureData_t> bspTextureData;

	inline std::vector<bspVertex_t> bspVertices;

	inline std::vector<bspVertexNormal_t> bspVertexNormals;

	inline std::vector<bspVertexLitBump_t> bspVertexLitBump;

	inline std::vector<bspMeshIndex_t> bspMeshIndices;

	inline std::vector<bspMesh_t> bspMeshes;

	inline std::vector<bspMeshBounds_t> bspMeshBounds;

	inline std::vector<bspEntityPartitions_t> bspEntityPartitions;

	inline std::vector<bspModel_t_new> bspModels_new;

	inline std::vector<bspObjReferenceBounds_t> bspObjReferenceBounds;

	inline std::vector<bspObjReferences_t> bspObjReferences;

	inline std::vector<bspLevelInfo_t> bspLevelInfo;

	inline std::vector<uint16_t> bspBrushSidePlanes;

	inline std::vector<char> bspTextureDataData;

	inline std::vector<uint32_t> bspTextureDataTable;

	inline std::vector<bspMaterialSort_t> bspMaterialSorts;

	inline std::vector<bspPlane_t> bspPlanes;

	inline std::vector<bspBrush_t> bspBrushes;

	/* Stubs */
	inline std::vector<char> bspEntities_stub;

	inline std::vector<uint8_t> bspPlanes_stub;

	inline std::vector<uint8_t> bspTextureData_stub;

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

	inline std::vector<uint8_t> bspCMBrushSidePlaneOffsets_stub;

	inline std::vector<uint8_t> bspCMBrushSideProps_stub;

	inline std::vector<uint8_t> bspCMBrushTexVecs_stub;

	inline std::vector<uint8_t> bspTricollBevelStarts_stub;

	inline std::vector<uint8_t> bspLightMapDataSky_stub;

	inline std::vector<uint8_t> bspCSMAABBNodes_stub;

	inline std::vector<uint8_t> bspCellBSPNodes_stub;

	inline std::vector<uint8_t> bspCells_stub;

	inline std::vector<uint8_t> bspPortals_stub;

	inline std::vector<uint8_t> bspPortalVertices_stub;

	inline std::vector<uint8_t> bspPortalEdges_stub;

	inline std::vector<uint8_t> bspPortalVertexEdges_stub;

	inline std::vector<uint8_t> bspPortalVertexReferences_stub;

	inline std::vector<uint8_t> bspPortalEdgeReferences_stub;

	inline std::vector<uint8_t> bspPortalEdgeIntersectEdge_stub;

	inline std::vector<uint8_t> bspPortalEdgeIntersectAtVertex_stub;

	inline std::vector<uint8_t> bspPortalEdgeIntersectHeader_stub;

	inline std::vector<CellAABBNode_t> bspCellAABBNodes_stub;

	inline GameLump_t GameLump;
}