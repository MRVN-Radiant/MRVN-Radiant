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
#include "bspfile_rbsp.h"
#include "surface_extra.h"
#include "timer.h"




/* -------------------------------------------------------------------------------

   this file contains code that doe lightmap allocation and projection that
   runs in the -light phase.

   this is handled here rather than in the bsp phase for a few reasons--
   surfaces are no longer necessarily convex polygons, patches may or may not be
   planar or have lightmaps projected directly onto control points.

   also, this allows lightmaps to be calculated before being allocated and stored
   in the bsp. lightmaps that have little high-frequency information are candidates
   for having their resolutions scaled down.

   ------------------------------------------------------------------------------- */

/*
   WriteTGA24()
   based on WriteTGA() from qimagelib.cpp
 */

static void WriteTGA24( char *filename, byte *data, int width, int height, bool flip ){
	int i;
	const int headSz = 18;
	const int sz = width * height * 3 + headSz;
	byte    *buffer, *pix, *in;
	FILE    *file;

	/* allocate a buffer and set it up */
	buffer = safe_malloc( sz );
	pix = buffer + headSz;
	memset( buffer, 0, headSz );
	buffer[ 2 ] = 2;		// uncompressed type
	buffer[ 12 ] = width & 255;
	buffer[ 13 ] = width >> 8;
	buffer[ 14 ] = height & 255;
	buffer[ 15 ] = height >> 8;
	buffer[ 16 ] = 24;	// pixel size

	/* swap rgb to bgr */
	for ( i = 0; i < sz - headSz; i += 3 )
	{
		pix[ i + 0 ] = data[ i + 2 ];   /* blue */
		pix[ i + 1 ] = data[ i + 1 ];   /* green */
		pix[ i + 2 ] = data[ i + 0 ];   /* red */
	}

	/* write it and free the buffer */
	file = SafeOpenWrite( filename );

	/* flip vertically? */
	if ( flip ) {
		fwrite( buffer, 1, headSz, file );
		for ( in = pix + ( ( height - 1 ) * width * 3 ); in >= pix; in -= ( width * 3 ) )
			fwrite( in, 1, ( width * 3 ), file );
	}
	else{
		fwrite( buffer, 1, sz, file );
	}

	/* close the file */
	fclose( file );
	free( buffer );
}



/*
   ExportLightmaps()
   exports the lightmaps as a list of numbered tga images
 */

void ExportLightmaps(){
	int i;
	char dirname[ 1024 ], filename[ 1024 ];
	byte        *lightmap;


	/* note it */
	Sys_FPrintf( SYS_VRB, "--- ExportLightmaps ---\n" );

	/* do some path mangling */
	strcpy( dirname, source );
	StripExtension( dirname );

	/* sanity check */
	if ( bspLightBytes.empty() ) {
		Sys_Warning( "No BSP lightmap data\n" );
		return;
	}

	/* make a directory for the lightmaps */
	Q_mkdir( dirname );

	/* iterate through the lightmaps */
	for ( i = 0, lightmap = bspLightBytes.data(); lightmap < ( bspLightBytes.data() + bspLightBytes.size() ); i++, lightmap += ( g_game->lightmapSize * g_game->lightmapSize * 3 ) )
	{
		/* write a tga image out */
		sprintf( filename, "%s/lightmap_%04d.tga", dirname, i );
		Sys_Printf( "Writing %s\n", filename );
		WriteTGA24( filename, lightmap, g_game->lightmapSize, g_game->lightmapSize, false );
	}
}



/*
   ExportLightmapsMain()
   exports the lightmaps as a list of numbered tga images
 */

int ExportLightmapsMain( Args& args ){
	/* arg checking */
	if ( args.empty() ) {
		Sys_Printf( "Usage: q3map2 -export [-v] <mapname>\n" );
		return 0;
	}

	/* do some path mangling */
	strcpy( source, ExpandArg( args.takeBack() ) );
	path_set_extension( source, ".bsp" );

	/* load the bsp */
	Sys_Printf( "Loading %s\n", source );
	LoadBSPFile( source );

	/* export the lightmaps */
	ExportLightmaps();

	/* return to sender */
	return 0;
}



/*
   ImportLightmapsMain()
   imports the lightmaps from a list of numbered tga images
 */

int ImportLightmapsMain( Args& args ){
	int i, x, y, width, height;
	char dirname[ 1024 ], filename[ 1024 ];
	byte        *lightmap, *pixels, *in, *out;


	/* arg checking */
	if ( args.empty() ) {
		Sys_Printf( "Usage: q3map2 -import [-v] <mapname>\n" );
		return 0;
	}

	/* do some path mangling */
	strcpy( source, ExpandArg( args.takeBack() ) );
	path_set_extension( source, ".bsp" );

	/* load the bsp */
	Sys_Printf( "Loading %s\n", source );
	LoadBSPFile( source );

	/* note it */
	Sys_FPrintf( SYS_VRB, "--- ImportLightmaps ---\n" );

	/* do some path mangling */
	strcpy( dirname, source );
	StripExtension( dirname );

	/* sanity check */
	if ( bspLightBytes.empty() ) {
		Error( "No lightmap data" );
	}

	/* make a directory for the lightmaps */
	Q_mkdir( dirname );

	/* iterate through the lightmaps */
	for ( i = 0, lightmap = bspLightBytes.data(); lightmap < ( bspLightBytes.data() + bspLightBytes.size() ); i++, lightmap += ( g_game->lightmapSize * g_game->lightmapSize * 3 ) )
	{
		/* read a tga image */
		sprintf( filename, "%s/lightmap_%04d.tga", dirname, i );
		Sys_Printf( "Loading %s\n", filename );
		MemBuffer buffer = vfsLoadFile( filename, -1 );
		if ( !buffer ) {
			Sys_Warning( "Unable to load image %s\n", filename );
			continue;
		}

		/* parse file into an image */
		pixels = NULL;
		LoadTGABuffer( buffer.data(), buffer.size(), &pixels, &width, &height );

		/* sanity check it */
		if ( pixels == NULL ) {
			Sys_Warning( "Unable to load image %s\n", filename );
			continue;
		}
		if ( width != g_game->lightmapSize || height != g_game->lightmapSize ) {
			Sys_Warning( "Image %s is not the right size (%d, %d) != (%d, %d)\n",
			             filename, width, height, g_game->lightmapSize, g_game->lightmapSize );
		}

		/* copy the pixels */
		in = pixels;
		for ( y = 1; y <= g_game->lightmapSize; y++ )
		{
			out = lightmap + ( ( g_game->lightmapSize - y ) * g_game->lightmapSize * 3 );
			for ( x = 0; x < g_game->lightmapSize; x++, in += 4, out += 3 )
				VectorCopy( in, out );
		}

		/* free the image */
		free( pixels );
	}

	/* write the bsp */
	Sys_Printf( "writing %s\n", source );
	WriteBSPFile( source );

	/* return to sender */
	return 0;
}



