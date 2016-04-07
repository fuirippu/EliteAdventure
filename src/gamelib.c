/// Interface to game library (allegro)
///

/*
* The code in this file has not been derived from the original Elite code.
* Based on code by C.J.Pinder 1999-2001. email: <christian@newkind.co.uk>
* anti-aliased draw line and circle by T.Harte.
*/

#include <stdio.h>
#include <allegro.h>

#include "gamelib.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
// See individual regions below...


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

#pragma region Input
GmlbKeyboard gmlbKeyboard;

static GmlbJoystick gmlbJoysticks[2];		/// addresses passed to caller
static int gmlbJoystickCurrent = 1;


void gmlbKeyboardPoll()
{
	poll_keyboard();

	gmlbKeyboard.kbd_F1_pressed = key[KEY_F1];
	gmlbKeyboard.kbd_F2_pressed = key[KEY_F2];
	gmlbKeyboard.kbd_F3_pressed = key[KEY_F3];
	gmlbKeyboard.kbd_F4_pressed = key[KEY_F4];
	gmlbKeyboard.kbd_F5_pressed = key[KEY_F5];
	gmlbKeyboard.kbd_F6_pressed = key[KEY_F6];
	gmlbKeyboard.kbd_F7_pressed = key[KEY_F7];
	gmlbKeyboard.kbd_F8_pressed = key[KEY_F8];
	gmlbKeyboard.kbd_F9_pressed = key[KEY_F9];
	gmlbKeyboard.kbd_F10_pressed = key[KEY_F10];
	gmlbKeyboard.kbd_F11_pressed = key[KEY_F11];
	gmlbKeyboard.kbd_F12_pressed = key[KEY_F12];

	gmlbKeyboard.kbd_y_pressed = key[KEY_Y];
	gmlbKeyboard.kbd_n_pressed = key[KEY_N];

	gmlbKeyboard.kbd_fire_pressed = key[KEY_A];
	gmlbKeyboard.kbd_ecm_pressed = key[KEY_E];
	gmlbKeyboard.kbd_energy_bomb_pressed = key[KEY_TAB];
	gmlbKeyboard.kbd_hyperspace_pressed = key[KEY_H];
	gmlbKeyboard.kbd_ctrl_pressed = (key[KEY_LCONTROL]) || (key[KEY_RCONTROL]);
	gmlbKeyboard.kbd_jump_pressed = key[KEY_J];
	gmlbKeyboard.kbd_escape_pressed = key[KEY_ESC];

	gmlbKeyboard.kbd_dock_pressed = key[KEY_C];
	gmlbKeyboard.kbd_d_pressed = key[KEY_D];
	gmlbKeyboard.kbd_o_pressed = key[KEY_O];
	gmlbKeyboard.kbd_find_pressed = key[KEY_F];

	gmlbKeyboard.kbd_fire_missile_pressed = key[KEY_M];
	gmlbKeyboard.kbd_target_missile_pressed = key[KEY_T];
	gmlbKeyboard.kbd_unarm_missile_pressed = key[KEY_U];

	gmlbKeyboard.kbd_pause_pressed = key[KEY_P];
	gmlbKeyboard.kbd_resume_pressed = key[KEY_R];

	gmlbKeyboard.kbd_inc_speed_pressed = key[KEY_SPACE];
	gmlbKeyboard.kbd_dec_speed_pressed = key[KEY_SLASH];

	gmlbKeyboard.kbd_up_pressed = key[KEY_S] || key[KEY_UP];
	gmlbKeyboard.kbd_down_pressed = key[KEY_X] || key[KEY_DOWN];
	gmlbKeyboard.kbd_left_pressed = key[KEY_COMMA] || key[KEY_LEFT];
	gmlbKeyboard.kbd_right_pressed = key[KEY_STOP] || key[KEY_RIGHT];

	gmlbKeyboard.kbd_enter_pressed = key[KEY_ENTER];
	gmlbKeyboard.kbd_backspace_pressed = key[KEY_BACKSPACE];
	gmlbKeyboard.kbd_space_pressed = key[KEY_SPACE];

#ifdef _DEBUG
	gmlbKeyboard.kbd_dbg_pressed = key[KEY_B];
#endif

	while (keypressed())
		readkey();
}
int gmlbKeyboardReadKey()
{
	gmlbKeyboard.kbd_enter_pressed = 0;
	gmlbKeyboard.kbd_backspace_pressed = 0;

	int keynum = readkey();
	int keycode = keynum >> 8;
	int keyasc = keynum & 255;

	if (keycode == KEY_ENTER)
	{
		keyasc = 0x0A;
		gmlbKeyboard.kbd_enter_pressed = 1;
		keyasc = 0;
	}
	else if (keyasc == 0x08)
	{
		gmlbKeyboard.kbd_backspace_pressed = 1;
		keyasc = 0;
	}

	return keyasc;
}

