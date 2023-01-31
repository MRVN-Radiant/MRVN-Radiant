/* -------------------------------------------------------------------------------

   Copyright (C) 2022-2023 MRVN-Radiant and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of MRVN-Radiant.

   MRVN-Radiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   MRVN-Radiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ------------------------------------------------------------------------------- */


#include <list>
#include "bspfile_abstract.h"
#include "remap.h"
#include "lump_names.h"

namespace Titanfall {
	void LoadLumpsAndEntities( rbspHeader_t *header, const char *filename );
	void ParseLoadedBSP();
	void LoadAndParseGameLump( rbspHeader_t *header );
	void ParseExtraBrushes( entity_t &entity );
	void ParseWorldspawn( entity_t &entity );
	void ParseGridCells( entity_t &entity, std::size_t index, std::size_t count );
	void ParseBrush( entity_t &entity, std::size_t index );
	void ParsePatch( entity_t &entity, std::size_t index );
	std::vector<Plane3> BuildPlanesFromMinMax( MinMax &minmax );
}