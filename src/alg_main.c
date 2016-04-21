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

/*
 * alg_main.c - main game handler
 */


#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#include "gamelib.h"

#include "gfx.h"
#include "main.h"
#include "vector.h"
#include "elite.h"
#include "docked.h"
#include "intro.h"
#include "shipdata.h"
#include "shipface.h"
#include "space.h"
#include "sound.h"
#include "threed.h"
#include "swat.h"
#include "random.h"
#include "options.h"
#include "stars.h"
#include "missions.h"
#include "pilot.h"
#include "file.h"
#include "obcomp.h"
#include "assets.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
int mcount;

#pragma region statics
static int old_cross_x, old_cross_y;
static int cross_timer;


/// Lasers are paused for n frames per shot - see fire_laser() [swat.c]
static int laser_frames_left;

/// These variables are set every game loop
static char *view_title= NULL;		/// Set to string if flight view (eg. "Front View"), or NULL
static int laser_type = 0;			/// Set to type of laser mounted in current view, or 0

static int message_colour;			/// info_message() sets the colour, count (#frames to disaplay
static int message_count;			/// for) & copies the string; main game loop displays the text
static char message_string[80];


static int rolling;
static int climbing;
static int game_paused;
static int have_joystick = 0;

static int find_input;
static char find_name[20];

static int finish = 0;              /// flag is set to 1 if Y is pressed on the quit
									/// screen, the outer game loop is while (!finish)
#pragma endregion

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
static void initialise_game(void)
{
	set_rand_seed((int)(time(NULL)));
	current_screen = SCR_INTRO_ONE;

	restore_saved_commander();

	flight_speed = 1;
	flight_roll = 0;
	flight_climb = 0;
	docked = 1;
	front_shield = 255;
	aft_shield = 255;
	energy = 255;
	laser_frames_left = 0;
	mcount = 0;
	hyper_ready = 0;
	detonate_bomb = 0;
	find_input = 0;
	witchspace = 0;
	game_paused = 0;
	auto_pilot = 0;
	
	create_new_stars();
	clear_universe();
	
	cross_x = -1;
	cross_y = -1;
	cross_timer = 0;

	
	myship.max_speed = 40;		/* 0.27 Light Mach */
	myship.max_roll = 31;
	myship.max_climb = 8;		/* CF 8 */
	myship.max_fuel = 70;		/* 7.0 Light Years */
}

static void finish_game(void)
{
	finish = 1;
	game_over = 1;
}


static void move_cross(int dx, int dy)
{
	cross_timer = 5;

	if (current_screen == SCR_SHORT_RANGE)
	{
		cross_x += (dx * 4);
		cross_y += (dy * 4);
		return;
	}

	if (current_screen == SCR_GALACTIC_CHART)
	{
		cross_x += (dx * 2);
		cross_y += (dy * 2);

		if (cross_x < 1)
			cross_x = 1;
			
		if (cross_x > 510)
			cross_x = 510;

		if (cross_y < 37)
			cross_y = 37;
		
		if (cross_y > 293)
			cross_y = 293;
	}
}
static void draw_cross(int cx, int cy)
{
	if (current_screen == SCR_SHORT_RANGE)
	{
		gfx_set_clip_region(1, 37, 510, 339);
		gmlbGraphicsSetXorMode(-1);
		gfx_draw_colour_line(cx - 16, cy, cx + 16, cy, GFX_COL_RED);
		gfx_draw_colour_line(cx, cy - 16, cx, cy + 16, GFX_COL_RED);
		gmlbGraphicsSetXorMode(0);
		gfx_set_clip_region(1, 1, 510, 383);
		return;
	}
	
	if (current_screen == SCR_GALACTIC_CHART)
	{
		gfx_set_clip_region(1, 37, 510, 293);
		gmlbGraphicsSetXorMode(-1);
		gfx_draw_colour_line(cx - 8, cy, cx + 8, cy, GFX_COL_RED);
		gfx_draw_colour_line(cx, cy - 8, cx, cy + 8, GFX_COL_RED);
		gmlbGraphicsSetXorMode(0);
		gfx_set_clip_region(1, 1, 510, 383);
	}
}


#pragma region Lasers
static void draw_pulse_sights(int col1, int col2)
{
	int x1 = 128 * GFX_SCALE;
	int y1 = (96 - 8) * GFX_SCALE;
	int y2 = (96 - 16) * GFX_SCALE;

	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);

	y1 = (96 + 8) * GFX_SCALE;
	y2 = (96 + 16) * GFX_SCALE;

	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);

	x1 = (128 - 8) * GFX_SCALE;
	y1 = 96 * GFX_SCALE;
	int x2 = (128 - 16) * GFX_SCALE;

	gfx_draw_colour_line(x1, y1 - 1, x2, y1 - 1, col2);
	gfx_draw_colour_line(x1, y1, x2, y1, col1);
	gfx_draw_colour_line(x1, y1 + 1, x2, y1 + 1, col2);

	x1 = (128 + 8) * GFX_SCALE;
	x2 = (128 + 16) * GFX_SCALE;

	gfx_draw_colour_line(x1, y1 - 1, x2, y1 - 1, col2);
	gfx_draw_colour_line(x1, y1, x2, y1, col1);
	gfx_draw_colour_line(x1, y1 + 1, x2, y1 + 1, col2);
}