/* -------------------------------------------------------------------------------

   this section deals with projecting a lightmap onto a raw drawsurface

   ------------------------------------------------------------------------------- */

namespace
{
int numSurfsVertexLit;
int numSurfsVertexForced;
int numSurfsVertexApproximated;
int numSurfsLightmapped;
int numPlanarsLightmapped;
int numNonPlanarsLightmapped;
int numPatchesLightmapped;
int numPlanarPatchesLightmapped;
}

/*
   FinishRawLightmap()
   allocates a raw lightmap's necessary buffers
 */

static void FinishRawLightmap( rawLightmap_t *lm ){
	int i, j, c, size, *sc;
	float is;
	surfaceInfo_t       *info;


	/* sort light surfaces by shader name */
	std::sort( &lightSurfaces[ lm->firstLightSurface ], &lightSurfaces[ lm->firstLightSurface ] + lm->numLightSurfaces, []( const int a, const int b ){
		/* get shaders */
		const shaderInfo_t *asi = surfaceInfos[ a ].si;
		const shaderInfo_t *bsi = surfaceInfos[ b ].si;

		/* dummy check */
		if ( asi == NULL ) {
			return true;
		}
		if ( bsi == NULL ) {
			return false;
		}

		/* compare shader names */
		return strcmp( asi->shader, bsi->shader ) < 0;
	} );

	/* count clusters */
	lm->numLightClusters = 0;
	for ( i = 0; i < lm->numLightSurfaces; i++ )
	{
		/* get surface info */
		info = &surfaceInfos[ lightSurfaces[ lm->firstLightSurface + i ] ];

		/* add surface clusters */
		lm->numLightClusters += info->numSurfaceClusters;
	}

	/* allocate buffer for clusters and copy */
	lm->lightClusters = safe_malloc( lm->numLightClusters * sizeof( *lm->lightClusters ) );
	c = 0;
	for ( i = 0; i < lm->numLightSurfaces; i++ )
	{
		/* get surface info */
		info = &surfaceInfos[ lightSurfaces[ lm->firstLightSurface + i ] ];

		/* add surface clusters */
		for ( j = 0; j < info->numSurfaceClusters; j++ )
			lm->lightClusters[ c++ ] = surfaceClusters[ info->firstSurfaceCluster + j ];
	}

	/* set styles */
	lm->styles[ 0 ] = LS_NORMAL;
	for ( i = 1; i < MAX_LIGHTMAPS; i++ )
		lm->styles[ i ] = LS_NONE;

	/* set supersampling size */
	lm->sw = lm->w * superSample;
	lm->sh = lm->h * superSample;

	/* manipulate origin/vecs for supersampling */
	if ( superSample > 1 && lm->vecs != NULL ) {
		/* calc inverse supersample */
		is = 1.0f / superSample;

		/* scale the vectors and shift the origin */
		#if 1
		/* new code that works for arbitrary supersampling values */
		lm->origin -= vector3_mid( lm->vecs[ 0 ], lm->vecs[ 1 ] );
		lm->vecs[ 0 ] *= is;
		lm->vecs[ 1 ] *= is;
		lm->origin += ( lm->vecs[ 0 ] + lm->vecs[ 1 ] ) * is;
		#else
		/* old code that only worked with a value of 2 */
		lm->vecs[ 0 ] *= is;
		lm->vecs[ 1 ] *= is;
		lm->origin -=  (lm->vecs[ 0 ] + lm->vecs[ 1 ] ) * is;
		#endif
	}

	/* allocate bsp lightmap storage */
	size = lm->w * lm->h * sizeof( *( lm->bspLuxels[ 0 ] ) );
	if ( lm->bspLuxels[ 0 ] == NULL ) {
		lm->bspLuxels[ 0 ] = safe_malloc( size );
	}
	memset( lm->bspLuxels[ 0 ], 0, size );

	/* allocate radiosity lightmap storage */
	if ( bounce ) {
		size = lm->w * lm->h * sizeof( *lm->radLuxels[ 0 ] );
		if ( lm->radLuxels[ 0 ] == NULL ) {
			lm->radLuxels[ 0 ] = safe_malloc( size );
		}
		memset( lm->radLuxels[ 0 ], 0, size );
	}

	/* allocate sampling lightmap storage */
	size = lm->sw * lm->sh * sizeof( *lm->superLuxels[ 0 ] );
	if ( lm->superLuxels[ 0 ] == NULL ) {
		lm->superLuxels[ 0 ] = safe_malloc( size );
	}
	memset( lm->superLuxels[ 0 ], 0, size );

	/* allocate origin map storage */
	size = lm->sw * lm->sh * sizeof( *lm->superOrigins );
	if ( lm->superOrigins == NULL ) {
		lm->superOrigins = safe_malloc( size );
	}
	memset( lm->superOrigins, 0, size );

	/* allocate normal map storage */
	size = lm->sw * lm->sh * sizeof( *lm->superNormals );
	if ( lm->superNormals == NULL ) {
		lm->superNormals = safe_malloc( size );
	}
	memset( lm->superNormals, 0, size );

	/* allocate dirt map storage */
	size = lm->sw * lm->sh * sizeof( *lm->superDirt );
	if ( lm->superDirt == NULL ) {
		lm->superDirt = safe_malloc( size );
	}
	memset( lm->superDirt, 0, size );

	/* allocate floodlight map storage */
	size = lm->sw * lm->sh * sizeof( *lm->superFloodLight );
	if ( lm->superFloodLight == NULL ) {
		lm->superFloodLight = safe_malloc( size );
	}
	memset( lm->superFloodLight, 0, size );

	/* allocate cluster map storage */
	size = lm->sw * lm->sh * sizeof( *lm->superClusters );
	if ( lm->superClusters == NULL ) {
		lm->superClusters = safe_malloc( size );
	}
	size = lm->sw * lm->sh;
	sc = lm->superClusters;
	for ( i = 0; i < size; i++ )
		( *sc++ ) = CLUSTER_UNMAPPED;

	/* deluxemap allocation */
	if ( deluxemap ) {
		/* allocate sampling deluxel storage */
		size = lm->sw * lm->sh * sizeof( *lm->superDeluxels );
		if ( lm->superDeluxels == NULL ) {
			lm->superDeluxels = safe_malloc( size );
		}
		memset( lm->superDeluxels, 0, size );

		/* allocate bsp deluxel storage */
		size = lm->w * lm->h * sizeof( *lm->bspDeluxels );
		if ( lm->bspDeluxels == NULL ) {
			lm->bspDeluxels = safe_malloc( size );
		}
		memset( lm->bspDeluxels, 0, size );
	}

	/* add to count */
	numLuxels += ( lm->sw * lm->sh );
}



