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

#include "gamelib.h"

#include "gfx.h"
#include "elite.h"
#include "file.h"
#include "assets.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////


/// Draw/plot primitives (px, circ, ln, tri, rect, txt, sprite)
#pragma region Draw/plot primitives
void gfx_fast_plot_pixel(int x, int y, int colour)
{
	if (directx == 1)
		gmlbPlotPixelDx(x, y, pColours[colour]);
	else
		gmlbPlotPixelGdi(x, y, pColours[colour]);
}
void gfx_plot_pixel(int x, int y, int colour)
{
	gmlbPlotPixelSafe(x, y, pColours[colour]);
}

void gfx_draw_circle(int cx, int cy, int radius, int colour)
{
	if (anti_alias_gfx && (colour == GFX_COL_WHITE))
		gmlbGraphicsAACircle(cx, cy, radius);
	else	
		gmlbGraphicsCircle(cx, cy, radius, pColours[colour]);
}
void gfx_draw_filled_circle(int cx, int cy, int radius, int colour)
{
	gmlbGraphicsCircleFill(cx, cy, radius, pColours[colour]);
}

void gfx_draw_colour_line(int x1, int y1, int x2, int y2, int colour)
{
	if (y1 == y2)
		gmlbGraphicsHLine(x1, x2, y1, pColours[colour]);
	else if (x1 == x2)
		gmlbGraphicsVLine(y1, y2, x1, pColours[colour]);
	else
	{
		if (anti_alias_gfx && (colour == GFX_COL_WHITE))
			gmlbGraphicsAALine(x1, y1, x2, y2);
		else
			gmlbGraphicsLine(x1, y1, x2, y2, pColours[colour]);
	}
}
void gfx_draw_line(int x1, int y1, int x2, int y2)
{
	gfx_draw_colour_line(x1, y1, x2, y2, GFX_COL_WHITE);
}

void gfx_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int colour)
{
	gmlbGraphicsTriangle(x1, y1, x2, y2, x3, y3, pColours[colour]);
}
void gfx_draw_rectangle(int tx, int ty, int bx, int by, int colour)
{
	gmlbGraphicsRectFill(tx, ty, bx, by, pColours[colour]);
}


void gfx_display_colour_text(int x, int y, char *txt, int colour)
{
	gmlbGraphicsText(ass_fonts[ass_fnt_one], (x / (2 / GFX_SCALE)), (y / (2 / GFX_SCALE)), txt, pColours[colour]);
}

void gfx_display_text(int x, int y, char *txt)
{
	gfx_display_colour_text(x, y, txt, GFX_COL_WHITE);
}

void gfx_display_pretty_text(int x1, int y1, int x2, int y2, char *txt)
{
	char strbuf[100];
	char *str;
	char *bptr;
	int len;
	int pos;
	int maxlen;

	maxlen = (x2 - x1) / 8;

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

		gmlbGraphicsText(ass_fonts[ass_fnt_one], x1, y1, strbuf, pColours[GFX_COL_WHITE]);
		y1 += (8 * GFX_SCALE);
	}
}

void gfx_display_centre_text(int y, char *str, int psize, int colour)
{
	ass_fnt font;
	int txt_colour;

	if (psize == 140)
	{
		font = ass_fnt_two;
		txt_colour = -1;
	}
	else
	{
		font = ass_fnt_one;
		txt_colour = pColours[colour];
	}

	gmlbGraphicsTextCentre(ass_fonts[font], y, str, txt_colour);
}


void gfx_draw_sprite(int sprite_no, int x, int y)
{
	GmlbPBitmap sprite_bmp;

	if ((sprite_no < 0) || (sprite_no >(NUM_BITMAPS - 1)))
		return;
	sprite_bmp = ass_bitmaps[sprite_no];

	if (x == -1)
		x = (((256 * GFX_SCALE) - gmlbBitmapGetWidth(sprite_bmp)) / 2);

	gmlbGraphicsBlitSprite(sprite_bmp, x, y);
}
#pragma endregion


void gfx_draw_scanner(void)
{
	gmlbGraphicsBlitScanner();
}


void gfx_clear_display(void)
{
	gfx_draw_rectangle(1, 1, 510, 383, GFX_COL_BLACK);
}
void gfx_clear_text_area(void)
{
	gfx_draw_rectangle(1, 340, 510, 383, GFX_COL_BLACK);
}
void gfx_clear_area(int tx, int ty, int bx, int by)
{
	gfx_draw_rectangle(tx, ty, bx, by, GFX_COL_BLACK);
}


void gfx_set_clip_region(int tx, int ty, int bx, int by)
{
	gmlbGraphicsSetClipRegion(tx, ty, bx, by);
}


void gfx_start_render(void)
{
	start_poly = 0;
	total_polys = 0;
}

void gfx_render_polygon(int num_points, int *point_list, int colour, int zavg)
{
	int i;
	int x;
	int nx;
	
	if (total_polys == MAX_POLYS)
		return;

	x = total_polys;
	total_polys++;
	
	poly_chain[x].no_points = num_points;
	poly_chain[x].face_colour = colour;
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
void gfx_render_line(int x1, int y1, int x2, int y2, int dist, int colour)
{
	int point_list[4];
	
	point_list[0] = x1;
	point_list[1] = y1;
	point_list[2] = x2;
	point_list[3] = y2;
	
	gfx_render_polygon(2, point_list, colour, dist);
}

static void gfx_polygon(int num_points, int *poly_list, int face_colour)
{
	gmlbGraphicsPoly(num_points, poly_list, pColours[face_colour]);
}

void gfx_finish_render(void)
{
	int num_points;
	int *pl;
	int i;
	int colour;
	
	if (total_polys == 0)
		return;
		
	for (i = start_poly; i != -1; i = poly_chain[i].next)
	{
		num_points = poly_chain[i].no_points;
		pl = poly_chain[i].point_list;
		colour = poly_chain[i].face_colour;

		if (num_points == 2)
		{
			gfx_draw_colour_line(pl[0], pl[1], pl[2], pl[3], colour);
			continue;
		}
		
		gfx_polygon(num_points, pl, colour); 
	};
}

