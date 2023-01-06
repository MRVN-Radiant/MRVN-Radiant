/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r2 ( Titanfall 2 ) .bsp file.

   A lot of this is temporary and will be altered heavily with new information.
   If you know how to c++ better than me feel free to contribute or call me words
   in discord. :)

   - Fifty#8113, also known as Fifteen, Sixteen, Seventeen, Eigtheen and FORTY

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "remap.h"
#include "bspfile_abstract.h"
#include <ctime>



/* funcs */
/*
   LoadR2BSPFile()
   loads a titanfall2 bsp file
*/
void LoadR2BSPFile(const char* filename) {
    Titanfall::LoadLumpsAndEntities( filename );
    Titanfall::ParseLoadedBSP();
}


/*
   WriteR2BSPFile()
   writes a titanfall2 bsp file and it's .ent files
*/
void WriteR2BSPFile(const char* filename) {
    rbspHeader_t header{};

    /* set up header */
    memcpy(header.ident, g_game->bspIdent, 4);
    header.version = LittleLong(g_game->bspVersion);
    header.mapVersion = 30;
    header.maxLump = 127;

    /* This should be automated maybe */
    header.lumps[0x01].lumpVer = 1;
    header.lumps[0x02].lumpVer = 1;
    header.lumps[0x36].lumpVer = 3;
    header.lumps[0x42].lumpVer = 2;
    header.lumps[0x44].lumpVer = 1;
    header.lumps[0x45].lumpVer = 1;
    header.lumps[0x49].lumpVer = 1;
    header.lumps[0x53].lumpVer = 1;

    /* write initial header */
    FILE* file = SafeOpenWrite(filename);
    SafeWrite(file, &header, sizeof(header));    /* overwritten later */

    /* :) */
    {
        char message[64] = "Built with love using MRVN-radiant :)";
        SafeWrite(file, &message, sizeof(message));
    }
    {
        char message[64];
        strncpy(message,StringOutputStream(64)("Version:        ", Q3MAP_VERSION).c_str(),64);
        SafeWrite(file, &message, sizeof(message));
    }
    {
        time_t t;
        time(&t);
        char message[64];
        strncpy(message,StringOutputStream(64)("Time:           ", asctime(localtime(&t))).c_str(),64);
        SafeWrite(file, &message, sizeof(message));
    }

    /* Write lumps */

    AddLump(file, header.lumps[R2_LUMP_ENTITIES],          Titanfall::Bsp::entities);
    AddLump(file, header.lumps[R2_LUMP_PLANES],            Titanfall::Bsp::planes);
    AddLump(file, header.lumps[R2_LUMP_TEXTURE_DATA],      Titanfall::Bsp::textureData);
    AddLump(file, header.lumps[R2_LUMP_VERTICES],          Titanfall::Bsp::vertices);
    AddLump(file, header.lumps[R2_LUMP_MODELS],            Titanfall::Bsp::models);
    AddLump(file, header.lumps[R2_LUMP_VERTEX_NORMALS],    Titanfall::Bsp::vertexNormals);
    AddLump(file, header.lumps[R2_LUMP_ENTITY_PARTITIONS], Titanfall::Bsp::entityPartitions);
    /* Game Lump */
    {
        std::size_t start = ftell(file);
        header.lumps[R2_LUMP_GAME_LUMP].offset = start;
        header.lumps[R2_LUMP_GAME_LUMP].length = 36
                                               + Titanfall2::GameLump.pathCount * sizeof(Titanfall2::GameLump_Path)
                                               + Titanfall2::GameLump.propCount * sizeof(Titanfall2::GameLump_Prop);
        Titanfall2::GameLump.offset = start + 20;
        Titanfall2::GameLump.length = 16
                                    + Titanfall2::GameLump.pathCount * sizeof(Titanfall2::GameLump_Path)
                                    + Titanfall2::GameLump.propCount * sizeof(Titanfall2::GameLump_Prop);
        SafeWrite(file, &Titanfall2::GameLump, sizeof(Titanfall2::GameLump));
        /* need to write vectors separately */
        /* paths */
        fseek(file, start + 24, SEEK_SET);
        SafeWrite(file, Titanfall2::GameLump.paths.data(), 128 * Titanfall2::GameLump.pathCount);
        /* :) */
        SafeWrite(file, &Titanfall2::GameLump.propCount, 4);
        SafeWrite(file, &Titanfall2::GameLump.propCount, 4);
        SafeWrite(file, &Titanfall2::GameLump.propCount, 4);
        /* props */
        SafeWrite(file, Titanfall2::GameLump.props.data(), 64 * Titanfall2::GameLump.propCount);
        SafeWrite(file, &Titanfall2::GameLump.unk5, 4);
    }
    AddLump(file, header.lumps[R2_LUMP_TEXTURE_DATA_STRING_DATA],  Titanfall::Bsp::textureDataData);
    AddLump(file, header.lumps[R2_LUMP_TEXTURE_DATA_STRING_TABLE], Titanfall::Bsp::textureDataTable);
    AddLump(file, header.lumps[R2_LUMP_WORLD_LIGHTS],              Titanfall2::Bsp::worldLights_stub);  // stub
    // AddLump(file, header.lumps[R2_LUMP_TRICOLL_TRIS],              Titanfall2::bspTricollTris_stub);  // stub
    // AddLump(file, header.lumps[R2_LUMP_TRICOLL_NODES],             Titanfall2::bspTricollNodes_stub);  // stub
    // AddLump(file, header.lumps[R2_LUMP_TRICOLL_HEADERS],           Titanfall2::bspTricollHeaders_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_VERTEX_UNLIT],              Titanfall::Bsp::vertexUnlitVertices);
    AddLump(file, header.lumps[R2_LUMP_VERTEX_LIT_FLAT],           Titanfall::Bsp::vertexLitFlatVertices);
    AddLump(file, header.lumps[R2_LUMP_VERTEX_LIT_BUMP],           Titanfall::Bsp::vertexLitBumpVertices);
    AddLump(file, header.lumps[R2_LUMP_VERTEX_UNLIT_TS],           Titanfall::Bsp::vertexUnlitTSVertices);
    AddLump(file, header.lumps[R2_LUMP_VERTEX_BLINN_PHONG],        Titanfall::Bsp::vertexBlinnPhongVertices);
    AddLump(file, header.lumps[R2_LUMP_MESH_INDICES],              Titanfall::Bsp::meshIndices);
    AddLump(file, header.lumps[R2_LUMP_MESHES],                    Titanfall::Bsp::meshes);
    AddLump(file, header.lumps[R2_LUMP_MESH_BOUNDS],               Titanfall::Bsp::meshBounds);
    AddLump(file, header.lumps[R2_LUMP_MATERIAL_SORT],             Titanfall::Bsp::materialSorts);
    AddLump(file, header.lumps[R2_LUMP_LIGHTMAP_HEADERS],          Titanfall2::Bsp::lightMapHeaders_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CM_GRID],                   Titanfall::Bsp::cmGrid);
    AddLump(file, header.lumps[R2_LUMP_CM_GRID_CELLS],             Titanfall::Bsp::cmGridCells);
    AddLump(file, header.lumps[R2_LUMP_CM_GEO_SETS],              Titanfall::Bsp::cmGeoSets);
    AddLump(file, header.lumps[R2_LUMP_CM_GEO_SET_BOUNDS],         Titanfall::Bsp::cmGeoSetBounds);
    // AddLump(file, header.lumps[R2_LUMP_CM_PRIMITIVES],             Titanfall::Bsp::cmPrimitives_stub);  // stub
    // AddLump(file, header.lumps[R2_LUMP_CM_PRIMITIVE_BOUNDS],       Titanfall::Bsp::cmPrimitiveBounds_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CM_UNIQUE_CONTENTS],        Titanfall::Bsp::cmUniqueContents_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CM_BRUSHES],                Titanfall::Bsp::cmBrushes);
    AddLump(file, header.lumps[R2_LUMP_CM_BRUSH_SIDE_PROPS],       Titanfall::Bsp::cmBrushSideProperties);
    AddLump(file, header.lumps[R2_LUMP_CM_BRUSH_SIDE_PLANES],      Titanfall::Bsp::cmBrushSidePlaneOffsets);
    // AddLump(file, header.lumps[R2_LUMP_TRICOLL_BEVEL_STARTS],      Titanfall2::bspTricollBevelStarts_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_LIGHTMAP_DATA_SKY],         Titanfall2::Bsp::lightMapDataSky_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CSM_AABB_NODES],            Titanfall::Bsp::csmAABBNodes_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CELL_BSP_NODES],            Titanfall::Bsp::cellBSPNodes_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CELLS],                     Titanfall::Bsp::cells_stub);  // stub
    AddLump(file, header.lumps[R2_LUMP_CELL_AABB_NODES],           Titanfall::Bsp::cellAABBNodes);
    AddLump(file, header.lumps[R2_LUMP_OBJ_REFERENCES],            Titanfall::Bsp::objReferences);
    AddLump(file, header.lumps[R2_LUMP_OBJ_REFERENCE_BOUNDS],      Titanfall::Bsp::objReferenceBounds);
    AddLump(file, header.lumps[R2_LUMP_LEVEL_INFO],                Titanfall::Bsp::levelInfo);

    /* emit bsp size */
    const int size = ftell(file);
    Sys_Printf("Wrote %.1f MB (%d bytes)\n", (float)size / (1024 * 1024), size);

    /* write the completed header */
    fseek(file, 0, SEEK_SET);
    SafeWrite(file, &header, sizeof(header));

    /* close the file */
    fclose(file);
}


