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

   -------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   -------------------------------------------------------------------------------

   "Q3Map2" has been significantly modified in the form of "remap" to support
   Respawn Entertainment games.

   ------------------------------------------------------------------------------- */

#pragma once


/* version */
#ifndef Q3MAP_VERSION
#error no Q3MAP_VERSION defined
#endif
#define REMAP_MOTD  "Built with love using MRVN-Radiant :)"


/* -------------------------------------------------------------------------------

   dependencies

   ------------------------------------------------------------------------------- */


/* general */
#include "version.h"
#include "toolsdef.h"

#include "cmdlib.h"
#include "qstringops.h"
#include "qpathops.h"

#include "scriplib.h"
#include "polylib.h"
#include "qimagelib.h"
#include "qthreads.h"
#include "inout.h"
#include "inout_xml.h"
#include "vfs/vfs.h"
#include "md4.h"

#include "stringfixedsize.h"
#include "stream/stringstream.h"
#include "bitflags.h"
#include <list>
#include <forward_list>
#include "qmath.h"

#include <cstddef>
#include <cstdlib>

#include "maxworld.h"
#include "games.h"


const static std::string REMAP_EMBLEM[] =
{
    R"(    ________  ____ ___  ____  ____ )",
    R"(   / ___/ _ \/ __ `__ \/ __ `/ __ \)",
    R"(  / /  /  __/ / / / / / /_/ / /_/ /)",
    R"( /_/   \___/_/ /_/ /_/\__,_/ .___/ )",
    R"(                          /_/      )"
};

/* -------------------------------------------------------------------------------

   constants

   ------------------------------------------------------------------------------- */


/* temporary hacks and tests (please keep off in SVN to prevent anyone's legacy map from screwing up) */
/* 2011-01-10 TTimo says we should turn these on in SVN, so turning on now */
#define Q3MAP2_EXPERIMENTAL_HIGH_PRECISION_MATH_FIXES  1
#define Q3MAP2_EXPERIMENTAL_SNAP_NORMAL_FIX            1
#define Q3MAP2_EXPERIMENTAL_SNAP_PLANE_FIX             1

/* general */
// actual shader name length limit depends on game engine and name use manner (plain texture/custom shader)
// now checking it for strlen() < MAX_QPATH (so it's null terminated), though this check may be not enough/too much, depending on the use case
#define MAX_QPATH               64

#define DEFAULT_IMAGE           "*default"

#define DEF_BACKSPLASH_FRACTION 0.05f   /* 5% backsplash by default */
#define DEF_BACKSPLASH_DISTANCE 23

#define DEF_RADIOSITY_BOUNCE    1.0f    /* ydnar: default to 100% re-emitted light */

#define MAX_SHADER_INFO         8192


/* epair parsing (note case-sensitivity directive) */
#define CASE_INSENSITIVE_EPAIRS 1

#if CASE_INSENSITIVE_EPAIRS
    #define EPAIR_EQUAL  striEqual
#else
    #define EPAIR_EQUAL  strEqual
#endif


/* shadow flags */
#define WORLDSPAWN_CAST_SHADOWS  1
#define WORLDSPAWN_RECV_SHADOWS  1
#define ENTITY_CAST_SHADOWS      0
#define ENTITY_RECV_SHADOWS      1

/* bsp */
#define MAX_PATCH_SIZE          32
#define MAX_BRUSH_SIDES         1024
#define MAX_BUILD_SIDES         1024

#define MAX_EXPANDED_AXIS       128

#define CLIP_EPSILON            0.1f
#define PLANESIDE_EPSILON       0.001f
#define PLANENUM_LEAF           -1

enum class EBrushType {
    Undefined,
    Quake,
    Bp,
    Valve220
};

/* vis */
#define VIS_HEADER_SIZE  8

#define SEPERATORCACHE  /* separator caching helps a bit */

#define PORTALFILE  "PRT1"

#define MAX_PORTALS                  0x20000  // same as MAX_MAP_PORTALS
#define MAX_SEPERATORS               MAX_POINTS_ON_WINDING
#define MAX_POINTS_ON_FIXED_WINDING  24  /* ydnar: increased this from 12 at the expense of more memory */
#define MAX_PORTALS_ON_LEAF          1024


/* light */
#define MAX_TRACE_TEST_NODES    256
#define DEFAULT_INHIBIT_RADIUS  1.5f

#define LUXEL_EPSILON   0.125f
#define VERTEX_EPSILON -0.125f
#define GRID_EPSILON    0.0f

#define DEFAULT_LIGHTMAP_SAMPLE_SIZE      16
#define DEFAULT_LIGHTMAP_MIN_SAMPLE_SIZE  0
#define DEFAULT_LIGHTMAP_SAMPLE_OFFSET    1.0f
#define DEFAULT_SUBDIVIDE_THRESHOLD       1.0f

#define CLUSTER_UNMAPPED  -1
#define CLUSTER_OCCLUDED  -2
#define CLUSTER_FLOODED   -3

