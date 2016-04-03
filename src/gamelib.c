#include <stdio.h>

#include <allegro.h>

#include "gamelib.h"

#include "keyboard.h"


/// Interface to game library (allegro)


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
/// See individual regions below...


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

#pragma region Input
void gmlbKeyboardPoll()
{
	poll_keyboard();

	kbd_F1_pressed = key[KEY_F1];
	kbd_F2_pressed = key[KEY_F2];
	kbd_F3_pressed = key[KEY_F3];
	kbd_F4_pressed = key[KEY_F4];
	kbd_F5_pressed = key[KEY_F5];
	kbd_F6_pressed = key[KEY_F6];
	kbd_F7_pressed = key[KEY_F7];
	kbd_F8_pressed = key[KEY_F8];
	kbd_F9_pressed = key[KEY_F9];
	kbd_F10_pressed = key[KEY_F10];
	kbd_F11_pressed = key[KEY_F11];
	kbd_F12_pressed = key[KEY_F12];

	kbd_y_pressed = key[KEY_Y];
	kbd_n_pressed = key[KEY_N];

	kbd_fire_pressed = key[KEY_A];
	kbd_ecm_pressed = key[KEY_E];
	kbd_energy_bomb_pressed = key[KEY_TAB];
	kbd_hyperspace_pressed = key[KEY_H];
	kbd_ctrl_pressed = (key[KEY_LCONTROL]) || (key[KEY_RCONTROL]);
	kbd_jump_pressed = key[KEY_J];
	kbd_escape_pressed = key[KEY_ESC];

	kbd_dock_pressed = key[KEY_C];
	kbd_d_pressed = key[KEY_D];
	kbd_o_pressed = key[KEY_O];
	kbd_find_pressed = key[KEY_F];

	kbd_fire_missile_pressed = key[KEY_M];
	kbd_target_missile_pressed = key[KEY_T];
	kbd_unarm_missile_pressed = key[KEY_U];

	kbd_pause_pressed = key[KEY_P];
	kbd_resume_pressed = key[KEY_R];

	kbd_inc_speed_pressed = key[KEY_SPACE];
	kbd_dec_speed_pressed = key[KEY_SLASH];

	kbd_up_pressed = key[KEY_S] || key[KEY_UP];
	kbd_down_pressed = key[KEY_X] || key[KEY_DOWN];
	kbd_left_pressed = key[KEY_COMMA] || key[KEY_LEFT];
	kbd_right_pressed = key[KEY_STOP] || key[KEY_RIGHT];

	kbd_enter_pressed = key[KEY_ENTER];
	kbd_backspace_pressed = key[KEY_BACKSPACE];
	kbd_space_pressed = key[KEY_SPACE];

#ifdef _DEBUG
	kbd_dbg_pressed = key[KEY_B];
#endif

	while (keypressed())
		readkey();
}

int gmlbKeyboardReadKey()
{
	int keynum = readkey();
	int keycode = keynum >> 8;
	int keyasc = keynum & 255;

	if (keycode == KEY_ENTER)
		keyasc = 0x0A;

	return keyasc;
}


GmlbJoystick joystick;

int gmlbJoystickInit()
{
	int rv = 0;
	if (install_joystick(JOY_TYPE_AUTODETECT) == 0)
		rv = (num_joysticks > 0);			// joystick is optional

	return rv;
}

