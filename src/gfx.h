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

/**
 *
 * Elite - The New Kind.
 *
 * The code in this file has not been derived from the original Elite code.
 * Written by C.J.Pinder 1999/2000.
 *
 **/

#ifndef GFX_H
#define GFX_H


//#define RES_512_512
#define RES_800_600


#pragma region Screen resolution constants

#ifdef RES_512_512

#define GFX_SCALE		(2)
#define GFX_X_OFFSET	(0)
#define GFX_Y_OFFSET	(0)
#define GFX_X_CENTRE	(256)
#define GFX_Y_CENTRE	(192)

#define GFX_VIEW_TX		1
#define GFX_VIEW_TY		1
#define GFX_VIEW_BX		509
#define GFX_VIEW_BY		381

#endif
#ifdef RES_800_600

#define GFX_SCALE		(2)
#define GFX_X_OFFSET	(144)
#define GFX_Y_OFFSET	(44)
#define GFX_X_CENTRE	(256)
#define GFX_Y_CENTRE	(192)

#define GFX_VIEW_TX		1
#define GFX_VIEW_TY		1
#define GFX_VIEW_BX		509
#define GFX_VIEW_BY		381

#endif

#ifndef GFX_SCALE

#define GFX_SCALE		(1)
#define GFX_X_OFFSET	(0)
#define GFX_Y_OFFSET	(0)
#define GFX_X_CENTRE	(128)
#define GFX_Y_CENTRE	(96)

#define GFX_VIEW_TX		1
#define GFX_VIEW_TY		1
#define GFX_VIEW_BX		253
#define GFX_VIEW_BY		191

#endif
 
#pragma endregion


/// GFX_COL_xxx defines are indices into the software palette pColours [colours.c]
#pragma region Colour defines
#define GFX_COL_BLACK		0
#define GFX_COL_DARK_RED	1
#define GFX_COL_WHITE		2
#define GFX_COL_GOLD		3
#define GFX_COL_RED			4
#define GFX_COL_CYAN		5

#define GFX_COL_GREY_1		6
#define GFX_COL_GREY_2		7
#define GFX_COL_GREY_3		8
#define GFX_COL_GREY_4		9

#define GFX_COL_BLUE_1		10
#define GFX_COL_BLUE_2		11
#define GFX_COL_BLUE_3		12
#define GFX_COL_BLUE_4		13

#define GFX_COL_RED_3		14
#define GFX_COL_RED_4		15

#define GFX_COL_WHITE_2		16

#define GFX_COL_YELLOW_1	17
#define GFX_COL_YELLOW_2	18
#define GFX_COL_YELLOW_3	19
#define GFX_COL_YELLOW_4	20
#define GFX_COL_YELLOW_5	21

#define GFX_ORANGE_1		22
#define GFX_ORANGE_2		23
#define GFX_ORANGE_3		24

#define GFX_COL_GREEN_1		25
#define GFX_COL_GREEN_2		26
#define GFX_COL_GREEN_3		27

#define GFX_COL_PINK_1		28

#define GFX_COL_SNES_102	29
#define GFX_COL_SNES_134	30
#define GFX_COL_SNES_167	31
#define GFX_COL_SNES_213	32
#define GFX_COL_SNES_255	GFX_COL_WHITE
#define GFX_COL_SNES__83	33
#define GFX_COL_SNES_122	GFX_ORANGE_3
#define GFX_COL_SNES_249	34

#define GFX_COL_AA_0		35
// Eight entries for anti-aliasing GFX_COL_AA_0-7...

#define GFX_COL_FUI			43
#define GFX_COL_BAR_ALERT0	44
#define GFX_COL_BAR_ALERT1	45
#define GFX_COL_BAR_SAFE0	46
#define GFX_COL_BAR_SAFE1	47
#define GFX_COL_BAR_SAFE2	48
#define GFX_COL_BAR_MED0	49
#define GFX_COL_BAR_MED1	50

#define GFX_COL_BAR_MED2	51
#define GFX_COL_CARDINAL	52

#define GFX_COL_BRK_00		53
// Eight entries for break pattern GFX_COL_BRK_00-7...
//		GFX_COL_BRK_07		60

#define GFX_COL_MISSILE		61
#define GFX_COL_VIPER		62
#define GFX_COL_PURPLE_1	63
#define GFX_COL_PURPLE_2	64
#define GFX_COL_SCAN_RED	65
#define GFX_COL_SCAN_YEL	66
#define GFX_COL_MINT		67
#define GFX_COL_NAVAJO		68

#define GFX_COL_PSMITH_00	69
#define GFX_COL_PSMITH_01	70
#define GFX_COL_PSMITH_02	71
#define GFX_COL_PSMITH_03	72
#define GFX_COL_PSMITH_04	73
#define GFX_COL_PSMITH_05	74
#define GFX_COL_PSMITH_06	75
#define GFX_COL_PSMITH_07	76

#define GFX_COL_TRICOL_0	77
// Three entries for tricol GFX_COL_TRICOL_0-2...
//		GFX_COL_TRICOL_2	79

#define GFX_COL_FRAC_0D		80
#define GFX_COL_FRAC_1D		81
#define GFX_COL_FRAC_0L		82
#define GFX_COL_FRAC_1L		83

#define GFX_COL_PINK_SHOCK	84

#define GFX_COL_NEXT		85
///        ^--- GFX_COL_NEXT must be (last colour + 1)
#pragma endregion

/// The software palette (entries are ARGB for dx, 8b palette index for GDI) [colours.c]
#define NUM_COLOURS			(GFX_COL_NEXT)
extern const int *pColours;
void setColours(int use_directx);

/////////////////////////////////////////////////////////////////////////////
void gfx_fast_plot_pixel(int x, int y, int col);
void gfx_plot_pixel(int x, int y, int col);

void gfx_draw_circle(int cx, int cy, int radius, int colour);
void gfx_draw_filled_circle(int cx, int cy, int radius, int colour);

void gfx_draw_colour_line(int x1, int y1, int x2, int y2, int colour);
void gfx_draw_line(int x1, int y1, int x2, int y2);

void gfx_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int colour);
void gfx_draw_rectangle(int tx, int ty, int bx, int by, int colour);

void gfx_display_colour_text(int x, int y, char *txt, int colour);
void gfx_display_text(int x, int y, char *txt);
void gfx_display_pretty_text(int x1, int y1, int x2, int y2, char *txt);
void gfx_display_centre_text(int y, char *str, int psize, int colour);

void gfx_draw_sprite(int sprite_no, int x, int y);
/////////////////////////////////////////////////////////////////////////////

void gfx_draw_scanner(void);

void gfx_clear_display(void);
void gfx_clear_text_area(void);
void gfx_clear_area(int tx, int ty, int bx, int by);

void gfx_set_clip_region(int tx, int ty, int bx, int by);

/////////////////////////////////////////////////////////////////////////////
void gfx_start_render(void);

void gfx_render_polygon(int num_points, int *point_list, int face_colour, int zavg);
void gfx_render_line(int x1, int y1, int x2, int y2, int dist, int colour);

void gfx_finish_render(void);
/////////////////////////////////////////////////////////////////////////////

int gfx_request_file(char *title, char *path, char *ext);


#endif		// #ifndef GFX_H
