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

    namespace Bsp {
        /* Stubs */
        inline std::vector<uint8_t>  worldLights_stub;
        inline std::vector<uint8_t>  lightMapHeaders_stub;
        inline std::vector<uint8_t>  lightMapDataSky_stub;
    }
}