void gmlbJoystickPoll()
{
	memset(&joystick, 0, sizeof(GmlbJoystick));

	poll_joystick();

	if (joy[0].stick[0].axis[1].d1)
		joystick.up = 1;
	if (joy[0].stick[0].axis[1].d2)
		joystick.down = 1;
	if (joy[0].stick[0].axis[0].d1)
		joystick.left = 1;;
	if (joy[0].stick[0].axis[0].d2)
		joystick.right = 1;

	if (joy[0].stick[1].axis[1].d1)
		joystick.d_up = 1;
	if (joy[0].stick[1].axis[1].d2)
		joystick.d_down = 1;
	if (joy[0].stick[1].axis[0].d1)
		joystick.d_left = 1;
	if (joy[0].stick[1].axis[0].d2)
		joystick.d_right = 1;

	if (joy[0].button[0].b)
		joystick.fire0 = 1;
	if (joy[0].button[1].b)
		joystick.fire1 = 1;
	if (joy[0].button[2].b)
		joystick.fire2 = 1;
	if (joy[0].button[3].b)
		joystick.fire3 = 1;
	if (joy[0].button[4].b)
		joystick.fire4 = 1;
	if (joy[0].button[5].b)
		joystick.fire5 = 1;
	if (joy[0].button[7].b)
		joystick.fire7 = 1;
	if (joy[0].button[9].b)
		joystick.fire9 = 1;
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


/// Allegro set_gfx_mode() must be called before data files can be loaded
/// The rest of the graphics initialisation is performed in gmlbGraphicsInit2,
/// called after the assets are loaded.
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

	// TODO: destroy assets
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
int  gmlbGraphicsLoadFont(const char *file, void **ppFont)
{
	*ppFont = load_font(file, NULL, NULL);
	if (*ppFont)
		return 0;
	else
		return -1;
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
	blit(gmlbBmpScreen, screen, gmlbGfxOffsetX, gmlbGfxOffsetY, gmlbGfxOffsetX, gmlbGfxOffsetY, 512, 512);
	release_screen();
}
void gmlbReleaseScreen()
{
	release_bitmap(gmlbBmpScreen);
}

void gmlbPlotPixelDx(int x, int y, int col)
{
	((long *)gmlbBmpScreen->line[y])[x] = col;
}
void gmlbPlotPixel(int x, int y, int col)
{
	gmlbBmpScreen->line[y][x] = col;
}
void gmlbPlotPixelA(int x, int y, int col)
{
	putpixel(gmlbBmpScreen, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY, col);
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

#pragma region Anti-aliasing
extern const int *pColours;
static const int gmlbAABits = 3;
#define trunc(x) ((x) & ~65535)
#define frac(x) ((x) & 65535)
#define invfrac(x) (65535-frac(x))
/// AALines drawn from xxx_starfield() [stars.c] hang if dx fast pixel plot is used
/// ToDo: test GDI
void gmlbGraphicsAALine(int x1, int y1, int x2, int y2, int useDx)
{
	fixed grad, xd, yd;
	fixed xgap, ygap, xend, yend, xf, yf;
	fixed brightness1, brightness2, swap;
	
	int x, y, ix1, ix2, iy1, iy2;

	fixed x1b = itofix(x1);
	fixed y1b = itofix(y1);
	fixed x2b = itofix(x2);
	fixed y2b = itofix(y2);

	x1 = x1b + itofix(gmlbGfxOffsetX);
	x2 = x2b + itofix(gmlbGfxOffsetX);
	y1 = y1b + itofix(gmlbGfxOffsetY);
	y2 = y2b + itofix(gmlbGfxOffsetY);
	
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
	
		if (useDx)
		{
			////plot_aa_pixel(ix1, iy1, brightness1 >> (16 - gmlbAABits));
			////plot_aa_pixel(ix1, iy1 + 1, brightness2 >> (16 - gmlbAABits));
			//gmlbPlotPixelDx(ix1, iy1, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			//gmlbPlotPixelDx(ix1, iy1 + 1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix1, iy1, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix1, iy1 + 1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

		}
		else
		{
			//plot_aa_pixel(ix1, iy1, brightness1 >> (16 - gmlbAABits));
			//plot_aa_pixel(ix1, iy1 + 1, brightness2 >> (16 - gmlbAABits));
			gmlbPlotPixel(ix1, iy1, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			gmlbPlotPixel(ix1, iy1 + 1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
		}

		yf = yend+grad;
	
		//end point 2;
	
		xend = trunc(x2 + 32768);
		yend = y2 + fmul(grad, xend-x2);
	
		xgap = invfrac(x2 - 32768);
	
		ix2 = xend >> 16;
		iy2 = yend >> 16;
	
		brightness1 = fmul(invfrac(yend), xgap);
		brightness2 = fmul(frac(yend), xgap);

		if (useDx)
		{
			////plot_aa_pixel(ix2, iy2, brightness1 >> (16 - gmlbAABits));
			////plot_aa_pixel(ix2, iy2 + 1, brightness2 >> (16 - gmlbAABits));
			//gmlbPlotPixelDx(ix2, iy2, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			//gmlbPlotPixelDx(ix2, iy2 + 1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix2, iy2, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix2, iy2 + 1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

			for (x = ix1 + 1; x <= ix2 - 1; x++)
			{
				brightness1 = invfrac(yf);
				brightness2 = frac(yf);

				////plot_aa_pixel(x, (yf >> 16), brightness1 >> (16 - gmlbAABits));
				////plot_aa_pixel(x, 1 + (yf >> 16), brightness2 >> (16 - gmlbAABits));
				//gmlbPlotPixelDx(x, (yf >> 16), pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
				//gmlbPlotPixelDx(x, 1 + (yf >> 16), pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
				putpixel(gmlbBmpScreen, x, (yf >> 16), pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
				putpixel(gmlbBmpScreen, x, 1 + (yf >> 16), pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

				yf += grad;
			}
		}
		else
		{
			{
				//plot_aa_pixel(ix2, iy2, brightness1 >> (16 - gmlbAABits));
				//plot_aa_pixel(ix2, iy2 + 1, brightness2 >> (16 - gmlbAABits));
				gmlbPlotPixel(ix2, iy2, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
				gmlbPlotPixel(ix2, iy2 + 1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

				for (x = ix1 + 1; x <= ix2 - 1; x++)
				{
					brightness1 = invfrac(yf);
					brightness2 = frac(yf);

					//plot_aa_pixel(x, (yf >> 16), brightness1 >> (16 - gmlbAABits));
					//plot_aa_pixel(x, 1 + (yf >> 16), brightness2 >> (16 - gmlbAABits));
					gmlbPlotPixel(x, (yf >> 16), pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
					gmlbPlotPixel(x, 1 + (yf >> 16), pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

					yf += grad;
				}
			}
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
	
		if (useDx)
		{
			////plot_aa_pixel(ix1, iy1, brightness1 >> (16 - gmlbAABits));
			////plot_aa_pixel(ix1 + 1, iy1, brightness2 >> (16 - gmlbAABits));
			//gmlbPlotPixelDx(ix1, iy1, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			//gmlbPlotPixelDx(ix1 + 1, iy1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix1, iy1, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix1 + 1, iy1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
		}
		else
		{
			//plot_aa_pixel(ix1, iy1, brightness1 >> (16 - gmlbAABits));
			//plot_aa_pixel(ix1 + 1, iy1, brightness2 >> (16 - gmlbAABits));
			gmlbPlotPixel(ix1, iy1, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			gmlbPlotPixel(ix1 + 1, iy1, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
		}

		xf = xend+grad;
	
		//end point 2;
	
		yend = trunc(y2 + 32768);
		xend = x2 + fmul(grad, yend-y2);
	
		ygap = invfrac(y2 - 32768);
	
		ix2 = xend >> 16;
		iy2 = yend >> 16;
	
		brightness1 = fmul(invfrac(xend), ygap);
		brightness2 = fmul(frac(xend), ygap);
	      
		if (useDx)
		{
			////plot_aa_pixel(ix2, iy2, brightness1 >> (16 - gmlbAABits));
			////plot_aa_pixel(ix2 + 1, iy2, brightness2 >> (16 - gmlbAABits));
			//gmlbPlotPixelDx(ix2, iy2, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			//gmlbPlotPixelDx(ix2 + 1, iy2, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix2, iy2, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			putpixel(gmlbBmpScreen, ix2 + 1, iy2, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

			for (y = iy1 + 1; y <= iy2 - 1; y++)
			{
				brightness1 = invfrac(xf);
				brightness2 = frac(xf);

				////plot_aa_pixel((xf >> 16), y, brightness1 >> (16 - gmlbAABits));
				////plot_aa_pixel(1 + (xf >> 16), y, brightness2 >> (16 - gmlbAABits));
				//gmlbPlotPixelDx((xf >> 16), y, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
				//gmlbPlotPixelDx(1 + (xf >> 16), y, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);
				putpixel(gmlbBmpScreen, (xf >> 16), y, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
				putpixel(gmlbBmpScreen, 1 + (xf >> 16), y, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

				xf += grad;
			}
		}
		else
		{
			//plot_aa_pixel(ix2, iy2, brightness1 >> (16 - gmlbAABits));
			//plot_aa_pixel(ix2 + 1, iy2, brightness2 >> (16 - gmlbAABits));
			gmlbPlotPixel(ix2, iy2, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
			gmlbPlotPixel(ix2 + 1, iy2, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

			for (y = iy1 + 1; y <= iy2 - 1; y++)
			{
				brightness1 = invfrac(xf);
				brightness2 = frac(xf);

				//plot_aa_pixel((xf >> 16), y, brightness1 >> (16 - gmlbAABits));
				//plot_aa_pixel(1 + (xf >> 16), y, brightness2 >> (16 - gmlbAABits));
				gmlbPlotPixel((xf >> 16), y, pColours[35 + (brightness1 >> (16 - gmlbAABits))]);
				gmlbPlotPixel(1 + (xf >> 16), y, pColours[35 + (brightness2 >> (16 - gmlbAABits))]);

				xf += grad;
			}
		}
	}
}
#pragma endregion

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
	blit(gmlbBmpScanner, gmlbBmpScreen, 0, 0, gmlbGfxOffsetX, 385 + gmlbGfxOffsetY, gmlbBmpScanner->w, gmlbBmpScanner->h);
}
void gmlbGraphicsBlitSprite(GmlbPBitmap sprite, int x, int y)
{
	draw_sprite(gmlbBmpScreen, sprite, x + gmlbGfxOffsetX, y + gmlbGfxOffsetY);
}
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

int elite_main();			/// [alg_main.c]
int main()
{
	return elite_main();
}
END_OF_MAIN()
