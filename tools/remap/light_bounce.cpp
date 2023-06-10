/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "remap.h"


/* functions */



inline float Modulo1IfNegative( float f ){
	return f < 0.0f ? f - floor( f ) : f;
}


/*
   RadSampleImage()
   samples a texture image for a given color
   returns false if pixels are bad
 */

bool RadSampleImage( const byte *pixels, int width, int height, const Vector2& st, Color4f& color ){
	int x, y;

	/* clear color first */
	color.set( 255 );

	/* dummy check */
	if ( pixels == NULL || width < 1 || height < 1 ) {
		return false;
	}

	/* get offsets */
	x = ( (float) width * Modulo1IfNegative( st[ 0 ] ) ) + 0.5f;
	x %= width;
	y = ( (float) height * Modulo1IfNegative( st[ 1 ] ) ) + 0.5f;
	y %= height;

	/* get pixel */
	pixels += ( y * width * 4 ) + ( x * 4 );
	VectorCopy( pixels, color.rgb() );
	color.alpha() = pixels[ 3 ];

	if ( texturesRGB ) {
		color[0] = Image_LinearFloatFromsRGBFloat( color[0] * ( 1.0 / 255.0 ) ) * 255.0;
		color[1] = Image_LinearFloatFromsRGBFloat( color[1] * ( 1.0 / 255.0 ) ) * 255.0;
		color[2] = Image_LinearFloatFromsRGBFloat( color[2] * ( 1.0 / 255.0 ) ) * 255.0;
	}

	return true;
}



/*
   RadSample()
   samples a fragment's lightmap or vertex color and returns an
   average color and a color gradient for the sample
 */

#define MAX_SAMPLES         150
#define SAMPLE_GRANULARITY  6





/*
   RadSubdivideDiffuseLight()
   subdivides a radiosity winding until it is smaller than subdivide, then generates an area light
 */

#define RADIOSITY_MAX_GRADIENT      0.75f   //%	0.25f
#define RADIOSITY_VALUE             500.0f
#define RADIOSITY_MIN               0.0001f
#define RADIOSITY_CLIP_EPSILON      0.125f





/*
   RadLightForTriangles()
   creates unbounced diffuse lights for triangle soup (misc_models, etc)
 */

void RadLightForTriangles( int num, int lightmapNum, rawLightmap_t *lm, const shaderInfo_t *si, float scale, float subdivide, clipWork_t *cw ){
	
}



/*
   RadLightForPatch()
   creates unbounced diffuse lights for patches
 */

#define PLANAR_EPSILON  0.1f

void RadLightForPatch( int num, int lightmapNum, rawLightmap_t *lm, const shaderInfo_t *si, float scale, float subdivide, clipWork_t *cw ){
	
}







/*
   RadCreateDiffuseLights()
   creates lights for unbounced light on surfaces in the bsp
 */

void RadCreateDiffuseLights(){
	
}