static void draw_beam_sights(int col1, int col2)
{
	int x1 = 128 * GFX_SCALE;
	int y1 = (96 - 10) * GFX_SCALE;
	int y2 = (96 - 18) * GFX_SCALE;

	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);

	y1 = (96 + 10) * GFX_SCALE;
	y2 = (96 + 18) * GFX_SCALE;

	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);

	x1 = (128 - 10) * GFX_SCALE;
	y1 = (96 - 6) * GFX_SCALE;
	y2 = (96 - 10) * GFX_SCALE;

	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);
	x1 = (128 + 10) * GFX_SCALE;
	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);

	x1 = (128 - 10) * GFX_SCALE;
	y1 = (96 + 10) * GFX_SCALE;
	y2 = (96 + 6) * GFX_SCALE;

	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);
	x1 = (128 + 10) * GFX_SCALE;
	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col2);
	gfx_draw_colour_line(x1, y1, x1, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col2);



	y1 = (96 + 10) * GFX_SCALE;
	x1 = (128 - 10) * GFX_SCALE;
	int x2 = (128 + 10) * GFX_SCALE;
	gfx_draw_colour_line(x1 + 1, y1 - 1, x2 - 1, y1 - 1, col2);
	gfx_draw_colour_line(x1, y1, x2, y1, col1);
	gfx_draw_colour_line(x1 + 1, y1 + 1, x2 - 1, y1 + 1, col2);
	y1 = (96 - 10) * GFX_SCALE;
	gfx_draw_colour_line(x1 + 1, y1 - 1, x2 - 1, y1 - 1, col2);
	gfx_draw_colour_line(x1, y1, x2, y1, col1);
	gfx_draw_colour_line(x1 + 1, y1 + 1, x2 - 1, y1 + 1, col2);
}

static void draw_military_sights(int col1, int col2, int offset)
{
	int x1, x2, y1, y2;

	y1 = (96 - 8 - offset) * GFX_SCALE;
	x1 = (128 - 4) * GFX_SCALE;
	x2 = (128 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x1, y1 - 1, x2, y1 - 1, col1);
	gfx_draw_colour_line(x1, y1 - 2, x2, y1 - 2, col2);
	x2 = 128 * GFX_SCALE;
	y2 = (96 - 4 - offset) * GFX_SCALE;
	gfx_draw_colour_line(x1, y1, x2, y2, col1);
	gfx_draw_colour_line(x1, y1 + 1, x2, y2 + 1, col2);
	x1 = (128 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x2, y2, x1, y1, col1);
	gfx_draw_colour_line(x2, y2 + 1, x1, y1 + 1, col2);

	y1 = (96 + 8 + offset) * GFX_SCALE;
	x1 = (128 - 4) * GFX_SCALE;
	x2 = (128 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x1, y1 + 1, x2, y1 + 1, col1);
	gfx_draw_colour_line(x1, y1 + 2, x2, y1 + 2, col2);
	x2 = 128 * GFX_SCALE;
	y2 = (96 + 4 + offset) * GFX_SCALE;
	gfx_draw_colour_line(x1, y1, x2, y2, col1);
	gfx_draw_colour_line(x1, y1 - 1, x2, y2 - 1, col2);
	x1 = (128 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x2, y2, x1, y1, col1);
	gfx_draw_colour_line(x2, y2 - 1, x1, y1 - 1, col2);



	x1 = (128 - 8 - offset) * GFX_SCALE;
	y1 = (96 - 4) * GFX_SCALE;
	y2 = (96 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x1 - 1, y1, x1 - 1, y2, col1);
	gfx_draw_colour_line(x1 - 2, y1, x1 - 2, y2, col2);
	x2 = (128 - 4 - offset) * GFX_SCALE;
	y2 = 96 * GFX_SCALE;
	gfx_draw_colour_line(x1, y1, x2, y2, col1);
	gfx_draw_colour_line(x1 + 1, y1, x2 + 1, y2, col2);
	y1 = (96 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x2, y2, x1, y1, col1);
	gfx_draw_colour_line(x2 + 1, y2, x1 + 1, y1, col2);

	x1 = (128 + 8 + offset) * GFX_SCALE;
	y1 = (96 - 4) * GFX_SCALE;
	y2 = (96 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x1 + 1, y1, x1 + 1, y2, col1);
	gfx_draw_colour_line(x1 + 2, y1, x1 + 2, y2, col2);
	x2 = (128 + 4 + offset) * GFX_SCALE;
	y2 = 96 * GFX_SCALE;
	gfx_draw_colour_line(x1, y1, x2, y2, col1);
	gfx_draw_colour_line(x1 - 1, y1, x2 - 1, y2, col2);
	y1 = (96 + 4) * GFX_SCALE;
	gfx_draw_colour_line(x2, y2, x1, y1, col1);
	gfx_draw_colour_line(x2 - 1, y2, x1 - 1, y1, col2);
}

static void draw_mining_sights()
{
	draw_pulse_sights(GFX_COL_PURPLE_1, GFX_COL_PURPLE_2);
	draw_beam_sights(GFX_COL_PURPLE_1, GFX_COL_PURPLE_2);

	//draw_military_sights(GFX_COL_PURPLE_1, GFX_COL_PURPLE_2, -1);
	draw_military_sights(GFX_COL_PURPLE_1, GFX_COL_PURPLE_2, 6);
	draw_military_sights(GFX_COL_PURPLE_1, GFX_COL_PURPLE_2, 10);
}

/// req(laser_type is a valid laser type, not 0 [elite.h])
/// Draws sights and shots as appropriate to type and laser_frames_left count
static void draw_lasers()
{
	int shot_colour = GFX_COL_WHITE;
	switch (laser_type)
	{
	case PULSE_LASER:
		draw_pulse_sights(GFX_COL_WHITE, GFX_COL_GREY_1);
		break;
	case BEAM_LASER:
		draw_beam_sights(GFX_COL_YELLOW_3, GFX_COL_GREY_1);
		shot_colour = GFX_COL_YELLOW_3;
		break;
	case MILITARY_LASER:
		draw_military_sights(GFX_COL_GREEN_3, GFX_COL_GREEN_1, 0);
		shot_colour = GFX_COL_RED_4;
		break;
	case MINING_LASER:
		draw_mining_sights();
		shot_colour = GFX_COL_MINT;
		break;
	}

	if (laser_frames_left)
	{
		draw_laser_shots(shot_colour);
		laser_frames_left--;
	}
}
#pragma endregion

