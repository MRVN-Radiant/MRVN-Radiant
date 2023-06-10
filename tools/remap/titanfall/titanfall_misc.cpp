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

#include "../remap.h"
#include "../bspfile_abstract.h"
#include "titanfall.h"


/*
    EmitPlane
*/
void Titanfall::EmitPlane(const Plane3 &plane) {
    Titanfall::Bsp::planes.emplace_back(Plane3f(plane));
}

/*
   EmitEntity()
   Saves an entity into it's corresponding .ent file or the lump in the .bsp
*/
void Titanfall::EmitEntity(const entity_t &e) {
    // temporary enums
    #define ENT_BSP     0
    #define ENT_ENV     1
    #define ENT_FX      2
    #define ENT_SCRIPT  3
    #define ENT_SND     4
    #define ENT_SPAWN   5

    int dest = ENT_BSP;  // deferred write
    const char *classname = e.classname();
    entity_t e2 = (entity_t)e;  // stripping const so we can make changes

    // NOTE: key string compares also exist as methods but we really need to unroll all those function calls
    // -- resist the urge to optimise all these string compares, there are more important jobs to do.
    #define ENT_IS(x)          striEqual(classname, x)
    #define ENT_STARTS(x)      striEqualPrefix(classname, x)
    #define EDITORCLASS(x, y)  if (ENT_IS(y)) { e2.setKeyValue("classname", x); e2.setKeyValue("editorclass", y); }
    // env
    if (ENT_IS("env_fog_controler")
     || ENT_IS("sky_camera")) {
        dest = ENT_ENV;
    // fx
    } else if (ENT_IS("beam_spotlight")
            || ENT_IS("env_sprite_clientside")
            || ENT_STARTS("info_target_fx")
            || ENT_IS("info_particle_system")) {
        dest = ENT_FX;
        /* editorclass conversion */
        EDITORCLASS("info_target", "info_target_fx")
        else EDITORCLASS("info_target_clientside", "info_target_fx_clientside")
    // script
    } else if (ENT_IS("assault_assaultpoint")
            || ENT_IS("info_hardpoint")
            || ENT_IS("info_hint")
            || ENT_STARTS("info_node")
            || ENT_IS("info_target")
            || ENT_IS("info_target_clientside")
            || ENT_IS("path_track")
            || ENT_IS("prop_control_panel")
            || ENT_IS("prop_dynamic")
            || ENT_IS("prop_refuel_pump")
            || ENT_IS("script_ref")
            || ENT_IS("traverse")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        EDITORCLASS("info_node", "info_node_spectre")
    // snd
    } else if (ENT_IS("ambient_generic")) {
        dest = ENT_SND;
    // spawn
    } else if (ENT_IS("info_frontline")
            || ENT_STARTS("info_spawnpoint_")) {
        dest = ENT_SPAWN;
    // TODO: filter out editor only entities *just* before this else
    // e.g. prop_static, env_cubemap, info_lightprobe, light (static worldlights), etc.
    // !!! and make sure those entities still get used for the appropriate lumps !!!
    // bsp
    } else {
        dest = ENT_BSP;
    }
    #undef ENT_IS
    #undef ENT_STARTS
    #undef EDITORCLASS

    // TODO: general entity edits (e.g. float angle (Yaw) -> float[3] angles (Pitch Yaw Roll))

    // write
    StringOutputStream  data;
    data << "{\n";
    for (const epair_t &pair : e2.epairs) {  // e2 to get editorclass changes
        data << "\"" << pair.key.c_str() << "\" \"" << pair.value.c_str() << "\"\n";
    }
    data << "}\n";

    std::vector<char> str = { data.begin(), data.end() };

    #define ENT_APPEND(x)  Titanfall::Ent::x.insert(Titanfall::Ent::x.end(), str.begin(), str.end()); break
    switch (dest) {
        case ENT_BSP:
            Titanfall::Bsp::entities.insert(Titanfall::Bsp::entities.end(), str.begin(), str.end()); break;
        case ENT_ENV:
            ENT_APPEND(env);
        case ENT_FX:
            ENT_APPEND(fx);
        case ENT_SCRIPT:
            ENT_APPEND(script);
        case ENT_SND:
            ENT_APPEND(snd);
        case ENT_SPAWN:
            ENT_APPEND(spawn);
        // NOTE: default should never be hit
    }
    #undef ENT_APPEND
    #undef ENT_BSP
    #undef ENT_ENV
    #undef ENT_FX
    #undef ENT_SCRIPT
    #undef ENT_SND
    #undef ENT_SPAWN
}

