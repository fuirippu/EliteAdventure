/**
 *
 * Elite - The New Kind.
 *
 * Allegro version of Graphics routines.
 *
 * The code in this file has not been derived from the original Elite code.
 * Written by C.J.Pinder 1999-2001.
 * email: <christian@newkind.co.uk>
 *
 * Routines for drawing anti-aliased lines and circles by T.Harte.
 *
 **/


/// alg_gfx.c
///
/// Routines for drawing primitives, clearing the screen and
/// general interface to Allegro graphics.
///
/// All functions which accept a colour parameter take an
/// index into the global palette, pColours [colours.c, gfx.h].

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include <allegro.h>

#include "gfx.h"
#include "alg_data.h"
#include "elite.h"
#include "file.h"

/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
static BITMAP *gfx_screen;
static volatile int frame_count;
DATAFILE *datafile;
static BITMAP *scanner_image;

#define MAX_POLYS	100

static int start_poly;
static int total_polys;

static struct poly_data
{
	int z;
	int no_points;
	int face_colour;
	int point_list[16];
	int next;
} poly_chain[MAX_POLYS];

#define DIRNAME_DX_ASSETS	DIRNAME_ASSETS "directx\\"


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void frame_timer (void)
{
	frame_count++;
}
END_OF_FUNCTION(frame_timer);


#pragma region Startup and shutdown
/// Updates allegro BITMAP (memory is ARGB) from .phd file
static int patch_bmp_for_directx(const char *strFileName, BITMAP *sprite)
{
	FILE *f = fopen(strFileName, "rb");
	if (f == NULL)
		return -1;

	for (int i = sprite->h - 1; i >= 0; --i)
	{
		for (int j = 0; j <sprite->w; ++j)
		{
			unsigned char argb[4];
			size_t s = fread(argb, sizeof(unsigned char), 4, f);
			if (s != 4)
			{
				fclose(f);
				return -2;
			}

			////////////////////////
			// Mem layout:        //
			//     A R G B        //
			//   0xFF000000 alpha //
			//   0x00FF0000 red   //
			//   0x0000FF00 green //
			//   0x000000FF blue  //
			//////////////////////////////
			// .fui file layout B G R A //
			//////////////////////////////
			unsigned int pixel = (unsigned int)argb[3] << 24;
			pixel |= (unsigned int)argb[2] << 16;
			pixel |= (unsigned int)argb[1] << 8;
			pixel |= (unsigned int)argb[0];

			((unsigned int **)sprite->line)[i][j] = pixel;
		}
	}
	fclose(f);

	return 0;
}
static int patch_for_directx(DATAFILE *datafile)
{
	// TODO: DANUBE and THEME (midi), ELITE_1 and ELITE_2 (fonts)

	int rv = 0;

#pragma region Patch BMPs (.fui files)
	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "blake32-v3.fui", datafile[BLAKE].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "ecm.fui", datafile[ECM].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "elitetx3.fui", datafile[ELITETXT].dat);
	if (rv != 0)
		return rv;

	// FRONTV - not used ?

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "greendot.fui", datafile[GRNDOT].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "missgrn.fui", datafile[MISSILE_G].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "missred.fui", datafile[MISSILE_R].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "missyell.fui", datafile[MISSILE_Y].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "reddot.fui", datafile[REDDOT].dat);
	if (rv != 0)
		return rv;

	rv = patch_bmp_for_directx(DIRNAME_DX_ASSETS "safe.fui", datafile[SAFE].dat);
	if (rv != 0)
		return rv;
#pragma endregion

	/*
	//DATAFILE data;
	//memcpy(&data, &datafile[ELITE_1], sizeof(DATAFILE));
	//fui_output("ELITE_1\n");
	//gfx_dump_datafile(&data);
	//
	//memcpy(&data, &datafile[GRNDOT], sizeof(DATAFILE));
	//fui_output("GRNDOT\n");
	//gfx_dump_datafile(&data);

	ELITE_1
	DATAFILE at 0x007724D8
	->dat at 0x007687A0
	type = 1179602516, size = 3069
	GRNDOT
	DATAFILE at 0x00772518
	->dat at 0x007C05D8
	type = 1112363040, size = 106
	*/

	return rv;
}