#define FLAG_FORCE_SUBSAMPLING   1
#define FLAG_ALREADY_SUBSAMPLED  2


/* -------------------------------------------------------------------------------

   abstracted bsp file

   ------------------------------------------------------------------------------- */


#define EXTERNAL_LIGHTMAP       "lm_%04d.tga"

#define MAX_LIGHTMAPS           4           /* RBSP */
#define MAX_SWITCHED_LIGHTS     32
#define LS_NORMAL               0x00
#define LS_UNUSED               0xFE
#define LS_NONE                 0xFF

#define MAX_LIGHTMAP_SHADERS    256

/* ok to increase these at the expense of more memory */
#define MAX_MAP_AREAS           0x100       /* MAX_MAP_AREA_BYTES in q_shared must match! */
/* MAX_MAP_FOGS is technically unlimited in engine, but drawsurf sorting code only has 5 bits for fogs */
#define MAX_IBSP_FOGS           31          /* (2^5 - world fog) */
#define MAX_RBSP_FOGS           30          /* (2^5 - world fog - goggles) */
#define MAX_MAP_LEAFS           0x20000
#define MAX_MAP_PORTALS         0x20000
#define MAX_MAP_LIGHTING        0x800000
#define MAX_MAP_LIGHTGRID       0x100000    // % 0x800000
/* ydnar: set to points, not bytes */
#define MAX_MAP_VISCLUSTERS     0x4000 // <= MAX_MAP_LEAFS
#define MAX_MAP_VISIBILITY      ( VIS_HEADER_SIZE + MAX_MAP_VISCLUSTERS * ( ( ( MAX_MAP_VISCLUSTERS + 63 ) & ~63 ) >> 3 ) )

#define MAX_MAP_DRAW_SURFS      0x20000

/* the editor uses these predefined yaw angles to orient entities up or down */
#define ANGLE_UP                -1
#define ANGLE_DOWN              -2

#define LIGHTMAP_WIDTH          128
#define LIGHTMAP_HEIGHT         128




using bspPlane_t = Plane3f;


struct bspBrush_t {
    Vector3   origin;
    uint16_t  flags;
    uint16_t  index;
    Vector3   extents;
    uint32_t  side_plane;
};


// Old
struct bspShader_t {
    char  shader[MAX_QPATH];
    int   surfaceFlags;
    int   contentFlags;
};


/* planes x^1 is always the opposite of plane x */
using bspPlane_t = Plane3f;


struct bspNode_t {
    int planeNum;
    int children[2];  /* negative numbers are -(leafs + 1), not nodes */
    MinMax___<int> minmax;  /* for frustum culling */
};


struct bspLeaf_t {
    int cluster;                    /* -1 = opaque cluster (do I still store these?) */
    int area;

    MinMax___<int> minmax;          /* for frustum culling */

    int firstBSPLeafSurface;
    int numBSPLeafSurfaces;

    int firstBSPLeafBrush;
    int numBSPLeafBrushes;
};


struct bspBrushSide_t {
    int  planeNum;    /* positive plane side faces out of the leaf */
    int  shaderNum;
    int  surfaceNum;  /* RBSP */
};


struct bspDrawVert_t {
    Vector3  xyz;
    Vector2  st;
    Vector2  lightmap[MAX_LIGHTMAPS];  /* RBSP */
    Vector3  normal;
    Color4b  color[MAX_LIGHTMAPS];  /* RBSP */
};


enum bspSurfaceType_t {
    MST_BAD,
    MST_PLANAR,
    MST_PATCH,
    MST_TRIANGLE_SOUP,
    MST_FLARE,
    MST_FOLIAGE
};


struct bspGridPoint_t {
    Vector3b  ambient[MAX_LIGHTMAPS];   /* RBSP - array */
    Vector3b  directed[MAX_LIGHTMAPS];  /* RBSP - array */
    byte      styles[MAX_LIGHTMAPS];    /* RBSP - whole */
    byte      latLong[2];
};


/* -------------------------------------------------------------------------------

   general types

   ------------------------------------------------------------------------------- */


/* ydnar: for q3map_tcMod */
typedef Vector3 tcMod_t[3];


struct image_t {
    CopiedString  name;      // relative path w/o extension
    CopiedString  filename;  // relative path with extension
    int   width, height;
    byte *pixels = nullptr;

    image_t() = default;

    image_t(const char *name, const char *filename, int width, int height, byte *pixels) :
        name(name),
        filename(filename),
        width(width),
        height(height),
        pixels(pixels)
    {}

    image_t(image_t&& other) noexcept :
        name(std::move(other.name)),
        filename(std::move(other.filename)),
        width(other.width),
        height(other.height),
        pixels(std::exchange(other.pixels, nullptr))
    {}

    ~image_t() {
        free(pixels);
    }

    byte *at(int width, int height) const {
        return pixels + 4 * (height * this->width + width);
    }
};


struct sun_t {
    Vector3  direction, color;
    float    photons, deviance, filterRadius;
    int      numSamples, style;
};