/// requires view_title is a valid string, laser_type has been set or cleared
/// Writes "Xxxx View" centre top, displays obc, draws laser (sights and shots as appropriate to player)
static void draw_view_hud_and_lasers()
{
	gfx_display_centre_text(32, view_title, 120, GFX_COL_WHITE);

	if (cmdr.obc)
		obc_display();

	if (laser_type != 0)
		draw_lasers();
}


static void display_break_pattern(void)
{
	gfx_set_clip_region(1, 1, 510, 383);
	gfx_clear_display();

	//break_pattern_base_colour = GFX_COL_AA_0;			// boring... zzzz..	(hyperspace)
	//break_pattern_base_colour = GFX_COL_GREEN_3;		// psychedlic		(witch space)
	//break_pattern_base_colour = GFX_COL_BRK_00;		// Cycle :)			(dock/launch)

	int numCircles = 15;
	int step = 20;
	if (break_pattern_base_colour == GFX_COL_AA_0)
	{
		step = 15;				// longer duration pattern for hyperspace
		numCircles = 20;		// ...more and tighter cirles
	}

	int col = 0;
	int minR = 30;
	int maxR = 30;

	for (int i = 0; i < numCircles; i++)
	{
		maxR = 30 + (i * step);
		for (int r = maxR; r >= minR; r -= step)
		{
			gfx_draw_circle(256, 192, r + 0, break_pattern_base_colour + col);
			col = (col + 3) % 8;
			gfx_draw_circle(256, 192, r + 6, break_pattern_base_colour + col);
			col = (col + 3) % 8;
			gfx_draw_circle(256, 192, r + 12, break_pattern_base_colour + col);
			col = (col + 3) % 8;
		}
		gmlbUpdateScreen();
		gfx_clear_display();
	}
	for (int i = 0; i < numCircles; i++)
	{
		minR = 30 + (i * step);
		for (int r = minR; r <= maxR; r += step)
		{
			gfx_draw_circle(256, 192, r + 0, break_pattern_base_colour + col);
			col = (col + 3) % 8;
			gfx_draw_circle(256, 192, r + 6, break_pattern_base_colour + col);
			col = (col + 3) % 8;
			gfx_draw_circle(256, 192, r + 12, break_pattern_base_colour + col);
			col = (col + 3) % 8;
		}
		gmlbUpdateScreen();
		gfx_clear_display();
	}

	//// Original code. gfx_draw_circle() detects white circles and anti-aliases.
	////for (int i = 0; i < 20; i++)
	//for (int i = 19; i > -1; --i)
	//{
	//	gfx_draw_circle(256, 192, 30 + i * 15, GFX_COL_WHITE);
	//	gmlbUpdateScreen();
	//}

	if (docked)
	{
		check_mission_brief();
		display_commander_status();
		update_console();
	}
	else
		current_screen = SCR_FRONT_VIEW;
}


#pragma region Key press actions
static void arrow_right(void)
{
	switch (current_screen)
	{
		case SCR_GALACTIC_CHART:
		case SCR_SHORT_RANGE:
			move_cross(1, 0);
			break;

		case SCR_MARKET_PRICES:
			buy_stock();
			break;

		case SCR_SETTINGS:
			select_right_setting();		// fall through...
		case SCR_OPTIONS:
			return;						// ...don't roll (settings and options = paused)
	}
	if (!docked)
	{
		if (flight_roll > 0)
			flight_roll = 0;
		else
		{
			decrease_flight_roll();
			decrease_flight_roll();
			rolling = 1;
		}
	}
}

static void arrow_left(void)
{
	switch (current_screen)
	{
		case SCR_GALACTIC_CHART:
		case SCR_SHORT_RANGE:
			move_cross(-1, 0);
			break;

		case SCR_MARKET_PRICES:
			sell_stock();
			break;

		case SCR_SETTINGS:
			select_left_setting();		// fall through...
		case SCR_OPTIONS:
			return;						// ...don't roll (settings and options = paused)
	}
	if (!docked)
	{
		if (flight_roll < 0)
			flight_roll = 0;
		else
		{
			increase_flight_roll();
			increase_flight_roll();
			rolling = 1;
		}
	}
}

static void arrow_up(void)
{
	switch (current_screen)
	{
		case SCR_EQUIP_SHIP:
			select_previous_equip();
			break;
		case SCR_MODIFY_SHIP:
			select_previous_modification();
			break;

		case SCR_GALACTIC_CHART:
		case SCR_SHORT_RANGE:
			move_cross(0, -1);
			break;

		case SCR_MARKET_PRICES:
			select_previous_stock();
			break;

		case SCR_OPTIONS:
			select_previous_option();
			return;						// don't pitch (options = paused)

		case SCR_SETTINGS:
			select_up_setting();
			return;						// don't pitch (settings = paused)
	}
	if (!docked)
	{
		if (flight_climb > 0)
			flight_climb = 0;
		else
		{
			decrease_flight_climb();
		}
		climbing = 1;
	}
}

static void arrow_down(void)
{
	switch (current_screen)
	{
		case SCR_EQUIP_SHIP:
			select_next_equip();
			break;
		case SCR_MODIFY_SHIP:
			select_next_modification();
			break;

		case SCR_GALACTIC_CHART:
		case SCR_SHORT_RANGE:
			move_cross(0, 1);
			break;

		case SCR_MARKET_PRICES:
			select_next_stock();
			break;

		case SCR_OPTIONS:
			select_next_option();
			return;						// don't pitch (options = paused)

		case SCR_SETTINGS:
			select_down_setting();
			return;						// don't pitch (settings = paused)
	}
	if (!docked)
	{
		if (flight_climb < 0)
			flight_climb = 0;
		else
		{
			increase_flight_climb();
		}
		climbing = 1;
	}
}