int gfx_graphics_startup (void)
{
	PALETTE the_palette;
	int rv;

#ifdef ALLEGRO_WINDOWS	

#ifdef RES_512_512
	rv = set_gfx_mode(GFX_DIRECTX_OVL, 512, 512, 0, 0);
	
	if (rv != 0)
		rv = set_gfx_mode(GFX_DIRECTX_WIN, 512, 512, 0, 0);

	if (rv != 0)
		rv = set_gfx_mode(GFX_GDI, 512, 512, 0, 0);

	if (rv == 0)
		set_display_switch_mode (SWITCH_BACKGROUND);
#else
	if (directx == 1)
	{
		set_color_depth(32);
		rv = set_gfx_mode(GFX_DIRECTX, 800, 600, 0, 0);
	}
	else
		rv = set_gfx_mode(GFX_GDI, 800, 600, 0, 0);
#endif

#else
	rv = set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0);
#endif

	if (rv != 0)
	{
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      	allegro_message("Unable to set graphics mode.");
      	return 1;
	}
	
	const char *strFileName = DIRNAME_ASSETS "elite.dat";
	datafile = load_datafile(strFileName);
	if (!datafile)
	{
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Error loading %s!\n", strFileName);
      	return 1;
	}

	if (directx == 1)
	{
		rv = patch_for_directx(datafile);
		if (rv != 0)
		{
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to patch datafile.");
			return 1;
		}
	}
	setColours(directx);

	scanner_image = load_bitmap(scanner_filename, the_palette);
	if (!scanner_image)
	{
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Error reading scanner bitmap file.\n");
		return 1;
	}

	/* select the scanner palette */
	set_palette(the_palette);

	/* Create the screen buffer bitmap */
	gfx_screen = create_bitmap (SCREEN_W, SCREEN_H);

	clear (gfx_screen);

	blit (scanner_image, gfx_screen, 0, 0, GFX_X_OFFSET, 385+GFX_Y_OFFSET, scanner_image->w, scanner_image->h);

	// Draw viewport border
	gfx_draw_line(  0,   0,   0, 384);
	gfx_draw_line(  0,   0, 511,   0);
	gfx_draw_line(511,   0, 511, 384);
	gfx_draw_line(  0, 384, 511, 384);

	/* Install a timer to regulate the speed of the game... */

	LOCK_VARIABLE(frame_count);
	LOCK_FUNCTION(frame_timer);
	frame_count = 0;
	install_int (frame_timer, speed_cap);
	
	return 0;
}


void gfx_graphics_shutdown (void)
{
	destroy_bitmap(scanner_image);
	destroy_bitmap(gfx_screen);
	unload_datafile(datafile);
}
#pragma endregion


/*
 * Blit the back buffer to the screen.
 */

void gfx_update_screen (void)
{
	while (frame_count < 1)
		rest (10);
	frame_count = 0;
	
	acquire_screen();
 	blit (gfx_screen, screen, GFX_X_OFFSET, GFX_Y_OFFSET, GFX_X_OFFSET, GFX_Y_OFFSET, 512, 512);
	release_screen();
}


void gfx_acquire_screen (void)
{
	acquire_bitmap (gfx_screen);
}


void gfx_release_screen (void)
{
	release_bitmap(gfx_screen);
}

void gfx_fast_plot_pixel (int x, int y, int col)
{
//	_putpixel(gfx_screen, x, y, col);

	if (directx == 1)
		((long *)gfx_screen->line[y])[x] = pColours[col];
	else
		gfx_screen->line[y][x] = pColours[col];
}


void gfx_plot_pixel(int x, int y, int col)
{
	putpixel(gfx_screen, x + GFX_X_OFFSET, y + GFX_Y_OFFSET, pColours[col]);
}


void gfx_draw_filled_circle(int cx, int cy, int radius, int colour)
{
	circlefill(gfx_screen, cx + GFX_X_OFFSET, cy + GFX_Y_OFFSET, radius, pColours[colour]);
}


#pragma region Anti-aliasing
#define AA_BITS 3
#define AA_AND  7

#define trunc(x) ((x) & ~65535)
#define frac(x) ((x) & 65535)
#define invfrac(x) (65535-frac(x))

//#define plot(x,y,c) putpixel(gfx_screen, (x), (y), (c)+AA_BASE)	[235]
static void plot_aa_pixel(int x, int y, int col)
{
	// The parameter col should range from 0 .. 7.
	putpixel(gfx_screen, (x), (y), pColours[col + GFX_COL_AA_0]);
}