struct skylight_t {
    float  value;
    int    iterations;
    int    horizon_min = 0;
    int    horizon_max = 90;
    bool   sample_color = true;
};


struct surfaceModel_t {
    CopiedString  model;
    float         density, odds;
    float         minScale, maxScale;
    float         minAngle, maxAngle;
    bool          oriented;
};


/* ydnar/sd: foliage stuff for wolf et (engine-supported optimization of the above) */
struct foliage_t {
    CopiedString  model;
    float         scale, density, odds;
    int           inverseAlpha;
};


struct foliageInstance_t {
    Vector3  xyz, normal;
};


struct remap_t {
    char  from[1024];
    char  to[MAX_QPATH];
};


enum class EColorMod {
    None,
    Volume,
    ColorSet,
    AlphaSet,
    ColorScale,
    AlphaScale,
    ColorDotProduct,
    AlphaDotProduct,
    ColorDotProductScale,
    AlphaDotProductScale,
    ColorDotProduct2,
    AlphaDotProduct2,
    ColorDotProduct2Scale,
    AlphaDotProduct2Scale
};


struct colorMod_t {
    colorMod_t *next;
    EColorMod   type;
    float       data[16];
};


enum class EImplicitMap {
    None,
    Opaque,
    Masked,
    Blend
};


struct shaderInfo_t {
    String512                  shader;
    int                        surfaceFlags;
    int                        contentFlags;
    int                        compileFlags;
    float                      value;                 /* light value */

    const char                *flareShader;           /* for light flares */
    char                      *damageShader;          /* ydnar: sof2 damage shader name */
    char                      *backShader;            /* for surfaces that generate different front and back passes */
    char                      *cloneShader;           /* ydnar: for cloning of a surface */
    char                      *remapShader;           /* ydnar: remap a shader in final stage */
    char                      *deprecateShader;       /* vortex: shader is deprecated and replaced by this on use */

    std::list<surfaceModel_t>  surfaceModels;         /* ydnar: for distribution of models */
    std::list<foliage_t>       foliage;               /* ydnar/splash damage: wolf et foliage */

    float                      subdivisions;          /* from a "tesssize xxx" */
    float                      backsplashFraction;    /* floating point value, usually 0.05 */
    float                      backsplashDistance;    /* default 16 */
    float                      lightSubdivide;        /* default 999 */
    float                      lightFilterRadius;     /* ydnar: lightmap filtering/blurring radius
                                                                for lights created by this shader (default: 0) */
    int                        lightmapSampleSize;    /* lightmap sample size */
    float                      lightmapSampleOffset;  /* ydnar: lightmap sample offset (default: 1.0) */

    float                      bounceScale;           /* ydnar: radiosity re-emission [0,1.0+] */
    float                      offset;                /* ydnar: offset in units */
    float                      shadeAngleDegrees;     /* ydnar: breaking angle for smooth shading (degrees) */

    MinMax                     minmax;                /* ydnar: for particle studio vertexDeform move support */

    bool                       legacyTerrain;         /* ydnar: enable legacy terrain crutches */
    bool                       indexed;               /* ydnar: attempt to use indexmap (terrain alphamap style) */
    bool                       forceMeta;             /* ydnar: force metasurface path */
    bool                       noClip;                /* ydnar: don't clip into bsp, preserve original face winding */
    bool                       noFast;                /* ydnar: suppress fast lighting for surfaces with this shader */
    bool                       invert;                /* ydnar: reverse facing */
    bool                       nonplanar;             /* ydnar: for nonplanar meta surface merging */
    bool                       tcGen;                 /* ydnar: has explicit texcoord generation */
    Vector3                    vecs[2];               /* ydnar: explicit texture vectors for [0,1] texture space */
    tcMod_t                    mod;                   /* ydnar: q3map_tcMod matrix for djbob :) */
    Vector3                    lightmapAxis {0};      /* ydnar: explicit lightmap axis projection */
    colorMod_t                *colorMod;              /* ydnar: q3map_rgb/color/alpha/Set/Mod support */

    int                        furNumLayers;          /* ydnar: number of fur layers */
    float                      furOffset;             /* ydnar: offset of each layer */
    float                      furFade;               /* ydnar: alpha fade amount per layer */

    bool                       splotchFix;            /* ydnar: filter splotches on lightmaps */

    bool                       hasPasses;             /* false if the shader doesn't define any rendering passes */
    bool                       globalTexture;         /* don't normalize texture repeats */
    bool                       twoSided;              /* cull none */
    bool                       autosprite;            /* autosprite shaders will become point lights instead of area lights */
    bool                       polygonOffset;         /* ydnar: don't face cull this or against this */
    bool                       patchShadows;          /* have patches casting shadows when using -light for this surface */
    bool                       vertexShadows;         /* shadows will be casted at this surface even when vertex lit */
    bool                       forceSunlight;         /* force sun light at this surface
                                                         even tho we might not calculate shadows in vertex lighting */
    bool                       notjunc;               /* don't use this surface for tjunction fixing */
    bool                       fogParms;              /* ydnar: has fogparms */
    bool                       noFog;                 /* ydnar: suppress fogging */
    bool                       clipModel;             /* ydnar: solid model hack */
    bool                       noVertexLight;         /* ydnar: leave vertex color alone */
    bool                       noDirty;               /* jal: do not apply the dirty pass to this surface */