int gmlbJoystickInit()
{
	int rv = 0;
	if (install_joystick(JOY_TYPE_AUTODETECT) == 0)
		rv = (num_joysticks > 0);			// joystick is optional

	return rv;
}
void gmlbJoystickPoll()
{
	gmlbJoystickCurrent ^= 1;
	GmlbJoystick *pJoystick = &gmlbJoysticks[gmlbJoystickCurrent];
	memset(pJoystick, 0, sizeof(GmlbJoystick));

	poll_joystick();

	if (joy[0].stick[0].axis[1].d1)
		pJoystick->up = 1;
	if (joy[0].stick[0].axis[1].d2)
		pJoystick->down = 1;
	if (joy[0].stick[0].axis[0].d1)
		pJoystick->left = 1;;
	if (joy[0].stick[0].axis[0].d2)
		pJoystick->right = 1;

	if (joy[0].stick[1].axis[1].d1)
		pJoystick->d_up = 1;
	if (joy[0].stick[1].axis[1].d2)
		pJoystick->d_down = 1;
	if (joy[0].stick[1].axis[0].d1)
		pJoystick->d_left = 1;
	if (joy[0].stick[1].axis[0].d2)
		pJoystick->d_right = 1;

	if (joy[0].button[0].b)
		pJoystick->fire0 = 1;
	if (joy[0].button[1].b)
		pJoystick->fire1 = 1;
	if (joy[0].button[2].b)
		pJoystick->fire2 = 1;
	if (joy[0].button[3].b)
		pJoystick->fire3 = 1;
	if (joy[0].button[4].b)
		pJoystick->fire4 = 1;
	if (joy[0].button[5].b)
		pJoystick->fire5 = 1;
	if (joy[0].button[7].b)
		pJoystick->fire7 = 1;
	if (joy[0].button[9].b)
		pJoystick->fire9 = 1;
}