/*
   AddPatchToRawLightmap()
   projects a lightmap for a patch surface
   since lightmap calculation for surfaces is now handled in a general way (light_ydnar.c),
   it is no longer necessary for patch verts to fall exactly on a lightmap sample
   based on AllocateLightmapForPatch()
 */

static bool AddPatchToRawLightmap( int num, rawLightmap_t *lm ){
	/* return */
	return true;
}



/*
   AddSurfaceToRawLightmap()
   projects a lightmap for a surface
   based on AllocateLightmapForSurface()
 */

static bool AddSurfaceToRawLightmap( int num, rawLightmap_t *lm ){
	return true;
}



/*
   CompareSurfaceInfo()
   compare functor for std::sort()
 */

struct CompareSurfaceInfo
{
	bool operator()( const int a, const int b ) const {
		/* get surface info */
		const surfaceInfo_t& aInfo = surfaceInfos[ a ];
		const surfaceInfo_t& bInfo = surfaceInfos[ b ];

		/* model first */
		if ( aInfo.modelindex < bInfo.modelindex ) {
			return false;
		}
		else if ( aInfo.modelindex > bInfo.modelindex ) {
			return true;
		}

		/* then lightmap status */
		if ( aInfo.hasLightmap < bInfo.hasLightmap ) {
			return false;
		}
		else if ( aInfo.hasLightmap > bInfo.hasLightmap ) {
			return true;
		}

		/* 27: then shader! */
		if ( aInfo.si < bInfo.si ) {
			return false;
		}
		else if ( aInfo.si > bInfo.si ) {
			return true;
		}


		/* then lightmap sample size */
		if ( aInfo.sampleSize < bInfo.sampleSize ) {
			return false;
		}
		else if ( aInfo.sampleSize > bInfo.sampleSize ) {
			return true;
		}

		/* then lightmap axis */
		for ( int i = 0; i < 3; i++ )
		{
			if ( aInfo.axis[ i ] < bInfo.axis[ i ] ) {
				return false;
			}
			else if ( aInfo.axis[ i ] > bInfo.axis[ i ] ) {
				return true;
			}
		}

		/* then plane */
		if ( aInfo.plane == NULL && bInfo.plane != NULL ) {
			return false;
		}
		else if ( aInfo.plane != NULL && bInfo.plane == NULL ) {
			return true;
		}
		else if ( aInfo.plane != NULL && bInfo.plane != NULL ) {
			for ( int i = 0; i < 3; i++ )
			{
				if ( aInfo.plane->normal()[ i ] < bInfo.plane->normal()[ i ] ) {
					return false;
				}
				else if ( aInfo.plane->normal()[ i ] > bInfo.plane->normal()[ i ] ) {
					return true;
				}
			}
			if ( aInfo.plane->dist() < bInfo.plane->dist() ) {
				return false;
			}
			else if ( aInfo.plane->dist() > bInfo.plane->dist() ) {
				return true;
			}

		}

		/* then position in world */
		for ( int i = 0; i < 3; i++ )
		{
			if ( aInfo.minmax.mins[ i ] < bInfo.minmax.mins[ i ] ) {
				return false;
			}
			else if ( aInfo.minmax.mins[ i ] > bInfo.minmax.mins[ i ] ) {
				return true;
			}
		}

		/* these are functionally identical (this should almost never happen) */
		return false;
	}
};



/*
   SetupSurfaceLightmaps()
   allocates lightmaps for every surface in the bsp that needs one
   this depends on yDrawVerts being allocated
 */

void SetupSurfaceLightmaps(){
	
}



/*
   StitchSurfaceLightmaps()
   stitches lightmap edges
   2002-11-20 update: use this func only for stitching nonplanar patch lightmap seams
 */

#define MAX_STITCH_CANDIDATES   32
#define MAX_STITCH_LUXELS       64