static void return_pressed(void)
{
	switch (current_screen)
	{
		case SCR_EQUIP_SHIP:
			buy_equip();
			break;
		case SCR_MODIFY_SHIP:
			purchase_modification();
			break;

		case SCR_OPTIONS:
			do_option();
			break;

		case SCR_SETTINGS:
			toggle_setting();
			break;
	}	
}

static void y_pressed(void)
{
	switch (current_screen)
	{
		case SCR_QUIT:
			finish_game();
			break;
	}
}

static void n_pressed(void)
{
	switch (current_screen)
	{
		case SCR_QUIT:
			if (docked)
				display_commander_status();
			else
				current_screen = SCR_FRONT_VIEW;
			break;
	}
}

static void d_pressed(void)
{
	switch (current_screen)
	{
		case SCR_GALACTIC_CHART:
		case SCR_SHORT_RANGE:
    		show_distance_to_planet();
			break;
		
		case SCR_FRONT_VIEW:
		case SCR_REAR_VIEW:
		case SCR_RIGHT_VIEW:
		case SCR_LEFT_VIEW:
			if (auto_pilot)
				disengage_auto_pilot();
			break;
	}
}

static void f_pressed(void)
{
	if ((current_screen == SCR_GALACTIC_CHART) ||
		(current_screen == SCR_SHORT_RANGE))
	{
		find_input = 1;
		*find_name = '\0';
		gfx_clear_text_area();
		gfx_display_text(16, 340, "Planet Name?");
	}
}

static void o_pressed()
{
	switch (current_screen)
	{
		case SCR_GALACTIC_CHART:
		case SCR_SHORT_RANGE:
			move_cursor_to_origin();
			break;
	}
}
#pragma endregion


static void add_find_char(int letter)
{
	char str[40];

	if (strlen(find_name) == 16)
		return;

	str[0] = toupper(letter);
	str[1] = '\0';
	strcat(find_name, str);

	sprintf(str, "Planet Name? %s", find_name);
	gfx_clear_text_area();
	gfx_display_text(16, 340, str);
}
static void delete_find_char(void)
{
	char str[40];
	int len;

	len = strlen(find_name);
	if (len == 0)
		return;

	find_name[len - 1] = '\0';

	sprintf(str, "Planet Name? %s", find_name);
	gfx_clear_text_area();
	gfx_display_text(16, 340, str);
}


static void auto_dock(void)
{
	struct univ_object ship;

	ship.location.x = 0;
	ship.location.y = 0;
	ship.location.z = 0;
	
	set_init_matrix(ship.rotmat);
	ship.rotmat[2].z = 1;
	ship.rotmat[0].x = -1;
	ship.type = -96;
	ship.velocity = flight_speed;
	ship.acceleration = 0;
	ship.bravery = 0;
	ship.rotz = 0;
	ship.rotx = 0;

	auto_pilot_ship(&ship);

	if (ship.velocity > 22)
		flight_speed = 22;
	else
		flight_speed = ship.velocity;
	
	if (ship.acceleration > 0)
	{
		flight_speed++;
		if (flight_speed > 22)
			flight_speed = 22;
	}

	if (ship.acceleration < 0)
	{
		flight_speed--;
		if (flight_speed < 1)
			flight_speed = 1;
	}	

	if (ship.rotx == 0)
		flight_climb = 0;
	
	if (ship.rotx < 0)
	{
		increase_flight_climb();

		if (ship.rotx < -1)
			increase_flight_climb();
	}
	
	if (ship.rotx > 0)
	{
		decrease_flight_climb();

		if (ship.rotx > 1)
			decrease_flight_climb();
	}
	
	if (ship.rotz == 127)
		flight_roll = -14;
	else
	{
		if (ship.rotz == 0)
			flight_roll = 0;

		if (ship.rotz > 0)
		{
			increase_flight_roll();

			if (ship.rotz > 1)
				increase_flight_roll();
		}
		
		if (ship.rotz < 0)
		{
			decrease_flight_roll();

			if (ship.rotz < -1)
				decrease_flight_roll();
		}
	}
}


static void run_escape_sequence(void)
{
	int i;
	int newship;
	Matrix rotmat;
	
	current_screen = SCR_ESCAPE_POD;
	
	flight_speed = 1;
	flight_roll = 0;
	flight_climb = 0;

	set_init_matrix(rotmat);
	rotmat[2].z = 1.0;
	
	newship = add_new_ship(SHIP_COBRA3, 0, 0, 200, rotmat, -127, -127);
	universe[newship].velocity = 7;
	snd_play_sample(ass_smp_launch);

	for (i = 0; i < 90; i++)
	{
		if (i == 40)
		{
			universe[newship].flags |= FLG_DEAD;
			snd_play_sample(ass_smp_explode);
		}

		gfx_set_clip_region(1, 1, 510, 383);
		gfx_clear_display();
		update_starfield();
		update_universe();

		universe[newship].location.x = 0;
		universe[newship].location.y = 0;
		universe[newship].location.z += 2;

		gfx_display_centre_text(358, "Escape pod launched - Ship auto-destuct initiated.", 120, GFX_COL_WHITE);
		
		update_console();
		gmlbUpdateScreen();
	}

	
	while ((ship_count[SHIP_CORIOLIS] == 0) &&
		   (ship_count[SHIP_DODEC] == 0))
	{
		auto_dock();

		if ((abs(flight_roll) < 3) && (abs(flight_climb) < 3))
		{
			for (i = 0; i < MAX_UNIV_OBJECTS; i++)
			{
				if (universe[i].type != 0)
					universe[i].location.z -= 1500;
			}

		}

		warp_stars = 1;
		gfx_set_clip_region(1, 1, 510, 383);
		gfx_clear_display();
		update_starfield();
		update_universe();
		update_console();
		gmlbUpdateScreen();
	}

	abandon_ship();
}