GmlbJoystick *gmlbJoystickGetCurrent()
{
	return &gmlbJoysticks[gmlbJoystickCurrent];
}
GmlbJoystick *gmlbJoystickGetPrevious()
{
	return &gmlbJoysticks[gmlbJoystickCurrent ^ 1];
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////

#pragma region Graphics
static const char gmlbScanerFilename[256] = "assets\\scanner.bmp";
static BITMAP *gmlbBmpScreen;
static BITMAP *gmlbBmpScanner;

static const int gmlbGfxOffsetX = 144;
static const int gmlbGfxOffsetY = 44;
static const int gmlbGfxScale = 2;


static volatile int frame_count;
void frame_timer(void) { frame_count++; }
END_OF_FUNCTION(frame_timer);


/// Allegro set_gfx_mode() must be called before assets can be loaded.
/// More graphics init is performed in gmlbGraphicsInit2, after loading.
int gmlbGraphicsInit(int dx)
{
	int rv;

#ifdef ALLEGRO_WINDOWS	

#ifdef RES_512_512
	rv = set_gfx_mode(GFX_DIRECTX_OVL, 512, 512, 0, 0);

	if (rv != 0)
		rv = set_gfx_mode(GFX_DIRECTX_WIN, 512, 512, 0, 0);

	if (rv != 0)
		rv = set_gfx_mode(GFX_GDI, 512, 512, 0, 0);

	if (rv == 0)
		set_display_switch_mode(SWITCH_BACKGROUND);
#else
	if (dx == 1)
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
		gmlbBasicError("Unable to set graphics mode.");

	return rv;
}
int gmlbGraphicsInit2(int speedCap)
{
	PALETTE the_palette;
	gmlbBmpScanner = load_bitmap(gmlbScanerFilename, the_palette);
	if (gmlbBmpScanner == NULL)
	{
		gmlbBasicError("Error scanner bitmap");
		return -1;
	}

	set_palette(the_palette);		/// Seems to only affect GDI mode
	gmlbBmpScreen = create_bitmap(SCREEN_W, SCREEN_H);
	clear(gmlbBmpScreen);
	blit(gmlbBmpScanner, gmlbBmpScreen, 0, 0, gmlbGfxOffsetX, 385 + gmlbGfxOffsetY, gmlbBmpScanner->w, gmlbBmpScanner->h);

	LOCK_VARIABLE(frame_count);
	LOCK_FUNCTION(frame_timer);
	frame_count = 0;
	install_int(frame_timer, speedCap);

	return 0;
}
void gmlbGraphicsShutdown()
{
	destroy_bitmap(gmlbBmpScanner);
	destroy_bitmap(gmlbBmpScreen);
}

int gmlbGraphicsLoadBitmap(const char *file, void **ppBitmap)
{
	*ppBitmap = load_bitmap(file, NULL);
	if (*ppBitmap)
		return 0;
	else
		return -1;
}
int gmlbBitmapGetWidth(GmlbPBitmap pBitmap)
{
	return pBitmap->w;
}
void gmlbDestroyBitmap(GmlbPBitmap pBitmap)
{
	destroy_bitmap(pBitmap);
}

int  gmlbGraphicsLoadFont(const char *file, void **ppFont)
{
	*ppFont = load_font(file, NULL, NULL);
	if (*ppFont)
		return 0;
	else
		return -1;
}
void gmlbDestroyFont(void *pFont)
{
	destroy_font(pFont);
}

void gmlbGraphicsSetXorMode(int i)
{
	xor_mode(i);
}
void gmlbGraphicsSetClipRegion(int x1, int y1, int x2, int y2)
{
	set_clip(gmlbBmpScreen, x1 + gmlbGfxOffsetX, y1 + gmlbGfxOffsetY, x2 + gmlbGfxOffsetX, y2 + gmlbGfxOffsetY);
}

void gmlbAcquireScreen()
{
	acquire_bitmap(gmlbBmpScreen);
}
void gmlbUpdateScreen()
{
	while (frame_count < 1)
		rest(10);
	frame_count = 0;

	acquire_screen();
	//blit(gmlbBmpScreen, screen, gmlbGfxOffsetX, gmlbGfxOffsetY, gmlbGfxOffsetX, gmlbGfxOffsetY, 512, 512);
	//blit(gmlbBmpScreen, screen, gmlbGfxOffsetX, gmlbGfxOffsetY, gmlbGfxOffsetX, gmlbGfxOffsetY, 512, 522);
	blit(gmlbBmpScreen, screen, gmlbGfxOffsetX, gmlbGfxOffsetY, gmlbGfxOffsetX, gmlbGfxOffsetY, 512, 530);
	release_screen();
}
void gmlbReleaseScreen()
{
	release_bitmap(gmlbBmpScreen);
}

#pragma region Draw primitives
void gmlbPlotPixelDx(int x, int y, int colour)
{
	((int *)gmlbBmpScreen->line[y])[x] = colour;
}
void gmlbPlotPixelGdi(int x, int y, int colour)
{
	gmlbBmpScreen->line[y][x] = colour;
}
void gmlbPlotPixelSafe(int x, int y, int colour)
{
	putpixel(gmlbBmpScreen, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, colour);
}

void gmlbGraphicsCircleFill(int x, int y, int radius, int colour)
{
	circlefill(gmlbBmpScreen, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, radius, colour);
}
void gmlbGraphicsCircle(int x, int y, int radius, int colour)
{
	circle(gmlbBmpScreen, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, radius, colour);
}

void gmlbGraphicsHLine(int x1, int x2, int y, int colour)
{
	hline(gmlbBmpScreen, x1 + gmlbGfxOffsetX, y + gmlbGfxOffsetY, x2 + gmlbGfxOffsetX, colour);
}
void gmlbGraphicsVLine(int y1, int y2, int x, int colour)
{
	vline(gmlbBmpScreen, x + gmlbGfxOffsetX, y1 + gmlbGfxOffsetY, y2 + gmlbGfxOffsetY, colour);
}
void gmlbGraphicsLine(int x1, int y1, int x2, int y2, int colour)
{
	line(gmlbBmpScreen, x1 + gmlbGfxOffsetX, y1 + gmlbGfxOffsetY, x2 + gmlbGfxOffsetX, y2 + gmlbGfxOffsetY, colour);
}

void gmlbGraphicsTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int colour)
{
	triangle(gmlbBmpScreen, x1 + gmlbGfxOffsetX, y1 + gmlbGfxOffsetY,
		                    x2 + gmlbGfxOffsetX, y2 + gmlbGfxOffsetY,
                            x3 + gmlbGfxOffsetX, y3 + gmlbGfxOffsetY, colour);
}
void gmlbGraphicsRectFill(int x1, int y1, int x2, int y2, int colour)
{
	rectfill(gmlbBmpScreen, x1 + gmlbGfxOffsetX, y1 + gmlbGfxOffsetY, x2 + gmlbGfxOffsetX, y2 + gmlbGfxOffsetY, colour);
}