/*
   CompileR2BSPFile()
   writes a titanfall2 bsp file and it's .ent files
 */
void CompileR2BSPFile()
{
	SetUpGameLump();

	for (size_t entityNum = 0; entityNum < entities.size(); ++entityNum)
	{
		/* get entity */
		entity_t& entity = entities[entityNum];
		const char* classname = entity.classname();

		/* visible geo */
		if ( striEqual( classname,"worldspawn" ) )
		{
			Titanfall::BeginModel();
			/* generate bsp meshes from map brushes */
			Shared::MakeMeshes( entity );
			Titanfall::EmitMeshes( entity );
			
			Titanfall::EmitBrushes( entity );

			Titanfall::EndModel();
		}
		/* hurt */
		else if ( striEqualPrefix( classname, "trigger_" ) )
		{
			Titanfall::EmitTriggerBrushPlaneKeyValues( entity );
		}
		/* props for gamelump */
		else if ( striEqual( classname, "misc_model" ) )
		{
			EmitProp( entity );
		}


		Titanfall::EmitEntity( entity );
	}

	/* */
	Titanfall::EmitEntityPartitions();

	Titanfall::EmitCollisionGrid();

	/**/
	Shared::MakeVisReferences();
	Shared::visRoot = Shared::MakeVisTree( Shared::visRefs, 1e30f );
	Shared::MergeVisTree(Shared::visRoot);
	Titanfall::EmitVisTree();

	/* Emit LevelInfo */
	Titanfall::EmitLevelInfo();

	/* Generate unknown lumps */
	Titanfall2::EmitStubs();
	Titanfall::EmitStubs();
}