void StitchSurfaceLightmaps(){
	int i, j, x, y, x2, y2,
	    numStitched, numCandidates, numLuxels, fOld;
	rawLightmap_t   *lm, *a, *b, *c[ MAX_STITCH_CANDIDATES ];
	float           sampleSize, totalColor;


	/* disabled for now */
	return;

	/* note it */
	Sys_Printf( "--- StitchSurfaceLightmaps ---\n" );

	/* init pacifier */
	fOld = -1;
	Timer timer;

	/* walk the list of raw lightmaps */
	numStitched = 0;
	for ( i = 0; i < numRawLightmaps; i++ )
	{
		/* print pacifier */
		if ( const int f = 10 * i / numRawLightmaps; f != fOld ) {
			fOld = f;
			Sys_Printf( "%i...", f );
		}

		/* get lightmap a */
		a = &rawLightmaps[ i ];

		/* walk rest of lightmaps */
		numCandidates = 0;
		for ( j = i + 1; j < numRawLightmaps && numCandidates < MAX_STITCH_CANDIDATES; j++ )
		{
			/* get lightmap b */
			b = &rawLightmaps[ j ];

			/* test bounding box */
			if ( !a->minmax.test( b->minmax ) ) {
				continue;
			}

			/* add candidate */
			c[ numCandidates++ ] = b;
		}

		/* walk luxels */
		for ( y = 0; y < a->sh; y++ )
		{
			for ( x = 0; x < a->sw; x++ )
			{
				/* ignore unmapped/unlit luxels */
				lm = a;
				if ( lm->getSuperCluster( x, y ) == CLUSTER_UNMAPPED ) {
					continue;
				}
				SuperLuxel& luxel = lm->getSuperLuxel( 0, x, y );
				if ( luxel.count <= 0.0f ) {
					continue;
				}

				/* get particulars */
				const Vector3& origin = lm->getSuperOrigin( x, y );
				const Vector3& normal = lm->getSuperNormal( x, y );

				/* walk candidate list */
				for ( j = 0; j < numCandidates; j++ )
				{
					/* get candidate */
					b = c[ j ];
					lm = b;

					/* set samplesize to the smaller of the pair */
					sampleSize = 0.5f * std::min( a->actualSampleSize, b->actualSampleSize );

					/* test bounding box */
					if ( !b->minmax.test( origin, sampleSize ) ) {
						continue;
					}

					/* walk candidate luxels */
					Vector3 average( 0 );
					numLuxels = 0;
					totalColor = 0.0f;
					for ( y2 = 0; y2 < b->sh && numLuxels < MAX_STITCH_LUXELS; y2++ )
					{
						for ( x2 = 0; x2 < b->sw && numLuxels < MAX_STITCH_LUXELS; x2++ )
						{
							/* ignore same luxels */
							if ( a == b && abs( x - x2 ) <= 1 && abs( y - y2 ) <= 1 ) {
								continue;
							}

							/* ignore unmapped/unlit luxels */
							if ( lm->getSuperCluster( x2, y2 ) == CLUSTER_UNMAPPED ) {
								continue;
							}
							const SuperLuxel& luxel2 = lm->getSuperLuxel( 0, x2, y2 );
							if ( luxel2.count <= 0.0f ) {
								continue;
							}

							/* get particulars */
							const Vector3& origin2 = lm->getSuperOrigin( x2, y2 );
							const Vector3& normal2 = lm->getSuperNormal( x2, y2 );

							/* test normal */
							if ( vector3_dot( normal, normal2 ) < 0.5f ) {
								continue;
							}

							/* test bounds */
							if ( !vector3_equal_epsilon( origin, origin2, sampleSize ) ) {
								continue;
							}

							/* add luxel */
							//%	luxel2.value = { 255, 0, 255 };
							numLuxels++;
							average += luxel2.value;
							totalColor += luxel2.count;
						}
					}

					/* early out */
					if ( numLuxels == 0 ) {
						continue;
					}

					/* scale average */
					luxel.value = average * ( 1.0f / totalColor );
					luxel.count = 1.0f;
					numStitched++;
				}
			}
		}
	}

	/* emit statistics */
	Sys_Printf( " (%i)\n", int( timer.elapsed_sec() ) );
	Sys_FPrintf( SYS_VRB, "%9d luxels stitched\n", numStitched );
}



/*
   CompareBSPLuxels()
   compares two surface lightmaps' bsp luxels, ignoring occluded luxels
 */

#define SOLID_EPSILON       0.0625f
#define LUXEL_TOLERANCE     0.0025
#define LUXEL_COLOR_FRAC    0.001302083 /* 1 / 3 / 256 */

static bool CompareBSPLuxels( rawLightmap_t *a, int aNum, rawLightmap_t *b, int bNum ){
	/* styled lightmaps will never be collapsed to non-styled lightmaps when there is _minlight */
	if ( minLight != g_vector3_identity &&
	     ( aNum == 0 ) != ( bNum == 0 ) ) {
		return false;
	}

	/* basic tests */
	if ( a->customWidth != b->customWidth || a->customHeight != b->customHeight ||
	     a->brightness != b->brightness ||
	     a->solid[ aNum ] != b->solid[ bNum ] ||
	     a->bspLuxels[ aNum ] == NULL || b->bspLuxels[ bNum ] == NULL ) {
		return false;
	}

	/* compare solid color lightmaps */
	if ( a->solid[ aNum ] && b->solid[ bNum ] ) {
		/* compare color */
		if ( !vector3_equal_epsilon( a->solidColor[ aNum ], b->solidColor[ bNum ], SOLID_EPSILON ) ) {
			return false;
		}

		/* okay */
		return true;
	}

	/* compare nonsolid lightmaps */
	if ( a->w != b->w || a->h != b->h ) {
		return false;
	}

	/* compare luxels */
	double delta = 0.0;
	double total = 0.0;
	for ( int y = 0; y < a->h; y++ )
	{
		for ( int x = 0; x < a->w; x++ )
		{
			/* increment total */
			total += 1.0;

			/* get luxels */
			const Vector3& aLuxel = a->getBspLuxel( aNum, x, y );
			const Vector3& bLuxel = b->getBspLuxel( bNum, x, y );

			/* ignore unused luxels */
			if ( aLuxel[ 0 ] < 0 || bLuxel[ 0 ] < 0 ) {
				continue;
			}

			/* 2003-09-27: compare individual luxels */
			if ( !vector3_equal_epsilon( aLuxel, bLuxel, 3.f ) ) {
				return false;
			}

			/* compare (fixme: take into account perceptual differences) */
			Vector3 diff = aLuxel - bLuxel;
			for( int i = 0; i < 3; ++i )
				diff[i] = fabs( diff[i] );
			delta += vector3_dot( diff, Vector3( LUXEL_COLOR_FRAC ) );

			/* is the change too high? */
			if ( total > 0.0 && ( ( delta / total ) > LUXEL_TOLERANCE ) ) {
				return false;
			}
		}
	}

	/* made it this far, they must be identical (or close enough) */
	return true;
}