static void handle_flight_joystick()
{
	gmlbJoystickPoll();

	GmlbJoystick *pJoystick = gmlbJoystickGetCurrent();
	GmlbJoystick *pJoystickPrev = gmlbJoystickGetPrevious();

	if (pJoystick->up)
		gmlbKeyboard.kbd_up_pressed = 1;
	if (pJoystick->down)
		gmlbKeyboard.kbd_down_pressed = 1;
	if (pJoystick->left)
		gmlbKeyboard.kbd_left_pressed = 1;;
	if (pJoystick->right)
		gmlbKeyboard.kbd_right_pressed = 1;

	// Respond to buttons
	if (pJoystick->fire1)
		gmlbKeyboard.kbd_inc_speed_pressed = 1;
	if (pJoystick->fire2)
		gmlbKeyboard.kbd_dec_speed_pressed = 1;

	if (current_screen == SCR_QUIT)
	{
		if (pJoystick->fire0 && !(pJoystickPrev->fire0))
			gmlbKeyboard.kbd_y_pressed = 1;
		if (pJoystick->fire1 && !(pJoystickPrev->fire1))
			gmlbKeyboard.kbd_n_pressed = 1;
	}
	else if ( (current_screen == SCR_EQUIP_SHIP) ||
		      (current_screen == SCR_OPTIONS)    ||
		      (current_screen == SCR_SETTINGS)   ||
			  (current_screen == SCR_MODIFY_SHIP))
	{
		if ((pJoystick->fire0) && (pJoystickPrev->fire0))
			gmlbKeyboard.kbd_enter_pressed = 1;
	}
	else  // in flight
	{
		if (x360_controller)
			gmlbKeyboard.kbd_fire_pressed |= pJoystick->fire4;
		else
			gmlbKeyboard.kbd_fire_pressed |= pJoystick->fire0;
	}

	/// Bonus controls for x360
	if (x360_controller == 1)
	{
		if (pJoystick->fire7 && !(pJoystickPrev->fire7))	/// Start button - options menu/resume
		{
			if (game_paused)
				gmlbKeyboard.kbd_resume_pressed = 1;
			else
				gmlbKeyboard.kbd_F11_pressed = 1;
		}

		if (docked)
		{
			if (pJoystick->fire9)				/// Launch by clicking right stick
				gmlbKeyboard.kbd_F1_pressed = 1;
		}
		else // if (!docked)
		{
			if (pJoystick->fire9)					/// Hyper - space by clicking right stick
				gmlbKeyboard.kbd_hyperspace_pressed = 1;
			if (pJoystick->fire3)					/// Y to jump
				gmlbKeyboard.kbd_jump_pressed = 1;

			if (pJoystick->d_up)		/// Switch view in flight with d-pad
				gmlbKeyboard.kbd_F1_pressed = 1;
			if (pJoystick->d_down)
				gmlbKeyboard.kbd_F2_pressed = 1;
			if (pJoystick->d_left)
				gmlbKeyboard.kbd_F3_pressed = 1;
			if (pJoystick->d_right)
				gmlbKeyboard.kbd_F4_pressed = 1;

			if (pJoystick->fire5)				/// R bumper to refresh obc
				gmlbKeyboard.kbd_o_pressed = 1;
		}
	}
}

