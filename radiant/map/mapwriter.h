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
#pragma once

#include "imap.h"
#include "iscriplib.h"
#include "scenelib.h"

#include "brush.h"
#include "patch.h"

void Map_Write(scene::Node& root, GraphTraversalFunc traverse, TokenWriter& writer);


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class IBrushExporter
{
public:
	STRING_CONSTANT(Name, "IBrushExporter");

	virtual void ExportBrush(TokenWriter& writer) const = 0;
};

class CBrushExporter : public IBrushExporter
{
	const Brush& m_brush;

public:
	CBrushExporter(const Brush& brush) : m_brush(brush) {
	}
	void ExportBrush(TokenWriter& writer) const {
		m_brush.evaluateBRep(); // ensure b-rep is up-to-date, so that non-contributing faces can be identified.

		if (!m_brush.hasContributingFaces()) {
			return;
		}

		writer.writeToken("Brush");
		writer.nextLine();

		writer.writeToken("{");
		writer.nextLine();

		for (Brush::const_iterator i = m_brush.begin(); i != m_brush.end(); ++i)
		{
			const Face& face = *(*i);

			if (!face.contributes())
				continue;

			// Write plane
			const FacePlane& facePlane = face.getPlane();
			for (std::size_t i = 0; i < 3; i++)
			{
				writer.writeToken("(");
				for (std::size_t j = 0; j < 3; j++)
				{
					writer.writeFloat(Face::m_quantise(facePlane.planePoints()[i][j]));
				}
				writer.writeToken(")");
			}

			// Write shader
			const FaceShader& faceShader = face.getShader();
			if (string_empty(shader_get_textureName(faceShader.getShader()))) {
				writer.writeString("NULL");
			}
			else
			{
				writer.writeString(shader_get_textureName(faceShader.getShader()));
			}

			// Write Valve 220 texture def
			const FaceTexdef& faceTexdef = face.getTexdef();

			if (!texdef_sane(faceTexdef.m_projection.m_texdef))
				globalWarningStream() << "mapwriter.cpp: Valve 220 bad texdef\n";

			writer.writeToken("[");
			writer.writeFloat(faceTexdef.m_projection.m_basis_s.x());
			writer.writeFloat(faceTexdef.m_projection.m_basis_s.y());
			writer.writeFloat(faceTexdef.m_projection.m_basis_s.z());
			writer.writeFloat(faceTexdef.m_projection.m_texdef.shift[0]);
			writer.writeToken("]");
			writer.writeToken("[");
			writer.writeFloat(faceTexdef.m_projection.m_basis_t.x());
			writer.writeFloat(faceTexdef.m_projection.m_basis_t.y());
			writer.writeFloat(faceTexdef.m_projection.m_basis_t.z());
			writer.writeFloat(faceTexdef.m_projection.m_texdef.shift[1]);
			writer.writeToken("]");
			writer.writeFloat(-faceTexdef.m_projection.m_texdef.rotate);
			writer.writeFloat(faceTexdef.m_projection.m_texdef.scale[0]);
			writer.writeFloat(faceTexdef.m_projection.m_texdef.scale[1]);

			writer.nextLine();
		}

		writer.writeToken("}");
		writer.nextLine();
	}
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class IPatchExporter
{
public:
	STRING_CONSTANT(Name, "IPatchExporter");

	virtual void ExportPatch(TokenWriter& writer) const = 0;
};

class CPatchExporter : public IPatchExporter
{
	const Patch& m_Patch;

public:
	CPatchExporter(const Patch& patch) : m_Patch(patch)
	{
	}
	void ExportPatch(TokenWriter& writer) const
	{
		writer.writeToken("Patch");
		writer.nextLine();
		writer.writeToken("{");
		writer.nextLine();

		// Dimensions + shader
		writer.writeToken("[");
		writer.writeInteger(m_Patch.getWidth());
		writer.writeInteger(m_Patch.getHeight());
		writer.writeToken("]");
		// Write shader
		const char* pszShader = m_Patch.GetShader();
		if (string_empty(pszShader)) {
			writer.writeString("NULL");
		}
		else
		{
			writer.writeString(pszShader);
		}
		writer.nextLine();

		// Write vertices
		Array<PatchControl> patchPoints = m_Patch.getControlPoints();
		for (PatchControl point : patchPoints)
		{
			writer.writeToken("(");
			writer.writeFloat(point.m_vertex.x());
			writer.writeFloat(point.m_vertex.y());
			writer.writeFloat(point.m_vertex.z());
			writer.writeToken(")");

			writer.writeToken("[");
			writer.writeFloat(point.m_texcoord.x());
			writer.writeFloat(point.m_texcoord.y());
			writer.writeToken("]");

			// [Fifty]: stub for now
			writer.writeToken("[");
			writer.writeToken("0");
			writer.writeToken("0");
			writer.writeToken("0");
			writer.writeToken("0");
			writer.writeToken("]");

			writer.nextLine();
		}

		writer.writeToken("}");
		writer.nextLine();
	}
};