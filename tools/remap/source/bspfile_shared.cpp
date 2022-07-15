
#include "remap.h"
#include "bspfile_abstract.h"




/*
	EmitMeshes()
	Emits shared meshes for a given entity
*/
void shared::EmitMeshes( const entity_t &e )
{
	/* Multiple entities can have meshes, make sure we clear before making new meshes */
	shared::meshes.clear();

	Sys_FPrintf(SYS_VRB, "Making shared meshes\n");

	/* Brushes */
	for ( const brush_t &brush : e.brushes )
	{
		/* loop through sides */
		for ( const side_t &side : brush.sides )
		{
			/* Skip bevels */
			if ( side.bevel )
				continue;

			/* This should check flags, but those dont work rn */
			if ( striEqual( side.shaderInfo->shader.c_str(), "textures/common/caulk" ) )
				continue;
			
			shared::mesh_t &mesh = shared::meshes.emplace_back();
			mesh.shaderInfo = side.shaderInfo;

			/* Loop through vertices */
			for ( const Vector3 &vertex : side.winding )
			{
				/* Update AABB */
				mesh.minmax.extend( vertex );

				/* Texturing */
				/* this is taken from surface.cpp, It doesn't work :) */
				Vector2 st;
				Vector3 vTranslated, texX, texY;
				float x, y;
				ComputeAxisBase(side.plane.normal(), texX, texY);
				vTranslated = vertex + e.originbrush_origin;
				x = vector3_dot(vTranslated, texX);
				y = vector3_dot(vTranslated, texY);
				st[0] = side.texMat[0][0] * x + side.texMat[0][1] * y + side.texMat[0][2];
				st[1] = side.texMat[1][0] * x + side.texMat[1][1] * y + side.texMat[1][2];

				/* Normal */
				Vector3 normal;
				std::vector<Vector3> sideNormals;
				/* Find neighbouring sides and save their normals */
				for ( const side_t &s : brush.sides )
				{
					for ( const Vector3 &v : s.winding )
					{
						if ( VectorCompare( vertex, v ) )
						{
							sideNormals.push_back( s.plane.normal() );
							break;
						}
					}
				}

				/* Add up normals, then normalize them */
				for ( const Vector3 &n : sideNormals )
					normal += n;

				VectorNormalize(normal);

				/* Add vertex to mesh */
				shared::vertex_t &sv = mesh.vertices.emplace_back();
				sv.xyz = vertex;
				sv.st = st;
				sv.normal = normal;
			}

			/* Create triangles for side */
			for ( std::size_t i = 0; i < side.winding.size() - 2; i++ )
			{
				for ( int j = 0; j < 3; j++ )
				{
					int vert_index = j == 0 ? 0 : i + j;
					mesh.triangles.emplace_back( vert_index );
				}
			}
		}
	}

	/* Patches */

	/* Combine */
	std::size_t index = 0;
	std::size_t iterationsSinceCombine = 0;
	while ( true )
	{
		/* std::out_of_range can't comprehend this  */
		if ( index >= shared::meshes.size() )
			index = 0;

		/* No more meshes to combine; break from the loop */
		if ( iterationsSinceCombine >= shared::meshes.size() )
			break;

		/* Get mesh which we then compare to the rest, maybe combine, maybe not */
		shared::mesh_t &mesh1 = shared::meshes.at( index );

		for ( std::size_t i = 0; i < shared::meshes.size(); i++ )
		{
			/* We dont want to compare the same mesh */
			if ( index == i )
				continue;

			shared::mesh_t &mesh2 = shared::meshes.at( i );

			/* check if they have the same shader */
			if ( !striEqual( mesh1.shaderInfo->shader.c_str(), mesh2.shaderInfo->shader.c_str() ) )
				continue;

			/* Check if they're intersecting */
			if ( !mesh1.minmax.test( mesh2.minmax ) )
				continue;


			/* Combine them */
			/* Triangles */
			for ( uint16_t &triIndex : mesh2.triangles )
				mesh1.triangles.emplace_back( triIndex + mesh1.vertices.size() );
			
			/* Copy over vertices */
			mesh1.vertices.insert( mesh1.vertices.end(), mesh2.vertices.begin(), mesh2.vertices.end() );

			/* Update minmax */
			mesh1.minmax.extend( mesh2.minmax );
			

			/* Delete mesh we combined as to not create duplicates */
			shared::meshes.erase( shared::meshes.begin() + i );
			iterationsSinceCombine = 0;
			break;
		}

		iterationsSinceCombine++;
		index++;
	}

	Sys_Printf("Made: %i meshes\n", shared::meshes.size());
}