static void handle_flight_keys(void)
{
    int keyasc;
	
	gmlbKeyboardPoll();

	/// Except for obc refresh command, joystick handling simply sets kbd flags
	if (have_joystick)
		handle_flight_joystick();
	
	/// If paused, return early: don't process any keys except resume (start button)
	if (game_paused)
	{
		if (gmlbKeyboard.kbd_resume_pressed)
			game_paused = 0;
		return;
	}

	if (gmlbKeyboard.kbd_F1_pressed)
	{
		find_input = 0;
		
		if (docked)
			launch_player();
		else if (current_screen != SCR_FRONT_VIEW)
		{
			current_screen = SCR_FRONT_VIEW;
			flip_stars();
		}
	}
	else if (gmlbKeyboard.kbd_F2_pressed)
	{
		find_input = 0;
		
		if ((!docked) && (current_screen != SCR_REAR_VIEW))
		{
			current_screen = SCR_REAR_VIEW;
			flip_stars();
		}
	}
	else if (gmlbKeyboard.kbd_F3_pressed)
	{
		find_input = 0;
		
		if (docked)
			modify_ship();
		else if (current_screen != SCR_LEFT_VIEW)
		{
			current_screen = SCR_LEFT_VIEW;
			flip_stars();
		}
	}
	else if (gmlbKeyboard.kbd_F4_pressed)
	{
		find_input = 0;
		
		if (docked)
			equip_ship();
		else if (current_screen != SCR_RIGHT_VIEW)
		{
			current_screen = SCR_RIGHT_VIEW;
			flip_stars();
		}
	}
	else if (gmlbKeyboard.kbd_F5_pressed)
	{
		find_input = 0;
		old_cross_x = -1;
		display_galactic_chart();
	}
	else if (gmlbKeyboard.kbd_F6_pressed)
	{
		find_input = 0;
		old_cross_x = -1;
		display_short_range_chart();
	}
	else if (gmlbKeyboard.kbd_F7_pressed)
	{
		find_input = 0;
		display_data_on_planet();
	}
	else if (gmlbKeyboard.kbd_F8_pressed && (!witchspace))
	{
		find_input = 0;
		display_market_prices();
	}	
	else if (gmlbKeyboard.kbd_F9_pressed)
	{
		find_input = 0;
		display_commander_status();
	}
	else if (gmlbKeyboard.kbd_F10_pressed)
	{
		find_input = 0;
		display_inventory();
	}
	else if (gmlbKeyboard.kbd_F11_pressed)
	{
		find_input = 0;
		display_options();
	}

	/// On pause screens (options, settings), direction key handlers only affect menu
	/// selection. On other screens, flight controls are active if not docked.
	if (gmlbKeyboard.kbd_up_pressed)
		arrow_up();
	else if (gmlbKeyboard.kbd_down_pressed)
		arrow_down();
	if (gmlbKeyboard.kbd_left_pressed)
		arrow_left();
	else if (gmlbKeyboard.kbd_right_pressed)
		arrow_right();

	if (gmlbKeyboard.kbd_enter_pressed)
		return_pressed();


	if ((current_screen == SCR_OPTIONS) || (current_screen == SCR_SETTINGS))
		return;
	/// The following on key routines can assume not paused...

	if (find_input)
	{
		keyasc = gmlbKeyboardReadKey();
		
		if (gmlbKeyboard.kbd_enter_pressed)
		{
			find_input = 0;
			find_planet_by_name(find_name);
			return;
		}

		if (gmlbKeyboard.kbd_backspace_pressed)
		{
			delete_find_char();
			return;
		}

		if (isalpha(keyasc))
			add_find_char(keyasc);

		return;		
	}
	
	if (gmlbKeyboard.kbd_y_pressed)
		y_pressed();

	if (gmlbKeyboard.kbd_n_pressed)
		n_pressed();
 
	if (gmlbKeyboard.kbd_d_pressed)
		d_pressed();
	
	if (gmlbKeyboard.kbd_find_pressed)
		f_pressed();
	
	if (gmlbKeyboard.kbd_o_pressed)
		o_pressed();


	if (docked)
		return;
	/// The following routines can assume not docked...

	if (gmlbKeyboard.kbd_fire_pressed && (laser_frames_left == 0))
		laser_frames_left = fire_laser(laser_type);

	if (gmlbKeyboard.kbd_target_missile_pressed)
		arm_missile();
	else if (gmlbKeyboard.kbd_fire_missile_pressed)
		fire_missile();
	else if (gmlbKeyboard.kbd_unarm_missile_pressed)
		unarm_missile();
	else if (gmlbKeyboard.kbd_ecm_pressed && cmdr.ecm)
		activate_ecm(1);

	if ((gmlbKeyboard.kbd_inc_speed_pressed) && (flight_speed < myship.max_speed))
		flight_speed++;
	else if ((gmlbKeyboard.kbd_dec_speed_pressed) && (flight_speed > 1))
		flight_speed--;

	if (gmlbKeyboard.kbd_jump_pressed && (!witchspace))
		jump_warp();
	else if (gmlbKeyboard.kbd_hyperspace_pressed)
		(gmlbKeyboard.kbd_ctrl_pressed) ? start_galactic_hyperspace() : start_hyperspace();

	if ((gmlbKeyboard.kbd_escape_pressed) && (cmdr.escape_pod) && (!witchspace))
		run_escape_sequence();
	else if ((gmlbKeyboard.kbd_energy_bomb_pressed) && (cmdr.energy_bomb))
	{
		detonate_bomb = 1;
		cmdr.energy_bomb = 0;
	}

	if (gmlbKeyboard.kbd_dock_pressed && cmdr.docking_computer)
		(instant_dock) ? engage_instant_dock() : engage_auto_pilot();


	if ((current_screen != SCR_FRONT_VIEW) &&
		(current_screen != SCR_REAR_VIEW) &&
		(current_screen != SCR_LEFT_VIEW) &&
		(current_screen != SCR_RIGHT_VIEW))
		return;
	/// The following routines can assume the key is pressed on a flight screen...

	if (gmlbKeyboard.kbd_pause_pressed)
	{
		game_paused = 1;
		gfx_display_centre_text(115, "PAUSED", 140, 0);
	}
	if (gmlbKeyboard.kbd_o_pressed)
		obc_refresh();
}


static void set_commander_name(char *path)
{
	char *fname, *cname;
	int i;
	
	fname = gmlbFileNameFromPath(path);
	cname = cmdr.name;

	for (i = 0; i < 31; i++)
	{
		if (!isalnum(*fname) || isdigit(*fname))
			break;
		
		*cname++ = toupper(*fname++);
	}	

	*cname = '\0';
}
void save_commander_screen(void)
{
	
	current_screen = SCR_SAVE_CMDR;

	gfx_clear_display();
	gfx_display_centre_text(10, "SAVE COMMANDER", 140, GFX_COL_GOLD);
	gfx_draw_line(0, 36, 511, 36);
	gmlbUpdateScreen();
	
	char path[255];
	strcpy(path, cmdr.name);
	strcat(path, ".nkc");
	
	int clickedOkay = gmlbRequestFile("Save Commander", path, "nkc");
	if (!clickedOkay)			/// clicked Cancel
	{
		display_options();
		return;
	}

	int rv = save_commander_file(path);
	if (rv)
	{
		gfx_display_centre_text(175, "Error Saving Commander!", 140, GFX_COL_GOLD);
		return;
	}
	
	gfx_display_centre_text(175, "Commander Saved", 140, GFX_COL_GOLD);

	set_commander_name(path);
	saved_cmdr = cmdr;
	saved_cmdr.ship_x = docked_planet.d;
	saved_cmdr.ship_y = docked_planet.b;
}
void load_commander_screen(void)
{

	gfx_clear_display();
	gfx_display_centre_text(10, "LOAD COMMANDER", 140, GFX_COL_GOLD);
	gfx_draw_line(0, 36, 511, 36);
	gmlbUpdateScreen();
	
	
	char path[255];
	strcpy(path, "jameson.nkc");
	int clickedOkay = gmlbRequestFile("Load Commander", path, "nkc");
	if (!clickedOkay)			/// clicked Cancel
		return;

	int rv = load_commander_file(path);
	if (rv)
	{
		saved_cmdr = cmdr;
		gfx_display_centre_text(175, "Error Loading Commander!", 140, GFX_COL_GOLD);
		gfx_display_centre_text(200, "Press any key to continue", 140, GFX_COL_GOLD);
		gmlbUpdateScreen();
		gmlbKeyboardReadKey();
		return;
	}
	
	restore_saved_commander();
	set_commander_name(path);
	saved_cmdr = cmdr;
	update_console();
}


