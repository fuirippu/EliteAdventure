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
	ass_bmp_safe = 8,
	ass_bmp_scanner = 9
} ass_bmp;
#define NUM_BITMAPS		(10)
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

/////////////////////////////////////////////////////////////////////////////

int load_assets();

void destroy_assets();


#endif		// #ifndef ASSETS_H
