#pragma once

#include "remap.h"


/*
    Stores things supported bsp versions share

    If it starts with "bsp" it gets saved in a .bsp
    If it starts with "ent" it gets saved in a .ent
    The rest are just helpers and dont get saved
*/
namespace Shared {
	/* Structs */
	struct Vertex_t {
            Vector3  xyz;
            Vector3  normal;
            Vector2  st;
	};

	struct Mesh_t {
            MinMax                 minmax;
            shaderInfo_t          *shaderInfo;
            std::vector<Vertex_t>  vertices;
            std::vector<uint16_t>  triangles;
	};

	struct visRef_t {
            MinMax    minmax;
            uint16_t  index;  // index to mesh / model
	};

	struct visNode_t {
            MinMax minmax;
            std::vector<visNode_t> children;
            std::vector<visRef_t> refs;
	};

	/* Vectors */
	inline std::vector<Mesh_t>   meshes;
	inline std::vector<visRef_t> visRefs;
	inline visNode_t             visRoot;
	/* Functions */
	void MakeMeshes(const entity_t &e);
	void MakeVisReferences();
	visNode_t MakeVisTree( std::vector<Shared::visRef_t> refs, float parentCost );
	void MergeVisTree( Shared::visNode_t &node );
}