static void joystick_poll_fire_for_space()
{
	if (have_joystick)
	{
		gmlbJoystickPoll();
		GmlbJoystick *pJoyStick = gmlbJoystickGetCurrent();

		if (x360_controller == 1)
			gmlbKeyboard.kbd_space_pressed |= pJoyStick->fire4;
		else
			gmlbKeyboard.kbd_space_pressed |= pJoyStick->fire0;
	}
}
static void joystick_poll_yes_no()
{
	if (have_joystick)
	{
		gmlbJoystickPoll();
		GmlbJoystick *pJoyStick = gmlbJoystickGetCurrent();
		GmlbJoystick *pJoystickPrev = gmlbJoystickGetPrevious();

		if (pJoyStick->fire0 && !(pJoystickPrev->fire0))
			gmlbKeyboard.kbd_y_pressed = 1;
		if (pJoyStick->fire1 && !(pJoystickPrev->fire1))
			gmlbKeyboard.kbd_n_pressed = 1;
	}
}

static void run_first_intro_screen(void)
{
	current_screen = SCR_INTRO_ONE;

	snd_play_midi(ass_mid_theme);

	initialise_intro1();

	for (;;)
	{
		update_intro1();

		gmlbUpdateScreen();

		gmlbKeyboardPoll();
		joystick_poll_yes_no();

		if (gmlbKeyboard.kbd_y_pressed)
		{
			snd_stop_midi();	
			load_commander_screen();
			break;
		}
		if (gmlbKeyboard.kbd_n_pressed)
		{ 
			snd_stop_midi();	
			break;
		}
	} 

}
static void run_second_intro_screen(void)
{
	current_screen = SCR_INTRO_TWO;
	
	snd_play_midi(ass_mid_dnube);
		
	initialise_intro2();

	flight_speed = 3;
	flight_roll = 0;
	flight_climb = 0;

	for (;;)
	{
		update_intro2();

		gmlbUpdateScreen();

		gmlbKeyboardPoll();
		joystick_poll_fire_for_space();

		if (gmlbKeyboard.kbd_space_pressed)
			break;
	} 

	obc_clear();			/// Remove any messages generated by the intro screens
	snd_stop_midi();
}
static void run_game_over_screen()
{
	int i;
	int newship;
	Matrix rotmat;
	int type;
	
	current_screen = SCR_GAME_OVER;
	gfx_set_clip_region(1, 1, 510, 383);
	
	flight_speed = 6;
	flight_roll = 0;
	flight_climb = 0;
	clear_universe();

	set_init_matrix(rotmat);

	newship = add_new_ship(SHIP_COBRA3, 0, 0, -400, rotmat, 0, 0);
	universe[newship].flags |= FLG_DEAD;

	for (i = 0; i < 5; i++)
	{
		type = (rand255() & 1) ? SHIP_CARGO : SHIP_ALLOY;
		newship = add_new_ship(type, (rand255() & 63) - 32,
								(rand255() & 63) - 32, -400, rotmat, 0, 0);
		universe[newship].rotz = ((rand255() * 2) & 255) - 128;
		universe[newship].rotx = ((rand255() * 2) & 255) - 128;
		universe[newship].velocity = rand255() & 15;
	}
	
	
	for (i = 0; i < 100; i++)
	{
		gfx_clear_display();
		update_starfield();
		update_universe();
		gfx_display_centre_text(190, "GAME OVER", 140, GFX_COL_GOLD);
		gmlbUpdateScreen();
	}
}


void info_message(const char *message, int col, int beep)
{
	if (message_count > 0)			// TODO: implement message queue
		return;

	if (col == GFX_COL_BLACK)		/// Indicates a pause between messages
		message_count = 12;
	else
	{
		message_count = 37;

		if (beep == 1)
			snd_play_sample(ass_smp_beep);
		else if (beep == 2)
			snd_play_sample(ass_smp_boop);
	}
	message_colour = col;
	strcpy(message_string, message);
}


static int system_initialise()
{
	int rv = gmlbInit();
	if (rv != 0)
		return 0xA5A5A5A5;	// Catastophic failure, no allegro, no error message

	have_joystick = gmlbJoystickInit();

	/// Read cfg file before loading assets, in case directx is specified
	if ((rv = read_config_file()) != 0)
	{
		if (rv == -1)
			gmlbBasicError("Failed to open cfg");
		else
		{
			char buf[64];
			sprintf(buf, "Failed to read cfg\nCheck line %d", rv);
			gmlbBasicError(buf);
		}
	}

	if ((rv = gmlbGraphicsInit(directx)) != 0)
		return rv;			// Catastrophic failure, no graphics
	if ((rv = gmlbSoundInit()) != 0)
		return rv;			// Catastrophic failure, no sound

	if ((rv = load_assets(directx)) != 0)
		return rv;			// Catastrophic failure, no assets

	setColours(directx);
	if ((rv = gmlbGraphicsInit2(speed_cap)) != 0)
		return rv;			// Catastrophic failure, no graphics again

	/// Draw border around ship viewport, and scanner background
	gfx_draw_line(0, 0, 0, 384);
	gfx_draw_line(0, 0, 511, 0);
	gfx_draw_line(511, 0, 511, 384);
	gfx_draw_line(0, 384, 511, 384);
	gmlbGraphicsSprite(ass_bitmaps[ass_bmp_scanner], 0, 385);

	return 0;
}