    byte                       styleMarker;           /* ydnar: light styles hack */

    float                      vertexScale;           /* vertex light scale */

    String512                  skyParmsImageBase;     /* ydnar: for skies */

    String512                  editorImagePath;       /* use this image to generate texture coordinates */
    String512                  lightImagePath;        /* use this image to generate color / averageColor */
    String512                  normalImagePath;       /* ydnar: normalmap image for bumpmapping */

    String512                  baseTexture2Path;

    EImplicitMap               implicitMap;           /* ydnar: enemy territory implicit shaders */
    String512                  implicitImagePath;

    const image_t             *shaderImage;
    const image_t             *lightImage;
    const image_t             *normalImage;

    std::vector<skylight_t>    skylights;             /* ydnar */
    std::vector<sun_t>         suns;                  /* ydnar */

    Vector3                    color {0};             /* normalized color */
    Color4f                    averageColor = {0, 0, 0, 0};
    byte                       lightStyle;

    /* vortex: per-surface floodlight */
    float                      floodlightDirectionScale;
    Vector3                    floodlightRGB;
    float                      floodlightIntensity;
    float                      floodlightDistance;

    bool                       lmMergable;            /* ydnar */
    int                        lmCustomWidth;         /* ydnar */
    int                        lmCustomHeight;        /* ydnar */
    float                      lmBrightness;          /* ydnar */
    float                      lmFilterRadius;        /* ydnar: lightmap filtering/blurring radius for this shader (default: 0) */
    int                        shaderWidth;           /* ydnar */
    int                        shaderHeight;          /* ydnar */
    Vector2                    stFlat;

    Vector3                    fogDir {0};            /* ydnar */

    char                      *shaderText;            /* ydnar */
    bool                       custom;
    bool                       finished;
};


/* -------------------------------------------------------------------------------

   bsp structures

   ------------------------------------------------------------------------------- */

struct face_t {
    int        planenum;
    int        priority;
    // bool       checked;
    int        compileFlags;
    winding_t  w;
};
using facelist_t = std::forward_list<face_t>;


struct plane_t {
    Plane3f plane;

    Vector3       &normal()       { return plane.normal(); }
    const Vector3 &normal() const { return plane.normal(); }

    float       &dist()       { return plane.dist(); }
    const float &dist() const { return plane.dist(); }

    EPlaneType  type;
    int         counter;
    int         hash_chain;
};


struct side_t {
    int           planenum;

    int           outputNum;           /* set when the side is written to the file list */

    Vector3       texMat[2];           /* brush primitive texture matrix */
    Vector4       vecs[2];             /* old-style texture coordinate mapping */

    Plane3        plane {0, 0, 0, 0};  /* optional plane in double precision for building windings */
    winding_t     winding;
    winding_t     visibleHull;         /* convex hull of all visible fragments */

    shaderInfo_t *shaderInfo;

    int           contentFlags;        /* from shaderInfo */
    int           surfaceFlags;        /* from shaderInfo */
    int           compileFlags;        /* from shaderInfo */
    int           value;               /* from shaderInfo */

    bool          bevel;               /* don't ever use for bsp splitting, and don't bother making windings for it */
    bool          culled;              /* ydnar: face culling */
};


struct sideRef_t {
    sideRef_t    *next;
    const side_t *side;
};


/* ydnar: generic index mapping for entities (natural extension of terrain texturing) */
struct indexMap_t {
    int        w;
    int        h;
    int        numLayers;
    String512  shader;
    float      offsets[256];
    byte      *pixels;
};


struct brush_t {
    brush_t             *original;             /* chopped up brushes will reference the originals */

    int                  entityNum, brushNum;  /* editor numbering */
    int                  outputNum;            /* set when the brush is written to the file list */

    /* ydnar: for shadowcasting entities */
    int                  castShadows;
    int                  recvShadows;

    shaderInfo_t        *contentShader;
    shaderInfo_t        *celShader;            /* :) */

    /* ydnar: gs mods */
    int                  lightmapSampleSize;   /* jal : entity based _lightmapsamplesize */
    float                lightmapScale;
    float                shadeAngleDegrees;    /* jal : entity based _shadeangle */
    MinMax               eMinmax;
    indexMap_t          *im;

    int                  contentFlags;
    int                  compileFlags;         /* ydnar */
    bool                 detail;
    bool                 opaque;

    int                  portalareas[2];

    MinMax               minmax;

    std::vector<side_t>  sides;
};
using brushlist_t = std::list<brush_t>;


