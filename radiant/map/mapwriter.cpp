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
#include "mapwriter.h"

#include "ientity.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CBrushExporter* Node_getBrushExporter(scene::Node& node) {
	return NodeTypeCast<CBrushExporter>::cast(node);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CPatchExporter* Node_getPatchExporter(scene::Node& node) {
	return NodeTypeCast<CPatchExporter>::cast(node);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class WriteTokensWalker : public scene::Traversable::Walker
{
	mutable Stack<bool> m_stack;
	TokenWriter& m_writer;
public:
	WriteTokensWalker(TokenWriter& writer)
		: m_writer(writer) {
	}
	bool pre(scene::Node& node) const
	{
		m_stack.push(false);

		if (Node_isEntity(node))
		{
			Entity* pEntity = Node_getEntity(node);
			ASSERT_NOTNULL(pEntity);

			m_writer.writeToken("Entity");
			m_writer.nextLine();

			m_writer.writeToken("{");
			m_writer.nextLine();

			m_stack.top() = true;

			// TODO [Fifty]: Rework this
			class WriteKeyValue : public Entity::Visitor
			{
				TokenWriter& m_writer;
			public:
				WriteKeyValue(TokenWriter& writer)
					: m_writer(writer) {
				}

				void visit(const char* key, const char* value) {
					m_writer.writeString(key);
					m_writer.writeString(value);
					m_writer.nextLine();
				}

			} visitor(m_writer);

			pEntity->forEachKeyValue(visitor);
		}
		else if(Node_isBrush(node))
		{
			CBrushExporter* exporter = Node_getBrushExporter(node);
			ASSERT_NOTNULL(exporter);
			exporter->ExportBrush(m_writer);
		}
		else if (Node_isPatch(node))
		{
			CPatchExporter* pExporter = Node_getPatchExporter(node);
			ASSERT_NOTNULL(pExporter);
			pExporter->ExportPatch(m_writer);
		}

		return true;
	}
	void post(scene::Node& node) const {
		if (m_stack.top()) {
			m_writer.writeToken("}");
			m_writer.nextLine();
		}
		m_stack.pop();
	}
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void Map_Write(scene::Node& root, GraphTraversalFunc traverse, TokenWriter& writer)
{
	writer.writeToken("World");
	writer.nextLine();
	writer.writeToken("{");
	writer.nextLine();
	traverse(root, WriteTokensWalker(writer));
	writer.writeToken("}");
	writer.nextLine();
}