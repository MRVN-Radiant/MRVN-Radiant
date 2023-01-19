
#include <list>
#include "bspfile_abstract.h"
#include "remap.h"
#include "lump_names.h"

namespace Titanfall {
	void LoadLumpsAndEntities( rbspHeader_t *header, const char *filename );
	void ParseLoadedBSP();
	void ParseExtraBrushes( entity_t &entity );
	void ParseWorldspawn( entity_t &entity );
	void ParseGridCells( entity_t &entity, std::size_t index, std::size_t count );
	void ParseBrush( entity_t &entity, std::size_t index );
	void ParsePatch( entity_t &entity, std::size_t index );
	std::vector<Plane3> BuildPlanesFromMinMax( MinMax &minmax );
}