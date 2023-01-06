
#include <list>
#include "bspfile_abstract.h"
#include "remap.h"
#include "lump_names.h"

namespace Titanfall {
	void LoadLumpsAndEntities(const char* filename);
	void ParseLoadedBSP();
	void ParseWorldspawn( entity_t &entity );
	std::vector<Plane3> BuildPlanesFromMinMax( MinMax &minmax );
}