/*
 * Draw anti-aliased wireframe circle.
 * By T.Harte.
 */

static void gfx_draw_aa_circle(int cx, int cy, int radius)
{
	int x,y;
	int s;
	int sx, sy;

	cx += GFX_X_OFFSET;
	cy += GFX_Y_OFFSET;

	radius >>= (16 - AA_BITS);

	x = radius;
	s = -radius;
	y = 0;

	while (y <= x)
	{
		//wide pixels
		sx = cx + (x >> AA_BITS); sy = cy + (y >> AA_BITS);

		plot_aa_pixel(sx,     sy, AA_AND - (x & AA_AND));
		plot_aa_pixel(sx + 1, sy, x & AA_AND);

		sy = cy - (y >> AA_BITS);

		plot_aa_pixel(sx,     sy, AA_AND - (x & AA_AND));
		plot_aa_pixel(sx + 1, sy, x & AA_AND);

		sx = cx - (x >> AA_BITS);

		plot_aa_pixel(sx,     sy, AA_AND - (x & AA_AND));
		plot_aa_pixel(sx - 1, sy, x & AA_AND);

		sy = cy + (y >> AA_BITS);

		plot_aa_pixel(sx,     sy, AA_AND - (x & AA_AND));
		plot_aa_pixel(sx - 1, sy, x & AA_AND);

		//tall pixels
		sx = cx + (y >> AA_BITS); sy = cy + (x >> AA_BITS);

		plot_aa_pixel(sx, sy,     AA_AND - (x & AA_AND));
		plot_aa_pixel(sx, sy + 1, x & AA_AND);

		sy = cy - (x >> AA_BITS);

		plot_aa_pixel(sx, sy,     AA_AND - (x & AA_AND));
		plot_aa_pixel(sx, sy - 1, x & AA_AND);

		sx = cx - (y >> AA_BITS);

		plot_aa_pixel(sx, sy,     AA_AND - (x & AA_AND));
		plot_aa_pixel(sx, sy - 1, x & AA_AND);

		sy = cy + (x >> AA_BITS);

		plot_aa_pixel(sx, sy,     AA_AND - (x & AA_AND));
		plot_aa_pixel(sx, sy + 1, x & AA_AND);

		s += AA_AND + 1 + (y << (AA_BITS + 1)) + ((1 << (AA_BITS + 2)) - 2);
		y += AA_AND + 1;

		while(s >= 0)
		{
			s -= (x << 1) + 2;
			x --;
		}
	}
}


/*
 * Draw anti-aliased line.
 * By T.Harte.
 */
 