/*
   EmitEntity()
   Saves an entity into it's corresponding .ent file or the lump in the .bsp
*/
void Titanfall2::EmitEntity(const entity_t &e) {
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
    // NOTE: we test for script entities last, as it's the most complex & edge-case ridden
    // TODO: filter out editor only entities (e.g. prop_static, env_cubemap, info_lightprobe, light (static worldlights), etc.)
    // !!! and make sure those entities still get used for the appropriate lumps !!!
    // bsp
    if (ENT_IS("env_soundscape_triggerable")
      ||ENT_IS("env_wind")
      ||ENT_IS("envmap_volume")
      ||ENT_IS("fog_volume")
      ||ENT_IS("func_wallrun_curve")
      ||ENT_IS("func_window_hint")
      ||ENT_IS("info_intermission")
      ||ENT_IS("info_lightprobe")
      ||ENT_IS("info_spawnpoint_flag")
      ||ENT_IS("info_teleport_destination")
      ||ENT_IS("prop_dynamic_clientside")
      ||ENT_IS("prop_exfil_panel")
      ||ENT_IS("trigger_indoor_area")
      ||ENT_IS("trigger_no_grapple")
      ||ENT_IS("trigger_out_of_bounds")
      ||ENT_IS("water_lod_control")
      ||ENT_IS("worldspawn")) {
        dest = ENT_BSP;
        /* editorclass conversion */
        EDITORCLASS("prop_dynamic", "prop_dynamic_clientside")
    }
    // env
    else if (ENT_IS("color_correction")
     || ENT_IS("env_fog_controller")
     || ENT_STARTS("light")
     || ENT_IS("sky_camera")
     || ENT_IS("stream_hint_point")) {
        dest = ENT_ENV;
    // fx
    } else if (ENT_IS("beam_spotlight")
            || ENT_STARTS("info_target_fx")
            || ENT_STARTS("info_particle_system")) {
        dest = ENT_FX;
        /* editorclass conversion */
        EDITORCLASS("info_particle_system", "info_particle_system_clientside")
        else EDITORCLASS("info_target", "info_target_fx")
        else EDITORCLASS("info_target_clientside", "info_target_fx_clientside")
    // snd
    } else if (ENT_IS("ambient_generic")) {
        dest = ENT_SND;
    // spawn
    } else if (ENT_IS("info_frontline")
            || (ENT_STARTS("info_spawnpoint_") && !ENT_IS("info_spawnpoint_checkpoint"))) {
        dest = ENT_SPAWN;
        /* editorclass conversion */
        EDITORCLASS("info_spawnpoint_marvin", "info_spawnpoint_megaturret")
    // script
    #define EC_it(x)  else EDITORCLASS("info_target", "info_" #x)
    } else if (ENT_STARTS("info_")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        EDITORCLASS("info_node", "info_node_spectre")
        EC_it("airdrop_event")
        EC_it("attrition_bank")
        EC_it("attrition_camp")
        EC_it("bomb_mode_base")
        EC_it("bomb_mode_bomb")
        EC_it("bomb_mode_pilot_defuse_point")
        EC_it("dropship_deploy_position")
        EC_it("fd_ai_position")
        EC_it("fd_battery_port")
        EC_it("fd_harvester")
        EC_it("fd_mode_model")
        EC_it("fd_route_node")
        EC_it("fd_smoke_screen")
        EC_it("fd_turret_site")
        EC_it("fw_camp")
        EC_it("fw_team_tower")
        EC_it("fw_turret_site")
        EC_it("lts_bomb_site")
        EC_it("move_animation")
        EC_it("move_target")
        EC_it("pet_titan_start")
        EC_it("potential_threat_target")
        EC_it("spawnpoint_checkpoint")
        EC_it("startpoint")
        #define EC_sr_i(x)  else EDITORCLASS("script_ref", "info_" #x)
        EC_sr_i("atcoop_weapon_chest")
        EC_sr_i("don_panel")
        EC_sr_i("fw_battery_port")
        EC_sr_i("fw_foundation_plate")
        EC_sr_i("fw_sonar_site")
        EC_sr_i("fw_titan_barricade")
        EC_sr_i("pve_guard_spawn")
        EC_sr_i("pve_patrol_spawn")
        EC_sr_i("speedball_flag")
        #undef ED_sr_i
    // script
    } else if (ENT_STARTS("mp_")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        #define EC_it_mp(x)  else EDITORCLASS("info_target", "mp_" #x)
        EDITORCLASS("info_target", "mp_titanweapon_leadwall")
        EC_it_mp("titanweapon_meteor")
        EC_it_mp("titanweapon_particle_accelerator")
        EC_it_mp("titanweapon_rocketeer_rocketstream")
        EC_it_mp("titanweapon_sniper")
        EC_it_mp("titanweapon_sticky_40mm")
        else if (ENT_STARTS("mp_weapon_")) {
            EDITORCLASS("info_target", "mp_weapon_alternator_smg")
            EC_it_mp("weapon_arc_launcher")
            EC_it_mp("weapon_autopistol")
            EC_it_mp("weapon_car")
            EC_it_mp("weapon_defender")
            EC_it_mp("weapon_dmr")
            EC_it_mp("weapon_doubletake")
            EC_it_mp("weapon_epg")
            EC_it_mp("weapon_esaw")
            EC_it_mp("weapon_frag_drone")
            EC_it_mp("weapon_frag_grenade")
            EC_it_mp("weapon_g2")
            EC_it_mp("weapon_grenade_electric_smoke")
            EC_it_mp("weapon_grenade_emp")
            EC_it_mp("weapon_grenade_gravity")
            EC_it_mp("weapon_hemlok")
            EC_it_mp("weapon_hemlok_smg")
            EC_it_mp("weapon_lmg")
            EC_it_mp("weapon_lstar")
            EC_it_mp("weapon_mastiff")
            EC_it_mp("weapon_mgl")
            EC_it_mp("weapon_pulse_lmg")
            EC_it_mp("weapon_r97")
            EC_it_mp("weapon_rocket_launcher")
            EC_it_mp("weapon_rspn101")
            EC_it_mp("weapon_satchel")
            EC_it_mp("weapon_semipistol")
            EC_it_mp("weapon_shotgun")
            EC_it_mp("weapon_shotgun_pistol")
            EC_it_mp("weapon_smr")
            EC_it_mp("weapon_sniper")
            EC_it_mp("weapon_softball")
            EC_it_mp("weapon_thermite_grenade")
            EC_it_mp("weapon_vinson")
            EC_it_mp("weapon_wingman")
            #undef EC_it_mp
        }
    // script
    } else if (ENT_STARTS("npc_")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        #define EC_npc(x, y)  else EDITORCLASS("npc_" x, "npc_" x y)
        EDITORCLASS("npc_drone", "npc_mrvn_drone")
        EC_npc("dropship", "soldiers")
        else EDITORCLASS("npc_soldier", "npc_shield_captain_imc")
        else if (ENT_STARTS("npc_soldier_")) {
            EDITORCLASS("npc_soldier", "npc_soldier_hero_sarah")
            EC_npc("soldier", "imc_antititan")
            EC_npc("soldier", "imc_lmg")
            EC_npc("soldier", "imc_rifle")
            EC_npc("soldier", "imc_shotgun")
            EC_npc("soldier", "imc_sidearm")
            EC_npc("soldier", "imc_smg")
            EC_npc("soldier", "imc_sniper")
            EC_npc("soldier", "militia")
        } else EDITORCLASS("npc_soldier", "npc_specialist_imc")
        else if (ENT_STARTS("npc_spectre_")) {
            EDITORCLASS("npc_spectre", "npc_spectre_antititan")
            EC_npc("spectre", "lmg")
            EC_npc("spectre", "rifle")
            EC_npc("spectre", "shotgun")
            EC_npc("spectre", "smg")
            EC_npc("spectre", "sniper")
        } else if (ENT_STARTS("npc_stalker_")) {
            EDITORCLASS("npc_stalker", "npc_stalker_crawling")
            EC_npc("stalker", "crawling_mossy")
            EC_npc("stalker", "zombie")
            EC_npc("stalker", "zombie_mossy")
        } else if (ENT_STARTS("npc_titan_")) {
            EDITORCLASS("npc_titan", "npc_titan_atlas_stickybomb")
            EC_npc("titan", "atlas_tracker")
            EC_npc("titan", "hero_sarah")
            EC_npc("titan", "ogre_meteor")
            EC_npc("titan", "ogre_minigun")
            EC_npc("titan", "stryder_leadwall")
            EC_npc("titan", "stryder_rocketeer")
            EC_npc("titan", "stryder_sniper")
            EC_npc("titan", "vanguard")
        }
        EC_npc("turret_sentry", "plasma")
        #undef EC_npc
        else EDITORCLASS("prop_dynamic", "npc_spectre_rack_wall")
    // script
    } else if (ENT_STARTS("script_")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        EDITORCLASS("prop_dynamic", "script_crane")
        EC_it("script_crane_cargo")
        EC_it("script_drone_platform_despawn")
        EC_it("script_drone_platform_spawn")
        EC_it("script_mover_path")
        #undef EC_it
        else EDITORCLASS("prop_dynamic", "script_door")
        else EDITORCLASS("prop_dynamic", "script_drone_platform_large")
        else EDITORCLASS("prop_dynamic", "script_switch")
        else EDITORCLASS("script_marvin_job", "script_marvin_drone_job")
        else EDITORCLASS("script_mover_lightweight", "script_collectible")
        else EDITORCLASS("script_mover_lightweight", "script_rotator")
        else EDITORCLASS("script_mover_lightweight", "script_soundeffect")
        #define EC_sr_s(x)  else EDITORCLASS("script_ref", "script_" x)
        EC_sr_s("battery_generator")
        EC_sr_s("bird_alert")
        EC_sr_s("combatpath_enemy_reinforce")
        EC_sr_s("combatpath_friendlygoal")
        EC_sr_s("combatpath_spawnposition_imc")
        EC_sr_s("combatpath_spawnposition_mil")
        EC_sr_s("frontier_zone")
        EC_sr_s("pickup_ammo")
        EC_sr_s("pickup_health")
        EC_sr_s("pickup_health_large")
        EC_sr_s("pickup_weapon")
        EC_sr_s("power_up_other")
        EC_sr_s("titan_battery")
        #undef EC_sr_s
    // script
    } else if (ENT_STARTS("trigger_")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        #define EC_tm(x)  else EDITORCLASS("trigger_multiple", "trigger_" x)
        EDITORCLASS("trigger_multiple", "trigger_checkpoint")
        EC_tm("checkpoint_forced")
        EC_tm("checkpoint_safe")
        EC_tm("checkpoint_silent")
        EC_tm("checkpoint_to_safe_spots")
        EC_tm("deadly_fog")
        EC_tm("death_fall")
        EC_tm("flag_clear")
        EC_tm("flag_set")
        EC_tm("flag_touching")
        EC_tm("friendly")
        EC_tm("friendly_follow")
        EC_tm("fw_territory")
        EC_tm("level_transition")
        EC_tm("mp_spawn_zone")
        EC_tm("quickdeath")
        EC_tm("quickdeath_checkpoint")
        EC_tm("spawn")
        EC_tm("teleporter")
        #undef EC_tm
    // script
    } else if (ENT_IS("assault_assaultpoint")
            || ENT_STARTS("func_brush")
            || ENT_IS("move_rope")
            || ENT_IS("path_track")
            || ENT_STARTS("prop_")
            || ENT_IS("traverse")
            || ENT_IS("toy_arc_switch")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        EDITORCLASS("func_brush", "func_brush_fw_territory_border")
        else EDITORCLASS("func_brush", "func_brush_navmesh_separator")
        else EDITORCLASS("prop_script", "toy_arc_switch")
        else EDITORCLASS("trigger_multiple", "checkpoint_disabled_volume")
    } else {
        dest = ENT_BSP;  // anything we missed should still ship
    }
    #undef ENT_IS
    #undef ENT_STARTS
    #undef EDITORCLASS

    // TODO: general entity edits (e.g. float angle (Yaw) -> float[3] angles (Pitch Yaw Roll))

    // write
    StringOutputStream data;
    data << "{\n";
    for (const epair_t& pair : e2.epairs) {  // e2 to get editorclass changes
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
    EmitStubs
    Fills out all the nessesary lumps we dont generate so the map at least boots and we can noclip around
*/
void Titanfall2::EmitStubs() {
    {  // World Lights
        constexpr std::array<uint8_t, 1008> data = {
            0x00, 0x00, 0xC0, 0x42, 0x00, 0x00, 0x10, 0x42, 0x00, 0x80, 0x82, 0x43, 0xCE, 0xCC, 0x4C, 0x3E,
            0xCE, 0xCC, 0x4C, 0x3E, 0xCE, 0xCC, 0x4C, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xDF, 0x00, 0xC0, 0xC2, 0x99, 0xFB, 0x0F, 0xC2, 0x00, 0x80, 0x82, 0x43, 0xCE, 0xCC, 0x4C, 0x3E,
            0xCE, 0xCC, 0x4C, 0x3E, 0xCE, 0xCC, 0x4C, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xC0, 0x42, 0x00, 0x00, 0x10, 0x42, 0x00, 0x80, 0x82, 0x43, 0x00, 0x00, 0x80, 0x40,
            0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x40, 0x1D, 0xBF, 0xAB, 0xBE, 0x92, 0x8B, 0x06, 0x3F,
            0xFC, 0x25, 0x48, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xDF, 0x00, 0xC0, 0xC2, 0x99, 0xFB, 0x0F, 0xC2, 0x00, 0x80, 0x82, 0x43, 0x00, 0x00, 0x80, 0x40,
            0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x40, 0xC7, 0xEB, 0x8B, 0x3E, 0xEB, 0xD3, 0x05, 0xBF,
            0x62, 0xB7, 0x4E, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0xC2, 0x00, 0x00, 0x60, 0xC1, 0x00, 0x00, 0xC2, 0x42, 0x3A, 0x19, 0x00, 0x46,
            0x4B, 0x16, 0xC3, 0x46, 0xF6, 0x55, 0x08, 0x47, 0x22, 0xD1, 0x18, 0x3E, 0x41, 0x84, 0x52, 0xBF,
            0x41, 0x91, 0x0C, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x8F, 0x70, 0x3F,
            0x43, 0x1D, 0xAF, 0x3E, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x80, 0x3F,
            0x65, 0xF0, 0x1F, 0xBC, 0x8F, 0xBA, 0xC7, 0x37, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F,
            0x00, 0x00, 0x18, 0xC2, 0x00, 0x00, 0x85, 0x43, 0x00, 0x00, 0x8E, 0x43, 0xA5, 0x7B, 0x20, 0x47,
            0x27, 0x68, 0xF4, 0x47, 0x7E, 0xCD, 0x2A, 0x48, 0x26, 0x86, 0x6A, 0x3E, 0xD4, 0x96, 0x2B, 0xBF,
            0x60, 0xB5, 0x34, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x8F, 0x70, 0x3F,
            0x43, 0x1D, 0xAF, 0x3E, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0xAF, 0x43, 0x00, 0x00, 0x80, 0x3F,
            0x98, 0xC9, 0xB6, 0xBB, 0x63, 0x6F, 0x02, 0x37, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F,
            0x00, 0x00, 0x2A, 0x43, 0x00, 0x00, 0x56, 0x43, 0x00, 0x00, 0xB4, 0xC2, 0x34, 0x9D, 0xA4, 0x3E,
            0xC6, 0x6B, 0xE7, 0x3E, 0x00, 0xFF, 0xFE, 0x3E, 0x92, 0x2A, 0xB6, 0xBE, 0x60, 0x46, 0x13, 0xBF,
            0x07, 0x8C, 0x3C, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x8F, 0x70, 0x3F,
            0x43, 0x1D, 0xAF, 0x3E, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0xC8, 0x43, 0x00, 0x00, 0x80, 0x3F,
            0x54, 0xA9, 0xF9, 0xB8, 0x8F, 0xBA, 0xC7, 0xB6, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F,
            0x00, 0x00, 0x98, 0xC1, 0x00, 0x00, 0xD8, 0x41, 0x00, 0x00, 0x98, 0xC1, 0x01, 0x40, 0x1C, 0x47,
            0x3C, 0x95, 0xDF, 0x46, 0x06, 0x81, 0x88, 0x46, 0x64, 0xF5, 0x1F, 0x3F, 0xD0, 0x05, 0xFD, 0x3E,
            0x2B, 0xBD, 0x1A, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x1B, 0x44, 0x3F,
            0xBA, 0x8D, 0x24, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x96, 0x43, 0x00, 0x00, 0x80, 0x3F,
            0xE3, 0x70, 0x26, 0xB9, 0x63, 0x89, 0x31, 0xB7, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F,
            0x00, 0x00, 0x92, 0xC2, 0x00, 0x00, 0x4C, 0xC3, 0x00, 0x00, 0x80, 0x41, 0x01, 0x50, 0x43, 0x48,
            0x05, 0xD1, 0x04, 0x48, 0x47, 0xA1, 0xAA, 0x47, 0xEE, 0x1C, 0x0B, 0x3F, 0xD0, 0xB3, 0x32, 0x3F,
            0x14, 0xBD, 0xEE, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x1B, 0x44, 0x3F,
            0xBA, 0x8D, 0x24, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x5E, 0x43, 0x00, 0x00, 0x80, 0x3F,
            0x94, 0xEB, 0x60, 0xB9, 0xB5, 0x1A, 0xA2, 0xB7, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F
        };
        Titanfall2::Bsp::worldLights_stub = { data.begin(), data.end() };
    }
    {  // LightMap Headers
        constexpr std::array<uint8_t, 8> data = {
            0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00
        };
        Titanfall2::Bsp::lightMapHeaders_stub = { data.begin(), data.end() };
    }
    {  // LightMap Data Sky
        for (std::size_t i = 0; i < 524288; i++) {
            Titanfall2::Bsp::lightMapDataSky_stub.emplace_back(0);
        }
    }
}