void gmlbGraphicsPoly(int numPoints, int *poly, int colour)
{
	int x = 0;
	int y = 1;
	for (int i = 0; i < numPoints; i++)
	{
		poly[x] += gmlbGfxOffsetX;
		poly[y] += gmlbGfxOffsetY;
		x += 2;
		y += 2;
	}
	polygon(gmlbBmpScreen, numPoints, poly, colour);
}

void gmlbGraphicsText(void *pFont, int x, int y, char *txt, int colour)
{
	text_mode(-1);
	textout(gmlbBmpScreen, pFont, txt, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, colour);
}
void gmlbGraphicsTextCentre(void *pFont, int y, char *txt, int colour)
{
	text_mode(-1);
	textout_centre(gmlbBmpScreen, pFont, txt, (128 * gmlbGfxScale) + gmlbGfxOffsetX, (y / (2 / gmlbGfxScale)) + gmlbGfxOffsetY, colour);
}

void gmlbGraphicsBlitScanner()
{
	gmlbGraphicsSetClipRegion(0, 0, 512, 530);
	blit(gmlbBmpScanner, gmlbBmpScreen, 0, 0, gmlbGfxOffsetX, 385 + gmlbGfxOffsetY, gmlbBmpScanner->w, gmlbBmpScanner->h);
}
void gmlbGraphicsBlitSprite(GmlbPBitmap sprite, int x, int y)
{
	draw_sprite(gmlbBmpScreen, sprite, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY);
}
#pragma endregion

#pragma region Anti-aliasing
extern const int *pColours;					// colours.c

#define trunc(x)   ((x)& ~65535)
#define frac(x)    ((x)& 65535)
#define invfrac(x) (65535 - frac(x))

static const int gmlbAACol0 = 35;
static const int gmlbAAAnd  = 7;
static const int gmlbAABits = 3;

static void gmlbGraphicsAAPixel(int x, int y, int colour)
{
	// The parameter colour should range from 0 .. 7. 
	gmlbPlotPixelSafe(x, y, pColours[colour + gmlbAACol0]);

	/// Fast plots can freeze if circle (draw wire planet)
	/// or line (xxx_starfield) is drawn outside bitmap.
	//gmlbPlotPixelDx(x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, pColours[colour + gmlbAACol0]);
	//gmlbPlotPixelGdi(x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, pColours[colour + gmlbAACol0]);
}

