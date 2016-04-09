#ifndef GAMELIB_H
#define GAMELIB_H

/////////////////////////////////////////////////////////////////////////////
/// Interface to to game library (allegro 4.4.2)
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Input

typedef struct {
	int kbd_F1_pressed;
	int kbd_F2_pressed;
	int kbd_F3_pressed;
	int kbd_F4_pressed;
	int kbd_F5_pressed;
	int kbd_F6_pressed;
	int kbd_F7_pressed;
	int kbd_F8_pressed;
	int kbd_F9_pressed;
	int kbd_F10_pressed;
	int kbd_F11_pressed;
	int kbd_F12_pressed;
	int kbd_y_pressed;
	int kbd_n_pressed;
	int kbd_fire_pressed;
	int kbd_ecm_pressed;
	int kbd_energy_bomb_pressed;
	int kbd_hyperspace_pressed;
	int kbd_ctrl_pressed;
	int kbd_jump_pressed;
	int kbd_escape_pressed;
	int kbd_dock_pressed;
	int kbd_d_pressed;
	int kbd_o_pressed;
	int kbd_find_pressed;
	int kbd_fire_missile_pressed;
	int kbd_target_missile_pressed;
	int kbd_unarm_missile_pressed;
	int kbd_pause_pressed;
	int kbd_resume_pressed;
	int kbd_inc_speed_pressed;
	int kbd_dec_speed_pressed;
	int kbd_up_pressed;
	int kbd_down_pressed;
	int kbd_left_pressed;
	int kbd_right_pressed;
	int kbd_enter_pressed;
	int kbd_backspace_pressed;
	int kbd_space_pressed;
#ifdef _DEBUG
	int kbd_dbg_pressed;
#endif
} GmlbKeyboard;
extern GmlbKeyboard gmlbKeyboard;

void gmlbKeyboardPoll();
int gmlbKeyboardReadKey();

int gmlbJoystickInit();
void gmlbJoystickPoll();

typedef struct {
	int up;
	int down;
	int left;
	int right;
	int d_up;
	int d_down;
	int d_left;
	int d_right;
	int fire0;
	int fire1;
	int fire2;
	int fire3;
	int fire4;
	int fire5;
	int fire7;
	int fire9;
} GmlbJoystick;

GmlbJoystick *gmlbJoystickGetCurrent();
GmlbJoystick *gmlbJoystickGetPrevious();

/////////////////////////////////////////////////////////////////////////////
// Graphics

typedef struct BITMAP *GmlbPBitmap;

int gmlbGraphicsInit(int dx);
int gmlbGraphicsInit2(int speedCap);
void gmlbGraphicsShutdown();

int  gmlbGraphicsLoadBitmap(const char *file, void **ppBitmap);
int gmlbBitmapGetWidth(GmlbPBitmap pBitmap);
void gmlbDestroyBitmap(GmlbPBitmap pBitmap);

int  gmlbGraphicsLoadFont(const char *file, void **ppFont);
void gmlbDestroyFont(void *pFont);

void gmlbGraphicsSetXorMode(int i);
void gmlbGraphicsSetClipRegion(int x1, int y1, int x2, int y2);

void gmlbAcquireScreen();
void gmlbUpdateScreen();
void gmlbReleaseScreen();

#pragma region Draw primitives, text, sprites
void gmlbPlotPixelDx(int x, int y, int colour);
void gmlbPlotPixelGdi(int x, int y, int colour);
void gmlbPlotPixelSafe(int x, int y, int colour);

void gmlbGraphicsCircleFill(int x, int y, int radius, int colour);
void gmlbGraphicsCircle(int x, int y, int radius, int colour);

void gmlbGraphicsHLine(int x1, int x2, int y, int colour);
void gmlbGraphicsVLine(int y1, int y2, int x, int colour);
void gmlbGraphicsLine(int x1, int y1, int x2, int y2, int colour);

void gmlbGraphicsAALine(int x1, int y1, int x2, int y2);
void gmlbGraphicsAACircle(int cx, int cy, int radius);

void gmlbGraphicsTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int colour);
void gmlbGraphicsRectFill(int x1, int y1, int x2, int y2, int colour);

void gmlbGraphicsPoly(int numPoints, int *poly, int colour);

void gmlbGraphicsText(void *pFont, int x, int y, char *txt, int colour);
void gmlbGraphicsTextCentre(void *pFont, int y, char *txt, int colour);

void gmlbGraphicsBlitScanner();
void gmlbGraphicsBlitSprite(GmlbPBitmap sprite, int x, int y);
#pragma endregion

/////////////////////////////////////////////////////////////////////////////
// Audio

int  gmlbSoundInit();
int  gmlbSoundLoadSample(const char *file, void **ppSample);
int  gmlbSoundPlaySample(void *pSample);
void gmlbDestroySample(void *pSample);

int  gmlbSoundLoadMidi(const char *file, void **ppMidi);
void gmlbSoundPlayMidi(void *pMidi);
void gmlbSoundStopMidi();
void gmlbDestroyMidi(void *pMidi);

/////////////////////////////////////////////////////////////////////////////
// Misc
int gmlbInit();

char *gmlbFileNameFromPath(const char *path);
int gmlbRequestFile(char *title, char *path, char *ext);

void gmlbBasicError(const char *str);


/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

// Using OutputDebugString() requires windows.h, this defines BITMAP
// and clashes with allegro.h. For now, debug functions are in elite.c

//void gmlbDumpString(const char *str);
//void gmlbDumpObjects(int numObjects, int x);
#endif		// #ifdef _DEBUG

#endif		// #ifndef GAMELIB_H