static void gfx_draw_aa_line (int x1, int y1, int x2, int y2)
{
	fixed grad, xd, yd;
	fixed xgap, ygap, xend, yend, xf, yf;
	fixed brightness1, brightness2, swap;

	int x, y, ix1, ix2, iy1, iy2;

	x1 += itofix(GFX_X_OFFSET);
	x2 += itofix(GFX_X_OFFSET);
	y1 += itofix(GFX_Y_OFFSET);
	y2 += itofix(GFX_Y_OFFSET);

	xd = x2 - x1;
	yd = y2 - y1;

	if (abs(xd) > abs(yd))
	{
		if(x1 > x2)
		{
			swap = x1; x1 = x2; x2 = swap;
			swap = y1; y1 = y2; y2 = swap;
			xd   = -xd;
			yd   = -yd;
		}

		grad = fdiv(yd, xd);

		//end point 1

		xend = trunc(x1 + 32768);
		yend = y1 + fmul(grad, xend-x1);

		xgap = invfrac(x1+32768);

		ix1  = xend >> 16;
		iy1  = yend >> 16;

		brightness1 = fmul(invfrac(yend), xgap);
		brightness2 = fmul(frac(yend), xgap);

		plot_aa_pixel(ix1, iy1,   brightness1 >> (16 - AA_BITS));
		plot_aa_pixel(ix1, iy1+1, brightness2 >> (16 - AA_BITS));

		yf = yend+grad;

		//end point 2;

		xend = trunc(x2 + 32768);
		yend = y2 + fmul(grad, xend-x2);

		xgap = invfrac(x2 - 32768);

		ix2 = xend >> 16;
		iy2 = yend >> 16;

		brightness1 = fmul(invfrac(yend), xgap);
		brightness2 = fmul(frac(yend), xgap);
      
		plot_aa_pixel(ix2, iy2,   brightness1 >> (16 - AA_BITS));
		plot_aa_pixel(ix2, iy2+1, brightness2 >> (16 - AA_BITS));

		for(x = ix1+1; x <= ix2-1; x++)
		{
			brightness1 = invfrac(yf);
			brightness2 = frac(yf);

			plot_aa_pixel(x, (yf >> 16),   brightness1 >> (16 - AA_BITS));
			plot_aa_pixel(x, 1+(yf >> 16), brightness2 >> (16 - AA_BITS));

			yf += grad;
		}
	}
	else
	{
		if(y1 > y2)
		{
			swap = x1; x1 = x2; x2 = swap;
			swap = y1; y1 = y2; y2 = swap;
			xd   = -xd;
			yd   = -yd;
		}

		grad = fdiv(xd, yd);

		//end point 1

		yend = trunc(y1 + 32768);
		xend = x1 + fmul(grad, yend-y1);

		ygap = invfrac(y1+32768);

		iy1  = yend >> 16;
		ix1  = xend >> 16;

		brightness1 = fmul(invfrac(xend), ygap);
		brightness2 = fmul(frac(xend), ygap);

		plot_aa_pixel(ix1,   iy1, brightness1 >> (16 - AA_BITS));
		plot_aa_pixel(ix1+1, iy1, brightness2 >> (16 - AA_BITS));

		xf = xend+grad;

		//end point 2;

		yend = trunc(y2 + 32768);
		xend = x2 + fmul(grad, yend-y2);

		ygap = invfrac(y2 - 32768);

		ix2 = xend >> 16;
		iy2 = yend >> 16;

		brightness1 = fmul(invfrac(xend), ygap);
		brightness2 = fmul(frac(xend), ygap);
      
		plot_aa_pixel(ix2,   iy2, brightness1 >> (16 - AA_BITS));
		plot_aa_pixel(ix2+1, iy2, brightness2 >> (16 - AA_BITS));

		for(y = iy1+1; y <= iy2-1; y++)
		{
			brightness1 = invfrac(xf);
			brightness2 = frac(xf);

			plot_aa_pixel((xf >> 16),   y, brightness1 >> (16 - AA_BITS));
			plot_aa_pixel(1+(xf >> 16), y, brightness2 >> (16 - AA_BITS));

			xf += grad;
		}
	}
}

#undef trunc
#undef frac
#undef invfrac

#undef AA_BITS
#undef AA_AND
#pragma endregion


void gfx_draw_circle (int cx, int cy, int radius, int circle_colour)
{
	if (anti_alias_gfx && (circle_colour == GFX_COL_WHITE))
		gfx_draw_aa_circle (cx, cy, itofix(radius));
	else	
		circle(gfx_screen, cx + GFX_X_OFFSET, cy + GFX_Y_OFFSET, radius, pColours[circle_colour]);
}



void gfx_draw_line (int x1, int y1, int x2, int y2)
{
	if (y1 == y2)
	{
		hline(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, x2 + GFX_X_OFFSET, pColours[GFX_COL_WHITE]);
		return;
	}

	if (x1 == x2)
	{
		vline(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, y2 + GFX_Y_OFFSET, pColours[GFX_COL_WHITE]);
		return;
	}

	if (anti_alias_gfx)
		gfx_draw_aa_line (itofix(x1), itofix(y1), itofix(x2), itofix(y2));
	else
		line(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, x2 + GFX_X_OFFSET, y2 + GFX_Y_OFFSET, pColours[GFX_COL_WHITE]);
}
void gfx_draw_colour_line (int x1, int y1, int x2, int y2, int line_colour)
{
	if (y1 == y2)
	{
		hline(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, x2 + GFX_X_OFFSET, pColours[line_colour]);
		return;
	}

	if (x1 == x2)
	{
		vline(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, y2 + GFX_Y_OFFSET, pColours[line_colour]);
		return;
	}

	if (anti_alias_gfx && (line_colour == GFX_COL_WHITE))
		gfx_draw_aa_line (itofix(x1), itofix(y1), itofix(x2), itofix(y2));
	else
		line(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, x2 + GFX_X_OFFSET, y2 + GFX_Y_OFFSET, pColours[line_colour]);
}