/*
   MergeBSPLuxels()
   merges two surface lightmaps' bsp luxels, overwriting occluded luxels
 */

static bool MergeBSPLuxels( rawLightmap_t *a, int aNum, rawLightmap_t *b, int bNum ){
	int x, y;
	Vector3 luxel;


	/* basic tests */
	if ( a->customWidth != b->customWidth || a->customHeight != b->customHeight ||
	     a->brightness != b->brightness ||
	     a->solid[ aNum ] != b->solid[ bNum ] ||
	     a->bspLuxels[ aNum ] == NULL || b->bspLuxels[ bNum ] == NULL ) {
		return false;
	}

	/* compare solid lightmaps */
	if ( a->solid[ aNum ] && b->solid[ bNum ] ) {
		/* average */
		luxel = vector3_mid( a->solidColor[ aNum ], b->solidColor[ bNum ] );

		/* copy to both */
		a->solidColor[ aNum ] = luxel;
		b->solidColor[ bNum ] = luxel;

		/* return to sender */
		return true;
	}

	/* compare nonsolid lightmaps */
	if ( a->w != b->w || a->h != b->h ) {
		return false;
	}

	/* merge luxels */
	for ( y = 0; y < a->h; y++ )
	{
		for ( x = 0; x < a->w; x++ )
		{
			/* get luxels */
			Vector3& aLuxel = a->getBspLuxel( aNum, x, y );
			Vector3& bLuxel = b->getBspLuxel( bNum, x, y );

			/* handle occlusion mismatch */
			if ( aLuxel[ 0 ] < 0.0f ) {
				aLuxel = bLuxel;
			}
			else if ( bLuxel[ 0 ] < 0.0f ) {
				bLuxel = aLuxel;
			}
			else
			{
				/* average */
				luxel = vector3_mid( aLuxel, bLuxel );

				/* debugging code */
				//%	luxel[ 2 ] += 64.0f;

				/* copy to both */
				aLuxel = luxel;
				bLuxel = luxel;
			}
		}
	}

	/* done */
	return true;
}



/*
   ApproximateLuxel()
   determines if a single luxel is can be approximated with the interpolated vertex rgba
 */

static bool ApproximateLuxel( rawLightmap_t *lm, bspDrawVert_t *dv ){
	/* find luxel xy coords */
	const int x = std::clamp( int( dv->lightmap[ 0 ][ 0 ] / superSample ), 0, lm->w - 1 );
	const int y = std::clamp( int( dv->lightmap[ 0 ][ 1 ] / superSample ), 0, lm->h - 1 );

	/* walk list */
	for ( int lightmapNum = 0; lightmapNum < MAX_LIGHTMAPS; lightmapNum++ )
	{
		/* early out */
		if ( lm->styles[ lightmapNum ] == LS_NONE ) {
			continue;
		}

		/* get luxel */
		const Vector3& luxel = lm->getBspLuxel( lightmapNum, x, y );

		/* ignore occluded luxels */
		if ( luxel[ 0 ] < 0.0f || luxel[ 1 ] < 0.0f || luxel[ 2 ] < 0.0f ) {
			return true;
		}

		/* copy, set min color and compare */
		Vector3 color = luxel;
		Vector3 vertexColor = dv->color[ 0 ].rgb();

		/* styles are not affected by minlight */
		if ( lightmapNum == 0 ) {
			for ( int i = 0; i < 3; i++ )
			{
				/* set min color */
				value_maximize( color[ i ], minLight[ i ] );
				value_maximize( vertexColor[ i ], minLight[ i ] ); /* note NOT minVertexLight */
			}
		}

		/* set to bytes */
		Color4b cb( ColorToBytes( color, 1.0f ), 0 );
		Color4b vcb( ColorToBytes( vertexColor, 1.0f ), 0 );

		/* compare */
		for ( int i = 0; i < 3; i++ )
		{
			if ( abs( cb[ i ] - vcb[ i ] ) > approximateTolerance ) {
				return false;
			}
		}
	}

	/* close enough for the girls i date */
	return true;
}



/*
   ApproximateTriangle()
   determines if a single triangle can be approximated with vertex rgba
 */

static bool ApproximateTriangle_r( rawLightmap_t *lm, bspDrawVert_t *dv[ 3 ] ){
	bspDrawVert_t mid, *dv2[ 3 ];
	int max;


	/* approximate the vertexes */
	if ( !ApproximateLuxel( lm, dv[ 0 ] ) ) {
		return false;
	}
	if ( !ApproximateLuxel( lm, dv[ 1 ] ) ) {
		return false;
	}
	if ( !ApproximateLuxel( lm, dv[ 2 ] ) ) {
		return false;
	}

	/* subdivide calc */
	{
		int i;

		/* find the longest edge and split it */
		max = -1;
		float maxDist = 0;
		for ( i = 0; i < 3; i++ )
		{
			const float dist = vector2_length( dv[ i ]->lightmap[ 0 ] - dv[ ( i + 1 ) % 3 ]->lightmap[ 0 ] );
			if ( dist > maxDist ) {
				maxDist = dist;
				max = i;
			}
		}

		/* try to early out */
		if ( i < 0 || maxDist < subdivideThreshold ) {
			return true;
		}
	}

	/* split the longest edge and map it */
	LerpDrawVert( dv[ max ], dv[ ( max + 1 ) % 3 ], &mid );
	if ( !ApproximateLuxel( lm, &mid ) ) {
		return false;
	}

	/* recurse to first triangle */
	VectorCopy( dv, dv2 );
	dv2[ max ] = &mid;
	if ( !ApproximateTriangle_r( lm, dv2 ) ) {
		return false;
	}

	/* recurse to second triangle */
	VectorCopy( dv, dv2 );
	dv2[ ( max + 1 ) % 3 ] = &mid;
	return ApproximateTriangle_r( lm, dv2 );
}



/*
   ApproximateLightmap()
   determines if a raw lightmap can be approximated sufficiently with vertex colors
 */

