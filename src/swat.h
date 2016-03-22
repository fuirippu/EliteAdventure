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

#ifndef SWAT_H
#define SWAT_H


#include "space.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
#define MISSILE_UNARMED	-2
#define MISSILE_ARMED	-1

extern int ecm_active;
extern int missile_target;
extern int in_battle;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
void clear_universe(void);

int add_new_ship(int ship_type, int x, int y, int z, struct vector *rotmat, int rotx, int rotz);
void add_new_station(double sx, double sy, double sz, Matrix rotmat);

void remove_ship(int un);

void reset_weapons (void);
void tactics(int un);
void check_target(int un, struct univ_object *flip);

void draw_laser_shots(int colour);

int fire_laser(int type);		/// returns the number of frames for which to draw the laser
								/// requires type is a valid laser type, not 0 [elite.h]
void cool_laser(void);

void arm_missile(void);
void unarm_missile(void);
void fire_missile(void);

void activate_ecm(int ours);
void time_ecm(void);

void random_encounter(void);

void explode_object(int un);
void abandon_ship(void);
void create_thargoid(void);


#endif		// #ifndef SWAT_H