void gfx_draw_triangle (int x1, int y1, int x2, int y2, int x3, int y3, int col)
{
	triangle(gfx_screen, x1 + GFX_X_OFFSET, y1 + GFX_Y_OFFSET, x2 + GFX_X_OFFSET, y2 + GFX_Y_OFFSET,
				   x3 + GFX_X_OFFSET, y3 + GFX_Y_OFFSET, pColours[col]);
}



void gfx_display_text (int x, int y, char *txt)
{
	text_mode (-1);
	textout(gfx_screen, datafile[ELITE_1].dat, txt, (x / (2 / GFX_SCALE)) + GFX_X_OFFSET, (y / (2 / GFX_SCALE)) + GFX_Y_OFFSET, pColours[GFX_COL_WHITE]);
}
void gfx_display_colour_text (int x, int y, char *txt, int col)
{
	text_mode (-1);
	textout(gfx_screen, datafile[ELITE_1].dat, txt, (x / (2 / GFX_SCALE)) + GFX_X_OFFSET, (y / (2 / GFX_SCALE)) + GFX_Y_OFFSET, pColours[col]);
}
void gfx_display_centre_text (int y, char *str, int psize, int col)
{
	int txt_size;
	int txt_colour;
	
	if (psize == 140)
	{
		txt_size = ELITE_2;
		txt_colour = -1;
	}
	else
	{
		txt_size = ELITE_1;
		txt_colour = pColours[col];
	}

	text_mode (-1);
	textout_centre(gfx_screen,  datafile[txt_size].dat, str, (128 * GFX_SCALE) + GFX_X_OFFSET, (y / (2 / GFX_SCALE)) + GFX_Y_OFFSET, txt_colour);
}


void gfx_clear_display (void)
{
	rectfill(gfx_screen, GFX_X_OFFSET + 1, GFX_Y_OFFSET + 1, 510 + GFX_X_OFFSET, 383 + GFX_Y_OFFSET, pColours[GFX_COL_BLACK]);
}
void gfx_clear_text_area (void)
{
	rectfill(gfx_screen, GFX_X_OFFSET + 1, GFX_Y_OFFSET + 340, 510 + GFX_X_OFFSET, 383 + GFX_Y_OFFSET, pColours[GFX_COL_BLACK]);
}
void gfx_clear_area (int tx, int ty, int bx, int by)
{
	rectfill(gfx_screen, tx + GFX_X_OFFSET, ty + GFX_Y_OFFSET,
				   bx + GFX_X_OFFSET, by + GFX_Y_OFFSET, pColours[GFX_COL_BLACK]);
}


void gfx_draw_rectangle (int tx, int ty, int bx, int by, int col)
{
	rectfill(gfx_screen, tx + GFX_X_OFFSET, ty + GFX_Y_OFFSET,
				   bx + GFX_X_OFFSET, by + GFX_Y_OFFSET, pColours[col]);
}


void gfx_display_pretty_text (int tx, int ty, int bx, int by, char *txt)
{
	char strbuf[100];
	char *str;
	char *bptr;
	int len;
	int pos;
	int maxlen;
	
	maxlen = (bx - tx) / 8;

	str = txt;	
	len = strlen(txt);
	
	while (len > 0)
	{
		pos = maxlen;
		if (pos > len)
			pos = len;

		while ((str[pos] != ' ') && (str[pos] != ',') &&
			   (str[pos] != '.') && (str[pos] != '\0'))
		{
			pos--;
		}

		len = len - pos - 1;
	
		for (bptr = strbuf; pos >= 0; pos--)
			*bptr++ = *str++;

		*bptr = '\0';

		text_mode (-1);
		textout(gfx_screen, datafile[ELITE_1].dat, strbuf, tx + GFX_X_OFFSET, ty + GFX_Y_OFFSET, pColours[GFX_COL_WHITE]);
		ty += (8 * GFX_SCALE);
	}
}


void gfx_draw_scanner (void)
{
	blit (scanner_image, gfx_screen, 0, 0, GFX_X_OFFSET, 385+GFX_Y_OFFSET, scanner_image->w, scanner_image->h);
}

