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

#ifndef ELITE_H
#define ELITE_H

#include "planet.h"
#include "trade.h"


#pragma region Screens (SCR_XXX_XXXX)
#define SCR_INTRO_ONE       1
#define SCR_INTRO_TWO       2
#define SCR_GALACTIC_CHART  3
#define SCR_SHORT_RANGE     4
#define SCR_PLANET_DATA     5
#define SCR_MARKET_PRICES   6
#define SCR_CMDR_STATUS     7
#define SCR_FRONT_VIEW      8
#define SCR_REAR_VIEW       9
#define SCR_LEFT_VIEW       10
#define SCR_RIGHT_VIEW      11
#define SCR_BREAK_PATTERN   12
#define SCR_INVENTORY       13
#define SCR_EQUIP_SHIP      14
#define SCR_OPTIONS         15
#define SCR_LOAD_CMDR       16
#define SCR_SAVE_CMDR       17
#define SCR_QUIT            18
#define SCR_GAME_OVER       19
#define SCR_SETTINGS        20
#define SCR_ESCAPE_POD      21
#define SCR_MODIFY_SHIP     22
#pragma endregion


#define PULSE_LASER     0x0F
#define BEAM_LASER      0x8F
#define MILITARY_LASER  0x97
#define MINING_LASER    0x32


#define FLG_DEAD            (1)
#define FLG_REMOVE          (2)
#define FLG_EXPLOSION       (4)
#define FLG_ANGRY           (8)
#define FLG_FIRING          (16)
#define FLG_HAS_ECM         (32)
#define FLG_HOSTILE         (64)
#define FLG_CLOAKED         (128)
#define FLG_FLY_TO_PLANET   (256)
#define FLG_FLY_TO_STATION  (512)
#define FLG_INACTIVE        (1024)
#define FLG_SLOW            (2048)
#define FLG_BOLD            (4096)
#define FLG_POLICE          (8192)
#define FLG_SHOT            (16384)


#define VERSION_STRING      "Version: beta-1"
#define CREDIT_FUI          "further adventures from fuirippu 2016"
#define VERSION_MAJOR       0x00
#define VERSION_MINOR       0x00

/////////////////////////////////////////////////////////////////////////////
// struct definitions
/////////////////////////////////////////////////////////////////////////////
struct commander
{
    char name[32];
    int mission;
    int ship_x;
    int ship_y;
    struct galaxy_seed galaxy;
    int credits;
    int fuel;
    int unused1;
    int galaxy_number;
    int front_laser;
    int rear_laser;
    int left_laser;
    int right_laser;
    int unused2;
    int unused3;
    int cargo_capacity;
    int current_cargo[NO_OF_STOCK_ITEMS];
    int ecm;
    int fuel_scoop;
    int energy_bomb;
    int energy_unit;
    int docking_computer;
    int galactic_hyperdrive;
    int escape_pod;
    int version_major;
    int version_minor;
    int unused6;
    int unused7;
    int missiles;
    int legal_status;
    int station_stock[NO_OF_STOCK_ITEMS];
    int market_rnd;
    int score;
    int saved;
	int ship_mods;
};

#pragma region Ship mod flags
#define SHIP_MOD_AUDIO_SCANNER		(1)
#define SHIP_MOD_VGA_SCANNER		(2)
#define SHIP_MOD_OBC				(4)
#define SHIP_MOD_SPEEDO				(8)
#define SHIP_MOD_MILO				(16)
#pragma endregion

struct player_ship
{
    int max_speed;
    int max_roll;
    int max_climb;
    int max_fuel;
    int altitude;
    int cabtemp;
};


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
extern struct player_ship myship;

extern struct commander cmdr;
extern struct commander saved_cmdr;

extern struct galaxy_seed docked_planet;
extern struct galaxy_seed hyperspace_planet;

extern struct planet_data current_planet_data;

extern int carry_flag;
extern int current_screen;
extern int break_pattern_base_colour;

extern struct ship_data *ship_list[];

extern int directx;         /// 1 == Use directx (full-screen on linux)
                            /// 0 == Use GDI (windowed on linux)
extern int aspect_16_y;     /// In full-screen/dx, screen ratio is 16:Y
extern int x360_controller; /// 1 == yes, 0 == no

extern int wireframe;
extern int anti_alias_gfx;
extern int hoopy_casinos;
extern int instant_dock;
extern int speed_cap;

extern const int scanner_cx;
extern const int scanner_cy;
extern const int compass_centre_x;
extern const int compass_centre_y;

extern int planet_render_style;

extern int game_over;
extern int docked;
extern int flight_speed;
extern int flight_roll;
extern int flight_climb;
extern int front_shield;
extern int aft_shield;
extern int energy;
extern int laser_temp;
extern int mcount;
extern int detonate_bomb;
extern int witchspace;
extern int auto_pilot;


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void restore_saved_commander(void);


#ifdef _DEBUG
void dbg_dump_universe();
void dbg_out(const char *str);
#endif // _DEBUG


#endif      // #ifndef ELITE_H
