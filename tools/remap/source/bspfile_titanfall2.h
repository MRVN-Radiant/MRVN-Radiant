#pragma once

#include "bspfile_titanfall.h"
#include "qmath.h"
#include <cstdint>
#include "remap.h"
#include "lump_names.h"


void WriteR2EntFiles(const char* filename);

void LoadR2BSPFile( rbspHeader_t *header, const char *filename );
void WriteR2BSPFile(const char* filename);
void CompileR2BSPFile();


/* -------------------------------------------------------------------------------

   abstracted bsp globals

   ------------------------------------------------------------------------------- */


namespace Titanfall2 {
    void EmitEntity(const entity_t &e);
    void EmitStubs();

    struct GameLump_Path {
        char path[128];
    };

    struct GameLump_Prop {
        Vector3   origin;
        Vector3   angles;
        float     scale;
        uint16_t  modelName;
        uint8_t   solidMode;
        uint8_t   flags;
        int8_t    unk[4];
        float     fadeScale;
        Vector3   lightingOrigin;
        int8_t    cpuLevel[2];
        int8_t    gpuLevel[2];
        int8_t    diffuseModulation[4];
        uint16_t  collisionFlags[2];
    };

    struct GameLump_t {
        uint32_t  version;   // 1
        char      magic[4];  // "prps"
        uint32_t  const0;    // always 851968
        uint32_t  offset;    // offset to path_count
        uint32_t  length;
        uint32_t  pathCount;
        std::vector<GameLump_Path>  paths;
        uint32_t  propCount;
        uint32_t  unk3;  // From testing can be same as prop_count
        uint32_t  unk4;  // Same for this one
        std::vector<GameLump_Prop>  props;
        uint32_t  unk5;
    };

    namespace Bsp {
        /* Stubs */
        inline std::vector<uint8_t>  worldLights_stub;
        inline std::vector<uint8_t>  lightMapHeaders_stub;
        inline std::vector<uint8_t>  lightMapDataSky_stub;
    }

    inline GameLump_t  GameLump;
}