void gfx_set_clip_region (int tx, int ty, int bx, int by)
{
	set_clip (gfx_screen, tx + GFX_X_OFFSET, ty + GFX_Y_OFFSET, bx + GFX_X_OFFSET, by + GFX_Y_OFFSET);
}


void gfx_start_render (void)
{
	start_poly = 0;
	total_polys = 0;
}


void gfx_render_polygon(int num_points, int *point_list, int col, int zavg)
{
	int i;
	int x;
	int nx;
	
	if (total_polys == MAX_POLYS)
		return;

	x = total_polys;
	total_polys++;
	
	poly_chain[x].no_points = num_points;
	poly_chain[x].face_colour = col;
	poly_chain[x].z = zavg;
	poly_chain[x].next = -1;

	for (i = 0; i < 16; i++)
		poly_chain[x].point_list[i] = point_list[i];				

	if (x == 0)
		return;

	if (zavg > poly_chain[start_poly].z)
	{
		poly_chain[x].next = start_poly;
		start_poly = x;
		return;
	} 	

	for (i = start_poly; poly_chain[i].next != -1; i = poly_chain[i].next)
	{
		nx = poly_chain[i].next;
		
		if (zavg > poly_chain[nx].z)
		{
			poly_chain[i].next = x;
			poly_chain[x].next = nx;
			return;
		}
	}	
	
	poly_chain[i].next = x;
}
void gfx_render_line (int x1, int y1, int x2, int y2, int dist, int col)
{
	int point_list[4];
	
	point_list[0] = x1;
	point_list[1] = y1;
	point_list[2] = x2;
	point_list[3] = y2;
	
	gfx_render_polygon (2, point_list, col, dist);
}

static void gfx_polygon(int num_points, int *poly_list, int face_colour)
{
	int i;
	int x, y;

	x = 0;
	y = 1;
	for (i = 0; i < num_points; i++)
	{
		poly_list[x] += GFX_X_OFFSET;
		poly_list[y] += GFX_Y_OFFSET;
		x += 2;
		y += 2;
	}

	polygon(gfx_screen, num_points, poly_list, pColours[face_colour]);
}


void gfx_finish_render (void)
{
	int num_points;
	int *pl;
	int i;
	int col;
	
	if (total_polys == 0)
		return;
		
	for (i = start_poly; i != -1; i = poly_chain[i].next)
	{
		num_points = poly_chain[i].no_points;
		pl = poly_chain[i].point_list;
		col = poly_chain[i].face_colour;

		if (num_points == 2)
		{
			gfx_draw_colour_line (pl[0], pl[1], pl[2], pl[3], col);
			continue;
		}
		
		gfx_polygon (num_points, pl, col); 
	};
}


void gfx_draw_sprite (int sprite_no, int x, int y)
{
	BITMAP *sprite_bmp;
	
	switch (sprite_no)
	{
		case IMG_GREEN_DOT:
			sprite_bmp = datafile[GRNDOT].dat;
			break;
		
		case IMG_RED_DOT:
			sprite_bmp = datafile[REDDOT].dat;
			break;
			
		case IMG_BIG_S:
			sprite_bmp = datafile[SAFE].dat;
			break;
		
		case IMG_ELITE_TXT:
			sprite_bmp = datafile[ELITETXT].dat;
			break;
			
		case IMG_BIG_E:
			sprite_bmp = datafile[ECM].dat;
			break;
			
		case IMG_BLAKE:
			sprite_bmp = datafile[BLAKE].dat;
			break;
		
		case IMG_MISSILE_GREEN:
			sprite_bmp = datafile[MISSILE_G].dat;
			break;

		case IMG_MISSILE_YELLOW:
			sprite_bmp = datafile[MISSILE_Y].dat;
			break;

		case IMG_MISSILE_RED:
			sprite_bmp = datafile[MISSILE_R].dat;
			break;

		default:
			return;
	}

	if (x == -1)
		x = ((256 * GFX_SCALE) - sprite_bmp->w) / 2;
	
	draw_sprite (gfx_screen, sprite_bmp, x + GFX_X_OFFSET, y + GFX_Y_OFFSET);
}


int gfx_request_file (char *title, char *path, char *ext)
{
	int okay;

	show_mouse (screen);
	okay = file_select (title, path, ext);
	show_mouse (NULL);

	return okay;
}