void gmlbGraphicsAALine(int x1, int y1, int x2, int y2)
{
	fixed grad, xgap, ygap, xend, yend, xf, yf;
	fixed brightness1, brightness2, swap;

	int x, y, ix1, ix2, iy1, iy2;

	x1 = itofix(x1);
	y1 = itofix(y1);
	x2 = itofix(x2);
	y2 = itofix(y2);

	fixed xd = x2 - x1;
	fixed yd = y2 - y1;

	if (abs(xd) > abs(yd))
	{
		if (x1 > x2)
		{
			swap = x1; x1 = x2; x2 = swap;
			swap = y1; y1 = y2; y2 = swap;
			xd = -xd;
			yd = -yd;
		}
		grad = fdiv(yd, xd);

		// end point 1
		xend = trunc(x1 + 32768);
		yend = y1 + fmul(grad, xend - x1);

		xgap = invfrac(x1 + 32768);

		ix1 = xend >> 16;
		iy1 = yend >> 16;

		brightness1 = fmul(invfrac(yend), xgap);
		brightness2 = fmul(frac(yend), xgap);

		gmlbGraphicsAAPixel(ix1, iy1, (brightness1 >> (16 - gmlbAABits)));
		gmlbGraphicsAAPixel(ix1, iy1 + 1, (brightness2 >> (16 - gmlbAABits)));

		yf = yend + grad;

		// end point 2;
		xend = trunc(x2 + 32768);
		yend = y2 + fmul(grad, xend - x2);

		xgap = invfrac(x2 - 32768);

		ix2 = xend >> 16;
		iy2 = yend >> 16;

		brightness1 = fmul(invfrac(yend), xgap);
		brightness2 = fmul(frac(yend), xgap);

		gmlbGraphicsAAPixel(ix2, iy2, (brightness1 >> (16 - gmlbAABits)));
		gmlbGraphicsAAPixel(ix2, iy2 + 1, (brightness2 >> (16 - gmlbAABits)));

		for (x = ix1 + 1; x <= ix2 - 1; x++)
		{
			brightness1 = invfrac(yf);
			brightness2 = frac(yf);

			gmlbGraphicsAAPixel(x, (yf >> 16), (brightness1 >> (16 - gmlbAABits)));
			gmlbGraphicsAAPixel(x, 1 + (yf >> 16), (brightness2 >> (16 - gmlbAABits)));

			yf += grad;
		}
	}
	else
	{
		if (y1 > y2)
		{
			swap = x1; x1 = x2; x2 = swap;
			swap = y1; y1 = y2; y2 = swap;
			xd = -xd;
			yd = -yd;
		}
		grad = fdiv(xd, yd);

		// end point 1
		yend = trunc(y1 + 32768);
		xend = x1 + fmul(grad, yend - y1);

		ygap = invfrac(y1 + 32768);

		iy1 = yend >> 16;
		ix1 = xend >> 16;

		brightness1 = fmul(invfrac(xend), ygap);
		brightness2 = fmul(frac(xend), ygap);

		gmlbGraphicsAAPixel(ix1, iy1, (brightness1 >> (16 - gmlbAABits)));
		gmlbGraphicsAAPixel(ix1 + 1, iy1, (brightness2 >> (16 - gmlbAABits)));

		xf = xend + grad;

		// end point 2;
		yend = trunc(y2 + 32768);
		xend = x2 + fmul(grad, yend - y2);

		ygap = invfrac(y2 - 32768);

		ix2 = xend >> 16;
		iy2 = yend >> 16;

		brightness1 = fmul(invfrac(xend), ygap);
		brightness2 = fmul(frac(xend), ygap);

		gmlbGraphicsAAPixel(ix2, iy2, (brightness1 >> (16 - gmlbAABits)));
		gmlbGraphicsAAPixel(ix2 + 1, iy2, (brightness2 >> (16 - gmlbAABits)));

		for (y = iy1 + 1; y <= iy2 - 1; y++)
		{
			brightness1 = invfrac(xf);
			brightness2 = frac(xf);

			gmlbGraphicsAAPixel((xf >> 16), y, (brightness1 >> (16 - gmlbAABits)));
			gmlbGraphicsAAPixel(1 + (xf >> 16), y, (brightness2 >> (16 - gmlbAABits)));

			xf += grad;
		}
	}
}

void gmlbGraphicsAACircle(int cx, int cy, int radius)
{
	int r = itofix(radius);
	r >>= (16 - gmlbAABits);

	int x = r;
	int s = -r;
	int y = 0;

	int sx, sy;
	while (y <= x)
	{
		// wide pixels
		sx = cx + (x >> gmlbAABits); sy = cy + (y >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx + 1, sy, x & gmlbAAAnd);

		sy = cy - (y >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx + 1, sy, x & gmlbAAAnd);

		sx = cx - (x >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx - 1, sy, x & gmlbAAAnd);

		sy = cy + (y >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx - 1, sy, x & gmlbAAAnd);

		// tall pixels
		sx = cx + (y >> gmlbAABits); sy = cy + (x >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx, sy + 1, x & gmlbAAAnd);

		sy = cy - (x >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx, sy - 1, x & gmlbAAAnd);

		sx = cx - (y >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx, sy - 1, x & gmlbAAAnd);

		sy = cy + (x >> gmlbAABits);
		gmlbGraphicsAAPixel(sx, sy, gmlbAAAnd - (x & gmlbAAAnd));
		gmlbGraphicsAAPixel(sx, sy + 1, x & gmlbAAAnd);

		s += gmlbAAAnd + 1 + (y << (gmlbAABits + 1)) + ((1 << (gmlbAABits + 2)) - 2);
		y += gmlbAAAnd + 1;

		while (s >= 0)
		{
			s -= (x << 1) + 2;
			x--;
		}
	}
}
#pragma endregion