static bool ApproximateLightmap( rawLightmap_t *lm ){
	
	return false;
}



/*
   TestOutLightmapStamp()
   tests a stamp on a given lightmap for validity
 */

static bool TestOutLightmapStamp( rawLightmap_t *lm, int lightmapNum, outLightmap_t *olm, int x, int y ){
	/* bounds check */
	if ( x < 0 || y < 0 || ( x + lm->w ) > olm->customWidth || ( y + lm->h ) > olm->customHeight ) {
		return false;
	}

	/* solid lightmaps test a 1x1 stamp */
	if ( lm->solid[ lightmapNum ] ) {
		if ( bit_is_enabled( olm->lightBits, ( y * olm->customWidth ) + x ) ) {
			return false;
		}
		return true;
	}

	/* test the stamp */
	for ( int sy = 0; sy < lm->h; ++sy )
	{
		for ( int sx = 0; sx < lm->w; ++sx )
		{
			/* get luxel */
			if ( lm->getBspLuxel( lightmapNum, sx, sy )[ 0 ] < 0.0f ) {
				continue;
			}

			/* get bsp lightmap coords and test */
			const int ox = x + sx;
			const int oy = y + sy;
			if ( bit_is_enabled( olm->lightBits, ( oy * olm->customWidth ) + ox ) ) {
				return false;
			}
		}
	}

	/* stamp is empty */
	return true;
}



/*
   SetupOutLightmap()
   sets up an output lightmap
 */

static void SetupOutLightmap( rawLightmap_t *lm, outLightmap_t *olm ){
	/* dummy check */
	if ( lm == NULL || olm == NULL ) {
		return;
	}

	/* is this a "normal" bsp-stored lightmap? */
	if ( ( lm->customWidth == g_game->lightmapSize && lm->customHeight == g_game->lightmapSize ) || externalLightmaps ) {
		olm->lightmapNum = numBSPLightmaps;
		numBSPLightmaps++;

		/* lightmaps are interleaved with light direction maps */
		if ( deluxemap ) {
			numBSPLightmaps++;
		}
	}
	else{
		olm->lightmapNum = -3;
	}

	/* set external lightmap number */
	olm->extLightmapNum = -1;

	/* set it up */
	olm->numLightmaps = 0;
	olm->customWidth = lm->customWidth;
	olm->customHeight = lm->customHeight;
	olm->freeLuxels = olm->customWidth * olm->customHeight;
	olm->numShaders = 0;

	/* allocate buffers */
	olm->lightBits = safe_calloc( ( olm->customWidth * olm->customHeight / 8 ) + 8 );
	olm->bspLightBytes = safe_calloc( olm->customWidth * olm->customHeight * sizeof( *olm->bspLightBytes ) );
	if ( deluxemap ) {
		olm->bspDirBytes = safe_calloc( olm->customWidth * olm->customHeight * sizeof( *olm->bspDirBytes ) );
	}
}



/*
   FindOutLightmaps()
   for a given surface lightmap, find output lightmap pages and positions for it
 */

