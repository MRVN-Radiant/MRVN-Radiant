
#include <list>
#include "bspfile_abstract.h"
#include "remap.h"
#include "lump_names.h"

namespace Titanfall {
	void LoadLumpsAndEntities(const char* filename);
	void ParseLoadedBSP();
	void ParseWorldspawn( entity_t &entity );
	void ParseBrush( entity_t &entity, std::size_t index );
	void ParsePatch( entity_t &entity, std::size_t index );
	std::vector<Plane3> BuildPlanesFromMinMax( MinMax &minmax );
}