struct fog_t {
    shaderInfo_t  *si;
    const brush_t *brush;
    int            visibleSide;  /* the brush side that ray tests need to clip against (-1 == none) */
};


struct mesh_t {
    int            width;
    int            height;
    bspDrawVert_t *verts;
};


struct parseMesh_t {
    parseMesh_t  *next;

    /* ydnar: editor numbering */
    int           entityNum;
    int           brushNum;

    /* ydnar: for shadowcasting entities */
    int           castShadows;
    int           recvShadows;

    mesh_t        mesh;
    shaderInfo_t *shaderInfo;
    shaderInfo_t *celShader;            /* :) */

    /* ydnar: gs mods */
    int           lightmapSampleSize;  /* jal : entity based _lightmapsamplesize */
    float         lightmapScale;
    MinMax        eMinmax;
    indexMap_t   *im;

    /* grouping */
    bool          grouped;
    float         longestCurve;
    int           maxIterations;
};


/*
    ydnar: the drawsurf struct was extended to allow for:
    - non-convex planar surfaces
    - non-planar brushface surfaces
    - lightmapped terrain
    - planar patches
*/

enum class ESurfaceType {
    /* ydnar: these match up exactly with bspSurfaceType_t */
    Bad,
    Face,
    Patch,
    Triangles,
    Flare,
    Foliage,    /* wolf et */

    /* ydnar: compiler-relevant surface types */
    ForcedMeta,
    Meta,
    Foghull,
    Decal,
    Shader  /* this is used to define number of enum items */
};

constexpr const char *surfaceTypeName(ESurfaceType type) {
    switch (type) {
        case ESurfaceType::Bad:        return "ESurfaceType::Bad";
        case ESurfaceType::Face:       return "ESurfaceType::Face";
        case ESurfaceType::Patch:      return "ESurfaceType::Patch";
        case ESurfaceType::Triangles:  return "ESurfaceType::Triangles";
        case ESurfaceType::Flare:      return "ESurfaceType::Flare";
        case ESurfaceType::Foliage:    return "ESurfaceType::Foliage";
        case ESurfaceType::ForcedMeta: return "ESurfaceType::ForcedMeta";
        case ESurfaceType::Meta:       return "ESurfaceType::Meta";
        case ESurfaceType::Foghull:    return "ESurfaceType::Foghull";
        case ESurfaceType::Decal:      return "ESurfaceType::Decal";
        case ESurfaceType::Shader:     return "ESurfaceType::Shader";
    }
    return "SURFACE NAME ERROR";
}


/* ydnar: this struct needs an overhaul (again, heh) */
struct mapDrawSurface_t {
    ESurfaceType      type;
    bool              planar;
    int               outputNum;        /* ydnar: to match this sort of thing up */

    bool              fur;              /* ydnar: this is kind of a hack, but hey... */
    bool              skybox;           /* ydnar: yet another fun hack */
    bool              backSide;         /* ydnar: q3map_backShader support */

    mapDrawSurface_t *parent;           /* ydnar: for cloned (skybox) surfaces to share lighting data */
    mapDrawSurface_t *clone;            /* ydnar: for cloned surfaces */
    mapDrawSurface_t *cel;              /* ydnar: for cloned cel surfaces */

    shaderInfo_t     *shaderInfo;
    shaderInfo_t     *celShader;
    const brush_t    *mapBrush;
    parseMesh_t      *mapMesh;
    sideRef_t        *sideRef;

    int               fogNum;

    int               numVerts;         /* vertexes and triangles */
    bspDrawVert_t    *verts;
    int               numIndexes;
    int              *indexes;

    int               planeNum;
    Vector3           lightmapOrigin;   /* also used for flares */
    Vector3           lightmapVecs[3];  /* also used for flares */
    int               lightStyle;       /* used for flares */

    /* ydnar: per-surface (per-entity, actually) lightmap sample size scaling */
    float             lightmapScale;

    /* jal: per-surface (per-entity, actually) shadeangle */
    float             shadeAngleDegrees;

    /* ydnar: surface classification */
    MinMax            minmax;
    Vector3           lightmapAxis;
    int               sampleSize;

    /* ydnar: shadow group support */
    int               castShadows;
    int               recvShadows;

    /* ydnar: for patches */
    float             longestCurve;
    int               maxIterations;
    int               patchWidth;
    int               patchHeight;
    MinMax            bounds;

    /* ydnar/sd: for foliage */
    int               numFoliageInstances;

    /* ydnar: editor/useful numbering */
    int               entityNum;
    int               surfaceNum;
};


struct drawSurfRef_t {
    drawSurfRef_t *nextRef;
    int            outputNum;
};


struct epair_t { CopiedString  key, value; };


struct entity_t {
    Vector3                origin{ 0 };
    brushlist_t            brushes;
    std::vector<brush_t*>  colorModBrushes;
    parseMesh_t           *patches;
    int                    mapEntityNum;            /* .map file entities numbering */
    int                    firstDrawSurf;
    int                    firstBrush, numBrushes;  /* only valid during BSP compile */
    std::list<epair_t>     epairs;
    Vector3                originbrush_origin {0};