/*
    EmitExtraEntity
    Emits an entity into Titanfall::Ent::extra, only used when -onlyentities argument is used
*/
void Titanfall::EmitExtraEntity( entity_t &e ) {
    if( striEqual( e.classname(), "worldspawn" ) )
        return;

    Titanfall::EmitTriggerBrushPlaneKeyValues( e );

    StringOutputStream  data;
    data << "{\n";
    for( const epair_t& pair : e.epairs ) {
        data << "\"" << pair.key.c_str() << "\" \"" << pair.value.c_str() << "\"\n";
    }
    data << "}\n";

    std::vector<char> str = { data.begin(), data.end() };

    Titanfall::Ent::extra.insert( Titanfall::Ent::extra.end(), str.begin(), str.end() );
}

/*
    EmitTriggerBrushPlaneKeyValues()
    Emits brush planes as keyvalues because respawn
*/
void Titanfall::EmitTriggerBrushPlaneKeyValues(entity_t &e) {
    MinMax  minmax;
    std::size_t  b = 0;
    for (const brush_t &brush : e.brushes) {
        minmax.extend(brush.minmax);

        for (std::size_t s = 0; s < brush.sides.size(); s++) {
            const side_t &side = brush.sides.at(s);
            StringOutputStream  key;
            key << "*trigger_brush_" << b << "_plane_" << s;

            StringOutputStream  value;
            value << side.plane.a << " " << side.plane.b << " " << side.plane.c << " " << side.plane.d;

            e.setKeyValue(key.c_str(), value.c_str());
        }
        b++;
    }

    {
        StringOutputStream  key;
        key << "*trigger_bounds_mins";

        StringOutputStream  value;
        value << minmax.mins.x() << " " << minmax.mins.y() << " " << minmax.mins.z();

        e.setKeyValue(key.c_str(), value.c_str());
    }
    {
        StringOutputStream  key;
        key << "*trigger_bounds_maxs";

        StringOutputStream  value;
        value << minmax.maxs.x() << " " << minmax.maxs.y() << " " << minmax.maxs.z();

        e.setKeyValue(key.c_str(), value.c_str());
    }
}

/*
    BeginModel()
    Creates a new model entry
*/
void Titanfall::BeginModel(entity_t &entity) {
    Sys_FPrintf( SYS_VRB, "   BeginModel: \"%s\"\n", entity.classname() );

    Titanfall::Model_t &model = Titanfall::Bsp::models.emplace_back();
    model.firstMesh = (uint32_t)Titanfall::Bsp::meshes.size();

    StringOutputStream ss;
    ss << "*" << Titanfall::Bsp::models.size() - 1;

    if( entity.mapEntityNum != 0 ) {
        entity.setKeyValue("model", ss.c_str());
    }
}


/*
    EndModel()
    Fills the last model entry
*/
void Titanfall::EndModel() {
    Titanfall::Model_t &model = Titanfall::Bsp::models.back();
    model.meshCount = (uint32_t)Titanfall::Bsp::meshes.size() - model.firstMesh;

    for (Titanfall::MeshBounds_t &meshBounds : Titanfall::Bsp::meshBounds) {
        model.minmax.extend(meshBounds.origin - meshBounds.extents);
        model.minmax.extend(meshBounds.origin + meshBounds.extents);
    }

    Sys_FPrintf( SYS_VRB, "       numMeshes: %i\n", model.meshCount );
    Sys_FPrintf( SYS_VRB, "   EndModel\n" );
}

/*
    EmitEntityPartitions()
    Writes entitiy partitions respawn uses
*/
void Titanfall::EmitEntityPartitions() {
    std::string  partitions = "01*";

    if (Titanfall::Ent::env.size()) {
        partitions += " env";
    }

    if (Titanfall::Ent::fx.size()) {
        partitions += " fx";
    }

    if (Titanfall::Ent::script.size()) {
        partitions += " script";
    }

    if (Titanfall::Ent::snd.size()) {
        partitions += " snd";
    }

    if (Titanfall::Ent::spawn.size()) {
        partitions += " spawn";
    }

    Titanfall::Bsp::entityPartitions = { partitions.begin(), partitions.end() + 1 };
}
