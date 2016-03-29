#ifndef GAMELIB_H
#define GAMELIB_H

/////////////////////////////////////////////////////////////////////////////
/// Interface to to game library (allegro)
/////////////////////////////////////////////////////////////////////////////

void gmlbBasicError(const char *str);

//#ifdef _DEBUG
//void gmlbDumpObjects(int numObjects, int x);
//void gmlbDumpString(const char *str);
//#endif		// #ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////

int  gmlbGraphicsLoadBitmap(const char *file, void **ppBitmap);
int  gmlbGraphicsLoadFont(const char *file, void **ppFont);

/////////////////////////////////////////////////////////////////////////////

int  gmlbSoundInit();
int  gmlbSoundLoadSample(const char *file, void **ppSample);
void gmlbSoundUnloadSample(void **ppSample);
int  gmlbSoundPlaySample(void *pSample);

int  gmlbSoundLoadMidi(const char *file, void **ppMidi);
void gmlbSoundPlayMidi(void *pMidi);
void gmlbSoundStopMidi();


#endif		// #ifndef GAMELIB_H