    void        setKeyValue(const char *key, const char *value);
    const char *valueForKey(const char *key) const;

    template<typename ... Keys>
    bool boolForKey(Keys ... keys) const {
        bool bool_value = false;
        read_keyvalue_(bool_value, { keys ... });
        return bool_value;
    }

    template<typename ... Keys>
    int intForKey(Keys ... keys) const {
        int int_value = 0;
        read_keyvalue_(int_value, { keys ... });
        return int_value;
    }

    template<typename ... Keys>
    float floatForKey(Keys ... keys) const {
        float float_value = 0;
        read_keyvalue_(float_value, { keys ... });
        return float_value;
    }

    Vector3 vectorForKey(const char *key) const {
        Vector3 vec(0);
        read_keyvalue_(vec, { key });
        return vec;
    }

    const char *classname() const {
        return valueForKey("classname");
    }

    bool classname_is(const char *name) const {
        return striEqual(classname(), name);
    }

    bool classname_prefixed(const char *prefix) const {
        return striEqualPrefix(classname(), prefix);
    }

    /* entity: read key value variadic template
    returns true on successful read
    returns false and does not modify value otherwise */
    template<typename T, typename ... Keys>
    bool read_keyvalue(T &value_ref, Keys ... keys) const {
            return read_keyvalue_(value_ref, { keys ... });
    }

private:
    bool read_keyvalue_(bool              &bool_value, std::initializer_list<const char*> &&keys ) const;
    bool read_keyvalue_(int                &int_value, std::initializer_list<const char*> &&keys ) const;
    bool read_keyvalue_(float            &float_value, std::initializer_list<const char*> &&keys ) const;
    bool read_keyvalue_(Vector3        &vector3_value, std::initializer_list<const char*> &&keys ) const;
    bool read_keyvalue_(const char *&string_ptr_value, std::initializer_list<const char*> &&keys ) const;
};


struct node_t {
    /* both leafs and nodes */
    int              planenum;            /* -1 = leaf node */
    node_t          *parent;
    MinMax           minmax;              /* valid after portalization */

    /* nodes only */
    node_t          *children[2];
    int              compileFlags;        /* ydnar: hint, antiportal */
    int              tinyportals;
    Vector3          referencepoint;

    /* leafs only */
    bool             opaque;              /* view can never be inside */
    bool             areaportal;
    bool             skybox;              /* ydnar: a skybox leaf */
    bool             sky;                 /* ydnar: a sky leaf */
    int              cluster;             /* for portalfile writing */
    int              area;                /* for areaportals */
    brushlist_t      brushlist;           /* fragments of all brushes in this leaf */
    drawSurfRef_t   *drawSurfReferences;

    int              occupied;            /* 1 or greater can reach entity */
    const entity_t  *occupant;            /* for leak file testing */

    struct portal_t *portals;             /* also on nodes during construction */

    bool             has_structural_children;
};


class Args {
private:
    const char                               *m_arg0;
    std::vector<const char*>                  m_args;
    std::vector<const char*>::const_iterator  m_next;
    const char                               *m_current;

public:
    Args(int argc, char **argv){
        ENSURE(argc > 0);
        m_arg0 = argv[0];
        m_args = {argv + 1, argv + argc};
    }

    const char *getArg0() const { return m_arg0; }

    std::vector<const char*> getVector() { return m_args; }