int elite_main()
{
	int rv = system_initialise();
	if (rv != 0)
		return rv;

	while (!finish)
	{
		game_over = 0;	
		initialise_game();
		dock_player();

		update_console();

		//char buf[80];
		//sprintf(buf, "sizeof(int)  is %d\nsizeof(long) is %d\n", sizeof(int), sizeof(long));
		//dbg_out(buf);

		current_screen = SCR_FRONT_VIEW;
		run_first_intro_screen();
		run_second_intro_screen();

		old_cross_x = -1;
		old_cross_y = -1;

#pragma region Laser testing
		//saved_cmdr.front_laser = MILITARY_LASER;
		//saved_cmdr.rear_laser = MINING_LASER;
		//saved_cmdr.left_laser = PULSE_LASER;
		//saved_cmdr.right_laser = BEAM_LASER;
		//cmdr.front_laser = MILITARY_LASER;
		//cmdr.rear_laser = MINING_LASER;
		//cmdr.left_laser = PULSE_LASER;
		//cmdr.right_laser = BEAM_LASER;
#pragma endregion

		dock_player();
		display_commander_status();
		
		while (!game_over)
		{
			snd_update_sound();
			gmlbUpdateScreen();

			gfx_set_clip_region(1, 1, 510, 383);		/// Ship view port only

			rolling = 0;
			climbing = 0;

			handle_flight_keys();

			if ( (((current_screen == SCR_OPTIONS) || (current_screen == SCR_SETTINGS)) && !docked) || game_paused )
			{
				gmlbGraphicsSetClipRegion(0, 0, 512, 530);
				gmlbGraphicsSprite(ass_bitmaps[ass_bmp_scanner], 0, 385);

				continue;
			}
				
			if (message_count > 0)
				message_count--;

			/// If a message has just finished, pause before the next...
			if ((message_count == 0) && (message_colour != GFX_COL_BLACK))
				info_message(" ", GFX_COL_BLACK, 0);

			if (!rolling)
			{
				if (flight_roll > 0)
					decrease_flight_roll();
			
				if (flight_roll < 0)
					increase_flight_roll();
			}

			if (!climbing)
			{
				if (flight_climb > 0)
					decrease_flight_climb();

				if (flight_climb < 0)
					increase_flight_climb();
			}

			if (!docked)
			{
				gmlbAcquireScreen();
					
				view_title= NULL;
				laser_type = 0;

				if ((current_screen == SCR_FRONT_VIEW) || (current_screen == SCR_REAR_VIEW) ||
					(current_screen == SCR_LEFT_VIEW) || (current_screen == SCR_RIGHT_VIEW) ||
					(current_screen == SCR_INTRO_ONE) || (current_screen == SCR_INTRO_TWO) ||
					(current_screen == SCR_GAME_OVER))
				{
					gfx_clear_display();
					update_starfield();

					switch (current_screen)
					{
					case SCR_FRONT_VIEW:
						view_title= "Front View";
						laser_type = cmdr.front_laser;
						break;

					case SCR_REAR_VIEW:
						view_title= "Rear View";
						laser_type = cmdr.rear_laser;
						break;

					case SCR_LEFT_VIEW:
						view_title= "Left View";
						laser_type = cmdr.left_laser;
						break;

					case SCR_RIGHT_VIEW:
						view_title= "Right View";
						laser_type = cmdr.right_laser;
						break;
					}
				}

				if (auto_pilot)
				{
					auto_dock();
					if ((mcount & 127) == 0)
						info_message("Docking Computers On", GFX_COL_WHITE, 1);
				}

				update_universe();

				if (view_title)
					draw_view_hud_and_lasers();

				if ((message_count > 0) && (message_colour != GFX_COL_BLACK))
					gfx_display_centre_text(358, message_string, 120, message_colour);
					
				if (hyper_ready)
				{
					display_hyper_status();
					if ((mcount & 3) == 0)
					{
						countdown_hyperspace();
					}
				}

				gmlbReleaseScreen();
			
				mcount--;
				if (mcount < 0)
					mcount = 255;

				if ((mcount & 7) == 0)
					regenerate_shields();

				if ((mcount & 31) == 10)
				{
					if (energy < 50)
					{
						info_message("ENERGY LOW", GFX_COL_SNES_249, 1);
					}

					update_altitude();
				}
				
				if ((mcount & 31) == 20)
					update_cabin_temp();
					
				if ((mcount == 0) && (!witchspace))
					random_encounter();
					
				cool_laser();				
				time_ecm();

				update_console();
			}

			if (current_screen == SCR_BREAK_PATTERN)
				display_break_pattern();

			if (cross_timer > 0)
			{
				cross_timer--;
				if (cross_timer == 0)
				{
    				show_distance_to_planet();
				}
			}
			
			if ((cross_x != old_cross_x) ||
				(cross_y != old_cross_y))
			{
				if (old_cross_x != -1)
					draw_cross(old_cross_x, old_cross_y);

				old_cross_x = cross_x;
				old_cross_y = cross_y;

				draw_cross(old_cross_x, old_cross_y);
			}
		}

		if (!finish)		
			run_game_over_screen();
	}

	destroy_assets();
	gmlbGraphicsShutdown();
	
	return 0;
}

