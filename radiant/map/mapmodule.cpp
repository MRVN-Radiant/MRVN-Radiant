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
#include "mapmodule.h"

#include "imap.h"
#include "iscriplib.h"
#include "ibrush.h"
#include "ipatch.h"
#include "ifiletypes.h"
#include "ieclass.h"
#include "qerplugin.h"

#include "scenelib.h"
#include "string/string.h"
#include "stringio.h"
#include "generic/constant.h"

#include "modulesystem/singletonmodule.h"

#include "mapwriter.h"

class MapDependencies :
	public GlobalRadiantModuleRef,
	public GlobalBrushModuleRef,
	public GlobalPatchModuleRef,
	public GlobalFiletypesModuleRef,
	public GlobalScripLibModuleRef,
	public GlobalEntityClassManagerModuleRef,
	public GlobalSceneGraphModuleRef
{
public:
	MapDependencies() :
		GlobalBrushModuleRef(GlobalRadiant().getRequiredGameDescriptionKeyValue("brushtypes")),
		GlobalPatchModuleRef(GlobalRadiant().getRequiredGameDescriptionKeyValue("patchtypes")),
		GlobalEntityClassManagerModuleRef(GlobalRadiant().getRequiredGameDescriptionKeyValue("entityclass")) {
	}
};

class MapMrvnAPI final : public TypeSystemRef, public MapFormat//, public PrimitiveParser
{
public:
	typedef MapFormat Type;
	STRING_CONSTANT(Name, "map-mrvn");
	INTEGER_CONSTANT(MapVersion, 1);

	MapMrvnAPI() {
		GlobalFiletypesModule::getTable().addType(Type::Name, Name, filetype_t("mrvn maps", "*.mrvn-map", true, true, true));
	}
	MapFormat* getTable() {
		return this;
	}

	void readGraph(scene::Node& root, TextInputStream& inputStream, EntityCreator& entityTable) const {
		Tokeniser& tokeniser = GlobalScripLibModule::getTable().m_pfnNewSimpleTokeniser(inputStream);
		//Map_Read(root, tokeniser, entityTable, *this);
		tokeniser.release();
	}
	void writeGraph(scene::Node& root, GraphTraversalFunc traverse, TextOutputStream& outputStream) const {
		TokenWriter& writer = GlobalScripLibModule::getTable().m_pfnNewSimpleTokenWriter(outputStream);
		writer.writeToken("MapInfo");
		writer.nextLine();
		writer.writeToken("{");
		writer.nextLine();
		writer.writeToken("\t\"Version\"");
		writer.writeInteger(MapVersion);
		writer.nextLine();
		writer.writeToken("}");
		writer.nextLine();
		Map_Write(root, traverse, writer);
		writer.release();
	}
};

typedef SingletonModule<MapMrvnAPI, MapDependencies> MapMrvnModule;
typedef Static<MapMrvnModule> StaticMapMrvnModule;
StaticRegisterModule StaticRegisterMapMrvnModule(StaticMapMrvnModule::instance());