#pragma endregion

/////////////////////////////////////////////////////////////////////////////

#pragma region Audio
static const int gmlbVolumeSamples = 128;
static const int gmlbVolumeMidi = 96;

int gmlbSoundInit()
{
	int rv = install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".");
	if (rv == 0)
		set_volume(gmlbVolumeSamples, gmlbVolumeMidi);

	return rv;
}

int gmlbSoundLoadSample(const char *file, void **ppSample)
{
	*ppSample = load_sample(file);
	if (*ppSample)
		return 0;
	else
		return -1;
}

void gmlbSoundUnloadSample(void **ppSample)
{
	destroy_sample(*ppSample);
	*ppSample = NULL;
}

int gmlbSoundPlaySample(void *pSample)
{
	return play_sample(pSample, 255, 128, 1000, FALSE);
}

/////////////////////////////////////////////////////////////////////////////

int gmlbSoundLoadMidi(const char *file, void **ppMidi)
{
	*ppMidi = load_midi(file);
	if (*ppMidi)
		return 0;
	else
		return -1;
}

void gmlbSoundPlayMidi(void *pMidi)
{
	play_midi(pMidi, TRUE);
}

void gmlbSoundStopMidi()
{
	play_midi(NULL, TRUE);
}

void gmlbDestroyMidi(void *pMidi)
{
	destroy_midi(pMidi);
}
#pragma endregion

/////////////////////////////////////////////////////////////////////////////

#pragma region Misc
int gmlbInit()
{
	int rv = allegro_init();
	if (rv == 0)
	{
		install_keyboard();		// "very unlikely to fail" [Allegro manual]
		install_timer();		// "very unlikely to fail" [Allegro manual]
		install_mouse();									// mouse is optional
	}
	return rv;
}

char *gmlbFileNameFromPath(const char *path)
{
	return get_filename(path);
}
int gmlbRequestFile(char *title, char *path, char *ext)
{
	show_mouse(screen);
	int fileLoaded = file_select(title, path, ext);
	show_mouse(NULL);

	return fileLoaded;
}

void gmlbBasicError(const char *str)
{
	set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	allegro_message(str);
}
#pragma endregion

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
// Using OutputDebugString() requires windows.h, this defines BITMAP
// and clashes with allegro.h. For now, debug functions are in elite.c

/////////////////////////////////////////////////////////////////////////////
//void gmlbDumpString(const char *str)
//{
//	OutputDebugString(str);
//}
//
//
//void gmlbDumpObjects(int numObjects, int x)
//{
//	char buf[128];
//
//	for (int i = 0; i < MAX_UNIV_OBJECTS; i++)
//	{
//		if (universe[i].type != 0)
//		{
//			sprintf(buf, "[%02d] - %s\n", i, obc_ship_name(universe[i].type));
//			dbg_out(buf);
//			sprintf(buf, "      @ <% 11.3f, % 11.3f, % 11.3f> d=%d\n",
//				universe[i].location.x, universe[i].location.y, universe[i].location.z,
//				universe[i].distance);
//			dbg_out(buf);
//		}
//	}
//	dbg_out(" <End_of_universe>\n");
//
//	//sprintf(buf, "Fuel - %d of 64 (max %dly)\n", (cmdr.fuel * 64) / myship.max_fuel, myship.max_fuel);
//	//dbg_out(buf);
//	//sprintf(buf, " Alt - %d (x100km minimum alt)\n", myship.altitude / 4);
//	//dbg_out(buf);
//	sprintf(buf, "Cmdr %s, %d.%d Cr\n", cmdr.name, (cmdr.credits / 10), (cmdr.credits % 10));
//	dbg_out(buf);
//	sprintf(buf, "     score = %d (mission: %d)\n", cmdr.score, cmdr.mission);
//	dbg_out(buf);
//	sprintf(buf, "  NRG unit = %d\n", cmdr.energy_unit);
//	dbg_out(buf);
//}
/////////////////////////////////////////////////////////////////////////////
#endif

/////////////////////////////////////////////////////////////////////////////

int elite_main();			/// [alg_main.c]
int main()
{
	return elite_main();
}
END_OF_MAIN()
