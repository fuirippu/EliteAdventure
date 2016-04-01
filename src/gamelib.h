#ifndef GAMELIB_H
#define GAMELIB_H

/////////////////////////////////////////////////////////////////////////////
/// Interface to to game library (allegro)
/////////////////////////////////////////////////////////////////////////////

void gmlbBasicError(const char *str);

int gmlbInit();

/////////////////////////////////////////////////////////////////////////////

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

int  gmlbGraphicsLoadBitmap(const char *file, void **ppBitmap);
int  gmlbGraphicsLoadFont(const char *file, void **ppFont);

void gmlbGraphicsSetXorMode(int i);

/////////////////////////////////////////////////////////////////////////////

int  gmlbSoundInit();
int  gmlbSoundLoadSample(const char *file, void **ppSample);
void gmlbSoundUnloadSample(void **ppSample);
int  gmlbSoundPlaySample(void *pSample);

int  gmlbSoundLoadMidi(const char *file, void **ppMidi);
void gmlbSoundPlayMidi(void *pMidi);
void gmlbSoundStopMidi();

/////////////////////////////////////////////////////////////////////////////

char *gmlbFileNameFromPath(const char *path);


//#ifdef _DEBUG
//void gmlbDumpString(const char *str);
//void gmlbDumpObjects(int numObjects, int x);
//#endif		// #ifdef _DEBUG

#endif		// #ifndef GAMELIB_H