#define LIGHTMAP_RESERVE_COUNT 1
static void FindOutLightmaps( rawLightmap_t *lm, bool fastAllocate ){
	int i, j, k, lightmapNum, xMax, yMax, x = -1, y = -1, sx, sy, ox, oy;
	outLightmap_t       *olm;
	surfaceInfo_t       *info;
	bool ok;
	int xIncrement, yIncrement;


	/* set default lightmap number (-3 = LIGHTMAP_BY_VERTEX) */
	for ( lightmapNum = 0; lightmapNum < MAX_LIGHTMAPS; lightmapNum++ )
		lm->outLightmapNums[ lightmapNum ] = -3;

	/* can this lightmap be approximated with vertex color? */
	if ( ApproximateLightmap( lm ) ) {
		return;
	}

	/* walk list */
	for ( lightmapNum = 0; lightmapNum < MAX_LIGHTMAPS; lightmapNum++ )
	{
		/* early out */
		if ( lm->styles[ lightmapNum ] == LS_NONE ) {
			continue;
		}

		/* don't store twinned lightmaps */
		if ( lm->twins[ lightmapNum ] != NULL ) {
			continue;
		}

		/* if this is a styled lightmap, try some normalized locations first */
		ok = false;
		if ( lightmapNum > 0 && outLightmaps != NULL ) {
			/* loop twice */
			for ( j = 0; j < 2; j++ )
			{
				/* try identical position */
				for ( i = 0; i < numOutLightmaps; i++ )
				{
					/* get the output lightmap */
					olm = &outLightmaps[ i ];

					/* simple early out test */
					if ( olm->freeLuxels < lm->used ) {
						continue;
					}

					/* don't store non-custom raw lightmaps on custom bsp lightmaps */
					if ( olm->customWidth != lm->customWidth ||
					     olm->customHeight != lm->customHeight ) {
						continue;
					}

					/* try identical */
					if ( j == 0 ) {
						x = lm->lightmapX[ 0 ];
						y = lm->lightmapY[ 0 ];
						ok = TestOutLightmapStamp( lm, lightmapNum, olm, x, y );
					}

					/* try shifting */
					else
					{
						for ( sy = -1; sy <= 1; sy++ )
						{
							for ( sx = -1; sx <= 1; sx++ )
							{
								x = lm->lightmapX[ 0 ] + sx * ( olm->customWidth >> 1 );  //%	lm->w;
								y = lm->lightmapY[ 0 ] + sy * ( olm->customHeight >> 1 ); //%	lm->h;
								ok = TestOutLightmapStamp( lm, lightmapNum, olm, x, y );

								if ( ok ) {
									break;
								}
							}

							if ( ok ) {
								break;
							}
						}
					}

					if ( ok ) {
						break;
					}
				}

				if ( ok ) {
					break;
				}
			}
		}

		/* try normal placement algorithm */
		if ( !ok ) {
			/* reset origin */
			x = 0;
			y = 0;

			/* walk the list of lightmap pages */
			if ( lightmapSearchBlockSize <= 0 || numOutLightmaps < LIGHTMAP_RESERVE_COUNT ) {
				i = 0;
			}
			else{
				i = ( ( numOutLightmaps - LIGHTMAP_RESERVE_COUNT ) / lightmapSearchBlockSize ) * lightmapSearchBlockSize;
			}
			for ( ; i < numOutLightmaps; ++i )
			{
				/* get the output lightmap */
				olm = &outLightmaps[ i ];

				/* simple early out test */
				if ( olm->freeLuxels < lm->used ) {
					continue;
				}

				/* if fast allocation, skip lightmap files that are more than 90% complete */
				if ( fastAllocate ) {
					if (olm->freeLuxels < (olm->customWidth * olm->customHeight) / 10) {
						continue;
					}
				}

				/* don't store non-custom raw lightmaps on custom bsp lightmaps */
				if ( olm->customWidth != lm->customWidth ||
				     olm->customHeight != lm->customHeight ) {
					continue;
				}

				/* set maxs */
				if ( lm->solid[ lightmapNum ] ) {
					xMax = olm->customWidth;
					yMax = olm->customHeight;
				}
				else
				{
					xMax = ( olm->customWidth - lm->w ) + 1;
					yMax = ( olm->customHeight - lm->h ) + 1;
				}

				/* if fast allocation, do not test allocation on every pixels, especially for large lightmaps */
				if ( fastAllocate ) {
					xIncrement = std::max( 1, lm->w / 15 );
					yIncrement = std::max( 1, lm->h / 15 );
				}
				else {
					xIncrement = 1;
					yIncrement = 1;
				}

				/* walk the origin around the lightmap */
				for ( y = 0; y < yMax; y += yIncrement )
				{
					for ( x = 0; x < xMax; x += xIncrement )
					{
						/* find a fine tract of lauhnd */
						ok = TestOutLightmapStamp( lm, lightmapNum, olm, x, y );

						if ( ok ) {
							break;
						}
					}

					if ( ok ) {
						break;
					}
				}

				if ( ok ) {
					break;
				}

				/* reset x and y */
				x = 0;
				y = 0;
			}
		}

		/* no match? */
		if ( !ok ) {
			/* allocate LIGHTMAP_RESERVE_COUNT new output lightmaps */
			numOutLightmaps += LIGHTMAP_RESERVE_COUNT;
			olm = safe_malloc( numOutLightmaps * sizeof( outLightmap_t ) );

			if ( outLightmaps != NULL && numOutLightmaps > LIGHTMAP_RESERVE_COUNT ) {
				memcpy( olm, outLightmaps, ( numOutLightmaps - LIGHTMAP_RESERVE_COUNT ) * sizeof( outLightmap_t ) );
				free( outLightmaps );
			}
			outLightmaps = olm;

			/* initialize both out lightmaps */
			for ( k = numOutLightmaps - LIGHTMAP_RESERVE_COUNT; k < numOutLightmaps; ++k )
				SetupOutLightmap( lm, &outLightmaps[ k ] );

			/* set out lightmap */
			i = numOutLightmaps - LIGHTMAP_RESERVE_COUNT;
			olm = &outLightmaps[ i ];

			/* set stamp xy origin to the first surface lightmap */
			if ( lightmapNum > 0 ) {
				x = lm->lightmapX[ 0 ];
				y = lm->lightmapY[ 0 ];
			}
		}

		/* if this is a style-using lightmap, it must be exported */
		if ( lightmapNum > 0 && g_game->load != LoadRBSPFile ) {
			olm->extLightmapNum = 0;
		}

		/* add the surface lightmap to the bsp lightmap */
		lm->outLightmapNums[ lightmapNum ] = i;
		lm->lightmapX[ lightmapNum ] = x;
		lm->lightmapY[ lightmapNum ] = y;
		olm->numLightmaps++;

		/* add shaders */
		for ( i = 0; i < lm->numLightSurfaces; i++ )
		{
			/* get surface info */
			info = &surfaceInfos[ lightSurfaces[ lm->firstLightSurface + i ] ];

			/* test for shader */
			for ( j = 0; j < olm->numShaders; j++ )
			{
				if ( olm->shaders[ j ] == info->si ) {
					break;
				}
			}

			/* if it doesn't exist, add it */
			if ( j >= olm->numShaders && olm->numShaders < MAX_LIGHTMAP_SHADERS ) {
				olm->shaders[ olm->numShaders ] = info->si;
				olm->numShaders++;
				numLightmapShaders++;
			}
		}

		/* set maxs */
		if ( lm->solid[ lightmapNum ] ) {
			xMax = 1;
			yMax = 1;
		}
		else
		{
			xMax = lm->w;
			yMax = lm->h;
		}

		/* mark the bits used */
		for ( y = 0; y < yMax; y++ )
		{
			for ( x = 0; x < xMax; x++ )
			{
				/* get luxel */
				const Vector3& luxel = lm->getBspLuxel( lightmapNum, x, y );
				if ( luxel[ 0 ] < 0.0f && !lm->solid[ lightmapNum ] ) {
					continue;
				}
				Vector3 color;
				/* set minimum light */
				if ( lm->solid[ lightmapNum ] ) {
					if ( debug ) {
						color = { 255.0f, 0.0f, 0.0f };
					}
					else{
						color = lm->solidColor[ lightmapNum ];
					}
				}
				else{

					color = luxel;
				}

				/* styles are not affected by minlight */
				if ( lightmapNum == 0 ) {
					for ( i = 0; i < 3; i++ )
					{
						value_maximize( color[ i ], minLight[ i ] );
					}
				}

				/* get bsp lightmap coords  */
				ox = x + lm->lightmapX[ lightmapNum ];
				oy = y + lm->lightmapY[ lightmapNum ];

				/* flag pixel as used */
				bit_enable( olm->lightBits, ( oy * olm->customWidth ) + ox );
				olm->freeLuxels--;

				/* store color */
				olm->bspLightBytes[ oy * olm->customWidth + ox] = ColorToBytes( color, lm->brightness );

				/* store direction */
				if ( deluxemap ) {
					/* normalize average light direction */
					const Vector3 direction = VectorNormalized( lm->getBspDeluxel( x, y ) * 1000.0f ) * 127.5f;
					olm->bspDirBytes[ oy * olm->customWidth + ox ] = direction + Vector3( 127.5f );
				}
			}
		}
	}
}



