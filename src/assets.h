/*
 * assets.h - loading and saving of assets (graphics, audio)
 */

#ifndef ASSETS_H
#define ASSETS_H

/////////////////////////////////////////////////////////////////////////////

typedef enum {
	ass_bmp_blake = 0,
	ass_bmp_ecm = 1,
	ass_bmp_elite = 2,
	ass_bmp_grndot = 3,
	ass_bmp_reddot = 4,
	ass_bmp_missg = 5,
	ass_bmp_missr = 6,
	ass_bmp_missy = 7,
	ass_bmp_safe = 8
} ass_bmp;
#define NUM_BITMAPS		(9)
extern void *ass_bitmaps[NUM_BITMAPS];

typedef enum {
	ass_mid_theme = 0,
	ass_mid_dnube = 1
} ass_mid;
#define NUM_MIDIS		(2)
extern void *ass_midis[NUM_MIDIS];

typedef enum {
	ass_fnt_one = 0,
	ass_fnt_two = 1
} ass_fnt;
#define NUM_FONTS		(2)
extern void *ass_fonts[NUM_FONTS];

typedef enum {
	ass_smp_launch = 0,
	ass_smp_crash = 1,
	ass_smp_dock = 2,
	ass_smp_gameover = 3,
	ass_smp_pulse = 4,
	ass_smp_hit_enemy = 5,
	ass_smp_explode = 6,
	ass_smp_ecm = 7,
	ass_smp_missile = 8,
	ass_smp_hyperspace = 9,
	ass_smp_incoming_1 = 10,
	ass_smp_incoming_2 = 11,
	ass_smp_beep = 12,
	ass_smp_boop = 13
} ass_smp;
#define NUM_SAMPLES		(14)
extern void *ass_samples[NUM_SAMPLES];

/////////////////////////////////////////////////////////////////////////////

int load_assets();

void destroy_assets();


#endif		// #ifndef ASSETS_H
