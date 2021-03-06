/*
 * Elite - The New Kind.
 *
 * Reverse engineered from the BBC disk version of Elite.
 * Additional material by C.J.Pinder.
 *
 * The original Elite code is (C) I.Bell & D.Braben 1984.
 * This version re-engineered in C by C.J.Pinder 1999-2001.
 *
 * email: <christian@newkind.co.uk>
 *
 *
 */

#include <stdio.h>

#ifdef WINDOWS
#include <Windows.h>
#endif // WINDOWS

#include "elite.h"
#include "vector.h"
#include "planet.h"
#include "space.h"
#include "shipdata.h"
#include "obcomp.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
struct galaxy_seed docked_planet;

struct galaxy_seed hyperspace_planet;

struct planet_data current_planet_data;

int carry_flag = 0;
int current_screen = 0;
int break_pattern_base_colour;
int witchspace;

int directx = 1;
int aspect_16_y = 9;
int x360_controller = 0;
int wireframe = 0;
int anti_alias_gfx = 0;
int hoopy_casinos = 0;
int speed_cap = 75;
int instant_dock = 0;
int compass_target = 1;

const int scanner_cx = 253;
const int scanner_cy = 448;
const int compass_centre_x = 382;
const int compass_centre_y = 407;

int planet_render_style = 0;

int game_over;
int docked;
int flight_speed;
int flight_roll;
int flight_climb;
int front_shield;
int aft_shield;
int energy;
int laser_temp;
int detonate_bomb;
int auto_pilot;


struct commander saved_cmdr =
{
    "JAMESON",                                  /* Name             */
    0,                                          /* Mission Number   */
    0x14,0xAD,                                  /* Ship X,Y         */
    {0x4a, 0x5a, 0x48, 0x02, 0x53, 0xb7},       /* Galaxy Seed      */
    1000,                                       /* Credits * 10     */
    70,                                         /* Fuel * 10        */
    0,
    0,                                          /* Galaxy - 1       */
    PULSE_LASER,                                /* Front Laser      */
    0,                                          /* Rear Laser       */
    0,                                          /* Left Laser       */
    0,                                          /* Right Laser      */
    0, 0,
    20,                                         /* Cargo Capacity   */
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},        /* Current Cargo    */
    0,                                          /* ECM              */
    0,                                          /* Fuel Scoop       */
    0,                                          /* Energy Bomb      */
    0,                                          /* Energy Unit      */
    0,                                          /* Docking Computer */
    0,                                          /* Galactic H'Drive */
    0,                                          /* Escape Pod       */
    0,0,0,0,
    3,                                          /* No. of Missiles  */
    0,                                          /* Legal Status     */
    {0x10, 0x0F, 0x11, 0x00, 0x03, 0x1C,        /* Station Stock    */
     0x0E, 0x00, 0x00, 0x0A, 0x00, 0x11,
     0x3A, 0x07, 0x09, 0x08, 0x00},
    0,                                          /* Fluctuation      */
    0,                                          /* Score            */
    0x80,                                       /* Saved            */
    0                                           /* Ship mods        */
};

struct commander cmdr;

struct player_ship myship;


struct ship_data *ship_list[NO_OF_SHIPS + 1] =
{
    NULL,
    &missile_data,
    &coriolis_data,
    &esccaps_data,
    &alloy_data,
    &cargo_data,
    &boulder_data,
    &asteroid_data,
    &rock_data,
    &orbit_data,
    &transp_data,
    &cobra3a_data,
    &pythona_data,
    &boa_data,
    &anacnda_data,
    &hermit_data,
    &viper_data,
    &sidewnd_data,
    &mamba_data,
    &krait_data,
    &adder_data,
    &gecko_data,
    &cobra1_data,
    &worm_data,
    &cobra3b_data,
    &asp2_data,
    &pythonb_data,
    &ferdlce_data,
    &moray_data,
    &thargoid_data,
    &thargon_data,
    &constrct_data,
    &cougar_data,
    &dodec_data
};



/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void restore_saved_commander(void)
{
    cmdr = saved_cmdr;

    docked_planet = find_planet(cmdr.ship_x, cmdr.ship_y);
    hyperspace_planet = docked_planet;

    generate_planet_data(&current_planet_data, docked_planet);
    generate_stock_market();
    set_stock_quantities(cmdr.station_stock);
}



#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////

void dbg_out(const char *str)
{
#ifdef WINDOWS
    OutputDebugString(str);
#else // not WINDOWS
    printf("%s", str);
    fflush(stdout);
#endif // WINDOWS
}

static const char *econ[] = {
    "RchInd", "AvgInd", "PorInd", "MnlInd",
    "MnlAgr", "RchAgr", "AvgAgr", "PorAgr" };
static const char *govt[] = {
    "Anrchy", "Feudal", "MultiG", "Dctatr",
    "Cmmnst", "Confed", "Dmcrcy", "CorpSt" };
void dbg_dump_universe()
{
    char buf[128];
    for (int i = 0; i < MAX_UNIV_OBJECTS; i++)
    {
        if (universe[i].type != 0)
        {
            sprintf(buf, "[%02d] %-13s @ <% 11.3f, % 11.3f, % 11.3f> d=%d\n",
                            i, obc_ship_name(universe[i].type),
                            universe[i].location.x, universe[i].location.y, universe[i].location.z,
                            universe[i].distance);
            dbg_out(buf);
        }
    }
    //sprintf(buf, "    Fuel = %d of 64 (max %dly)\n", (cmdr.fuel * 64) / myship.max_fuel, myship.max_fuel);
    //dbg_out(buf);
    //sprintf(buf, "     Alt = %d (x100km minimum alt)\n", myship.altitude / 4);
    //dbg_out(buf);
    //sprintf(buf, "     legality = 0x%04X [%d]\n", cmdr.legal_status, cmdr.legal_status);
    //dbg_out(buf);
    //sprintf(buf, "     NRG unit = %d\n", cmdr.energy_unit);
    //dbg_out(buf);
    sprintf(buf, "  ^Cmdr \"%s\" %d.%d Cr, score = 0x%04X [%d] (mission: %d)\n",
                    cmdr.name, (cmdr.credits / 10), (cmdr.credits % 10),
                    cmdr.score, cmdr.score, cmdr.mission);
    dbg_out(buf);

    sprintf(buf, "   galaxy %d{0x%02X.%02X.%02X.%02X.%02X.%02X} p{%02X.%02X.%02X.%02X.%02X.%02X}\n",
                    cmdr.galaxy_number + 1, cmdr.galaxy.a, cmdr.galaxy.b, cmdr.galaxy.c,
                    cmdr.galaxy.d, cmdr.galaxy.e, cmdr.galaxy.f,
                    docked_planet.a, docked_planet.b, docked_planet.c,
                    docked_planet.d, docked_planet.e, docked_planet.f);
    dbg_out(buf);

    char strPlanetName[16];
    name_planet(strPlanetName, docked_planet);
    capitalise_name(strPlanetName);
    struct planet_data p;
    generate_planet_data(&p, docked_planet);

    sprintf(buf, "     near  %s|%s|tech%02d  \"%s\"\n\n",
                    econ[p.economy], govt[p.government], p.techlevel + 1, strPlanetName);
    dbg_out(buf);
}

/////////////////////////////////////////////////////////////////////////////
#endif // _DEBUG