    template<typename ...Args>
    bool takeArg( Args... args) {
        const std::array<const char*, sizeof...(Args)>  array = { args ... };
        for (auto &&arg : array) {
            for (auto it = m_args.cbegin(); it != m_args.cend(); ++it) {
                if (striEqual(*it, arg)) {
                    m_current = *it;
                    m_next = m_args.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    /* next three are only valid after takeArg() == true */
    const char *takeNext(){
        if (m_next == m_args.cend()) { Error("Out of arguments: No parameters specified after %s", m_current); }
        const char *ret = *m_next;
        m_next = m_args.erase(m_next);
        return ret;
    }

    bool nextAvailable() const { return(m_next != m_args.cend()); }
    const char *next() const { return *m_next; }
    /* --- */
    size_t size() const { return m_args.size(); }
    bool empty() const { return size() == 0; }

    bool takeFront(const char *arg) {
        if (!m_args.empty()
         && striEqual(m_args.front(), arg)) {
            m_args.erase(m_args.cbegin());
            return true;
        }
        return false;
    }

    const char *takeFront() {
        ENSURE(!m_args.empty());
        const char *ret = m_args.front();
        m_args.erase(m_args.cbegin());
        return ret;
    }

    const char *takeBack() {
        ENSURE(!m_args.empty());
        const char *ret = m_args.back();
        m_args.pop_back();
        return ret;
    }
};


/* -------------------------------------------------------------------------------

   prototypes

   ------------------------------------------------------------------------------- */

inline float Random() { return (float)rand() / RAND_MAX; }  /* returns a pseudorandom number between 0 and 1 */


/* path_init.c */
const game_t *GetGame(const char *arg);
void InitPaths(Args &args);

/* bsp.c */
int BSPMain(Args& args);

/* brush.c */
sideRef_t *AllocSideRef(const side_t *side, sideRef_t *next);
Vector3 SnapWeldVector(const Vector3 &a, const Vector3 &b);
bool CreateBrushWindings(brush_t &brush);
void WriteBSPBrushMap(const char *name, const brushlist_t &list);
void FilterDetailBrushesIntoTree(const entity_t &e, tree_t &tree);
void FilterStructuralBrushesIntoTree(const entity_t &e, tree_t &tree);
bool WindingIsTiny(const winding_t &w);

/* mesh.c */
void LerpDrawVert(const bspDrawVert_t *a, const bspDrawVert_t *b, bspDrawVert_t *out);
void LerpDrawVertAmount(bspDrawVert_t *a, bspDrawVert_t *b, float amount, bspDrawVert_t *out);
void FreeMesh(mesh_t *m);
mesh_t *CopyMesh(mesh_t *mesh);
void PrintMesh(mesh_t *m);
mesh_t *TransposeMesh(mesh_t *in);
void InvertMesh(mesh_t *m);
mesh_t *SubdivideMesh(mesh_t in, float maxError, float minLength);
int IterationsForCurve(float len, int subdivisions);
mesh_t *SubdivideMesh2(mesh_t in, int iterations);
mesh_t *RemoveLinearMeshColumnsRows(mesh_t *in);
void MakeMeshNormals(mesh_t in);
void PutMeshOnCurve(mesh_t in);

/* map.c */
void LoadMapFile(const char *filename, bool onlyLights, bool noCollapseGroups);
int FindFloatPlane(const Plane3f &plane, int numPoints, const Vector3 *points);

inline int FindFloatPlane(const Vector3 &normal, float dist, int numPoints, const Vector3 *points) {
    return FindFloatPlane(Plane3f(normal, dist), numPoints, points);
}

bool PlaneEqual(const plane_t &p, const Plane3f &plane);
bool SnapNormal(Vector3 &normal);
void AddBrushBevels();

/* portals.c */
bool PortalPassable(const portal_t *p);
void RemovePortalFromNode(portal_t *portal, node_t *l);

enum class EFloodEntities {
    Leaked,
    Good,
    Empty
};

EFloodEntities FloodEntities(tree_t &tree);
void FillOutside(node_t *headnode);
void FloodAreas(tree_t &tree);
inline portal_t *AllocPortal() { return new portal_t(); }
inline void FreePortal(portal_t *p) { delete p; }
void MakeTreePortals(tree_t &tree);


/* prtfile.c */
void NumberClusters(tree_t &tree);
void WritePortalFile(const tree_t &tree);

/* patch.c */
void ParsePatch(bool onlyLights, entity_t &mapEnt, int mapPrimitiveNum);

/* model.c */
void assimp_init();

/* map.c */
std::array<Vector3, 2> TextureAxisFromPlane(const plane_t &plane);

/* image.c */
const image_t *ImageLoad(const char *name);


/* shaders.c */
void ColorMod(const colorMod_t *colormod, int numVerts, bspDrawVert_t *drawVerts);
void TCMod(const tcMod_t &mod, Vector2 &st);

bool ApplyShaderType( const char *name, int *surfaceFlags, int *contentFlags, int *compileFlags );
bool ApplyShaderFlag( const char *name, int *surfaceFlags, int *contentFlags, int *compileFlags );

void LoadShaderInfo();
shaderInfo_t *ShaderInfoForShader(const char *shader);
shaderInfo_t *ShaderInfoForShaderNull(const char *shader);


/* bspfile_abstract.c */
void WriteBSPFile(const char *filename);
void WriteEntFiles(const char *path );

void ParseEPair(std::list<epair_t> &epairs);
void ParseEntities();
void PrintEntity(const entity_t *ent);

entity_t *FindTargetEntity(const char *target);
void GetEntityShadowFlags(const entity_t *ent, const entity_t *ent2, int *castShadows, int *recvShadows);
void InjectCommandLine(const char *stage, const std::vector<const char*> &args);


/* -------------------------------------------------------------------------------

   bsp/general global variables

   ------------------------------------------------------------------------------- */
// NOTE [Fifty]: Referenced in other source files so keeping untill i get to those

/* general */
inline shaderInfo_t *shaderInfo;
inline int           numShaderInfo;

inline String512     mapName;  /* ydnar: per-map custom shaders for larger lightmaps */


/* general commandline arguments */
inline int   patchSubdivisions = 8;  /* ydnar: -patchmeta subdivisions */

/* commandline arguments */
inline bool  nodetail;
inline bool  fulldetail;
inline bool  nowater;
inline bool  noCurveBrushes;
inline bool  noHint;                    /* ydnar */

inline int        maxSurfaceVerts = 999;     /* ydnar */
inline int        bevelSnap;                 /* ydnar: bevel plane snap */
inline bool       debugClip;                 /* debug model autoclipping */
inline String512  globalCelShader;

inline bool  g_bExternalModels;


#if Q3MAP2_EXPERIMENTAL_SNAP_NORMAL_FIX
// Increasing the normalEpsilon to compensate for new logic in SnapNormal(), where
// this epsilon is now used to compare against 0 components instead of the 1 or -1
// components.  Unfortunately, normalEpsilon is also used in PlaneEqual().  So changing
// this will affect anything that calls PlaneEqual() as well (which are, at the time
// of this writing, FindFloatPlane() and AddBrushBevels()).
inline double normalEpsilon = 0.00005;
#else
inline double normalEpsilon = 0.00001;
#endif


#if Q3MAP2_EXPERIMENTAL_HIGH_PRECISION_MATH_FIXES
// NOTE: This distanceEpsilon is too small if parts of the map are at maximum world
// extents (in the range of plus or minus 2^16).  The smallest epsilon at values
// close to 2^16 is about 0.007, which is greater than distanceEpsilon.  Therefore,
// maps should be constrained to about 2^15, otherwise slightly undesirable effects
// may result.  The 0.01 distanceEpsilon used previously is just too coarse in my
// opinion.  The real fix for this problem is to have 64 bit distances and then make
// this epsilon even smaller, or to constrain world coordinates to plus minus 2^15
// (or even 2^14).
inline double distanceEpsilon = 0.005;
#else
inline double distanceEpsilon = 0.01;
#endif


/* bsp */
inline int  blockSize[3] = { 1024, 1024, 1024 };  /* should be the same as in radiant */

inline CopiedString  g_enginePath;

inline char  source[1024];

inline int  sampleSize    = DEFAULT_LIGHTMAP_SAMPLE_SIZE;      /* lightmap sample size in units */
inline int  minSampleSize = DEFAULT_LIGHTMAP_MIN_SAMPLE_SIZE;  /* minimum sample size to use at all */
inline int  sampleScale;                                       /* vortex: lightmap sample scale (ie quality) */

inline std::vector<plane_t>  mapplanes;
/* mapplanes[num ^ 1] will always be the mirror or mapplanes[num] */
/* nummapplanes will always be even */
/* true for all Quake-based engines except RespawnSource */
inline MinMax  g_mapMinmax;

inline const MinMax  c_worldMinmax(Vector3(MIN_WORLD_COORD), Vector3(MAX_WORLD_COORD));


inline brush_t     buildBrush;
inline EBrushType  g_brushType = EBrushType::Undefined;

inline mapDrawSurface_t* mapDrawSurfs;
inline int               numMapDrawSurfs;
inline int      skyboxArea = -1;
inline Matrix4  skyboxTransform;
inline int         lmCustomSizeW = LIGHTMAP_WIDTH;
inline int         lmCustomSizeH = LIGHTMAP_WIDTH;
inline float  vertexglobalscale         =    1.0f;
inline float  g_backsplashFractionScale =    1.0f;
inline float  g_backsplashDistance      = -999.0f;
inline float  lightmapBrightness   = 1.0f;



/* -------------------------------------------------------------------------------

   abstracted bsp globals

   ------------------------------------------------------------------------------- */

/* global helpers */
inline std::size_t            numBSPEntities;
inline std::vector<entity_t>  entities;


#define CHECK_FLAG(value, flag) (value & flag) == flag
#define REMOVE_FLAG(value, flag) value &= ~flag

#define AUTOEXPAND_BY_REALLOC(ptr, reqitem, allocated, def)\
    do {\
        if (reqitem >= allocated) {\
            if (allocated == 0)                       { allocated = def; }\
            while (reqitem >= allocated && allocated) { allocated *= 2; }\
            if (!allocated || allocated > 2147483647 / (int)sizeof(*ptr)) { Error(#ptr " over 2 GB"); }\
            ptr = CVoidPtr(realloc(ptr, sizeof(*ptr) * allocated));\
            if (!ptr) { Error(#ptr " out of memory"); }\
        }\
    } while (0)

#define AUTOEXPAND_BY_REALLOC_ADD(ptr, used, allocated, add) \
    do {\
        if (used >= allocated) {\
            allocated += add;\
            ptr = CVoidPtr(realloc(ptr, sizeof(*ptr) * allocated));\
            if (!ptr) { Error(#ptr " out of memory"); }\
        }\
    } while (0)

#define Image_LinearFloatFromsRGBFloat(c)  (((c) <= 0.04045f) ? (c) * (1.0f / 12.92f) : (float)pow(((c) + 0.055f) * (1.0f / 1.055f), 2.4f))
#define Image_sRGBFloatFromLinearFloat(c)  (((c) < 0.0031308f) ? (c) * 12.92f : 1.055f * (float)pow((c), 1.0f / 2.4f) - 0.055f)
