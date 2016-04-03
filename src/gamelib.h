#ifndef GAMELIB_H
#define GAMELIB_H

/////////////////////////////////////////////////////////////////////////////
/// Interface to to game library (allegro)
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Input

void gmlbKeyboardPoll();
int gmlbKeyboardReadKey();

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
extern GmlbJoystick joystick;

int gmlbJoystickInit();
void gmlbJoystickPoll();

/////////////////////////////////////////////////////////////////////////////
// Graphics

typedef struct BITMAP *GmlbPBitmap;
// ToDo: typedef for fonts


int gmlbGraphicsInit(int dx);
int gmlbGraphicsInit2(int speedCap);
void gmlbGraphicsShutdown();

int  gmlbGraphicsLoadBitmap(const char *file, void **ppBitmap);
int gmlbBitmapGetWidth(GmlbPBitmap pBMP);
int  gmlbGraphicsLoadFont(const char *file, void **ppFont);

void gmlbGraphicsSetXorMode(int i);
void gmlbGraphicsSetClipRegion(int x1, int y1, int x2, int y2);

void gmlbAcquireScreen();
void gmlbUpdateScreen();
void gmlbReleaseScreen();

void gmlbPlotPixelDx(int x, int y, int col);
void gmlbPlotPixel(int x, int y, int col);
void gmlbPlotPixelA(int x, int y, int col);

void gmlbGraphicsCircleFill(int x, int y, int radius, int colour);
void gmlbGraphicsCircle(int x, int y, int radius, int colour);

void gmlbGraphicsHLine(int x1, int x2, int y, int colour);
void gmlbGraphicsVLine(int y1, int y2, int x, int colour);
void gmlbGraphicsLine(int x1, int y1, int x2, int y2, int colour);
void gmlbGraphicsAALine(int x1, int y1, int x2, int y2, int dx);

void gmlbGraphicsTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int colour);
void gmlbGraphicsRectFill(int x1, int y1, int x2, int y2, int colour);

void gmlbGraphicsPoly(int numPoints, int *poly, int colour);

void gmlbGraphicsText(void *pFont, int x, int y, char *txt, int colour);
void gmlbGraphicsTextCentre(void *pFont, int y, char *txt, int colour);

void gmlbGraphicsBlitScanner();
void gmlbGraphicsBlitSprite(GmlbPBitmap sprite, int x, int y);


/////////////////////////////////////////////////////////////////////////////
// Audio

int  gmlbSoundInit();
int  gmlbSoundLoadSample(const char *file, void **ppSample);
void gmlbSoundUnloadSample(void **ppSample);
int  gmlbSoundPlaySample(void *pSample);

int  gmlbSoundLoadMidi(const char *file, void **ppMidi);
void gmlbSoundPlayMidi(void *pMidi);
void gmlbSoundStopMidi();

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