/*
   CompareRawLightmap
   compare functor for std::sort()
 */

struct CompareRawLightmap
{
	bool operator()( const int a, const int b ) const {
		int diff;

		/* get lightmaps */
		const rawLightmap_t& alm = rawLightmaps[ a ];
		const rawLightmap_t& blm = rawLightmaps[ b ];

		/* get min number of surfaces */
		const int min = std::min( alm.numLightSurfaces, blm.numLightSurfaces );

		/* iterate */
		for ( int i = 0; i < min; i++ )
		{
			/* get surface info */
			const surfaceInfo_t& aInfo = surfaceInfos[ lightSurfaces[ alm.firstLightSurface + i ] ];
			const surfaceInfo_t& bInfo = surfaceInfos[ lightSurfaces[ blm.firstLightSurface + i ] ];

			/* compare shader names */
			diff = strcmp( aInfo.si->shader, bInfo.si->shader );
			if ( diff != 0 ) {
				return diff < 0;
			}
		}

		/* test style count */
		diff = 0;
		for ( int i = 0; i < MAX_LIGHTMAPS; i++ )
			diff += blm.styles[ i ] - alm.styles[ i ];
		if ( diff != 0 ) {
			return diff < 0;
		}

		/* compare size */
		diff = ( blm.w * blm.h ) - ( alm.w * alm.h );
		if ( diff != 0 ) {
			return diff < 0;
		}
		/* must be equivalent */
		return false;
	}
};

static void FillOutLightmap( outLightmap_t *olm ){
	int x, y;
	int ofs;
	int cnt, filled;
	byte *lightBitsNew = NULL;
	Vector3b *lightBytesNew = NULL;
	Vector3b *dirBytesNew = NULL;
	const size_t size = olm->customWidth * olm->customHeight * sizeof( Vector3b );

	lightBitsNew = safe_malloc( ( olm->customWidth * olm->customHeight + 8 ) / 8 );
	lightBytesNew = safe_malloc( size );
	if ( deluxemap ) {
		dirBytesNew = safe_malloc( size );
	}

	/*
	   memset(olm->lightBits, 0, (olm->customWidth * olm->customHeight + 8) / 8);
	    olm->lightBits[0] |= 1;
	    olm->lightBits[(10 * olm->customWidth + 30) >> 3] |= 1 << ((10 * olm->customWidth + 30) & 7);
	   memset(olm->bspLightBytes, 0, olm->customWidth * olm->customHeight * 3);
	    olm->bspLightBytes[0] = 255;
	    olm->bspLightBytes[(10 * olm->customWidth + 30) * 3 + 2] = 255;
	 */

	memcpy( lightBitsNew, olm->lightBits, ( olm->customWidth * olm->customHeight + 8 ) / 8 );
	memcpy( lightBytesNew, olm->bspLightBytes, size );
	if ( deluxemap ) {
		memcpy( dirBytesNew, olm->bspDirBytes, size );
	}

	for (;; )
	{
		filled = 0;
		for ( y = 0; y < olm->customHeight; ++y )
		{
			for ( x = 0; x < olm->customWidth; ++x )
			{
				ofs = y * olm->customWidth + x;
				if ( bit_is_enabled( olm->lightBits, ofs ) ) { /* already filled */
					continue;
				}
				cnt = 0;
				Vector3 dir_sum( 0 ), light_sum( 0 );

				/* try all four neighbors */
				ofs = ( ( y + olm->customHeight - 1 ) % olm->customHeight ) * olm->customWidth + x;
				if ( bit_is_enabled( olm->lightBits, ofs ) ) { /* already filled */
					++cnt;
					light_sum += olm->bspLightBytes[ofs];
					if ( deluxemap ) {
						dir_sum += olm->bspDirBytes[ofs];
					}
				}

				ofs = ( ( y + 1 ) % olm->customHeight ) * olm->customWidth + x;
				if ( bit_is_enabled( olm->lightBits, ofs ) ) { /* already filled */
					++cnt;
					light_sum += olm->bspLightBytes[ofs];
					if ( deluxemap ) {
						dir_sum += olm->bspDirBytes[ofs];
					}
				}

				ofs = y * olm->customWidth + ( x + olm->customWidth - 1 ) % olm->customWidth;
				if ( bit_is_enabled( olm->lightBits, ofs ) ) { /* already filled */
					++cnt;
					light_sum += olm->bspLightBytes[ofs];
					if ( deluxemap ) {
						dir_sum += olm->bspDirBytes[ofs];
					}
				}

				ofs = y * olm->customWidth + ( x + 1 ) % olm->customWidth;
				if ( bit_is_enabled( olm->lightBits, ofs ) ) { /* already filled */
					++cnt;
					light_sum += olm->bspLightBytes[ofs];
					if ( deluxemap ) {
						dir_sum += olm->bspDirBytes[ofs];
					}
				}

				if ( cnt ) {
					++filled;
					ofs = y * olm->customWidth + x;
					bit_enable( lightBitsNew, ofs );
					lightBytesNew[ofs] = light_sum * ( 1.0 / cnt );
					if ( deluxemap ) {
						dirBytesNew[ofs] = dir_sum * ( 1.0 / cnt );
					}
				}
			}
		}

		if ( !filled ) {
			break;
		}

		memcpy( olm->lightBits, lightBitsNew, ( olm->customWidth * olm->customHeight + 8 ) / 8 );
		memcpy( olm->bspLightBytes, lightBytesNew, size );
		if ( deluxemap ) {
			memcpy( olm->bspDirBytes, dirBytesNew, size );
		}
	}

	free( lightBitsNew );
	free( lightBytesNew );
	if ( deluxemap ) {
		free( dirBytesNew );
	}
}

/*
   StoreSurfaceLightmaps()
   stores the surface lightmaps into the bsp as byte rgb triplets
 */

void StoreSurfaceLightmaps( bool fastAllocate ){
	
}
