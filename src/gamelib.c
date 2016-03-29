#include <stdio.h>

#include <allegro.h>

#include "gamelib.h"


/// Interface to game library (allegro)


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void gmlbBasicError(const char *str)
{
	set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	allegro_message(str);
}

/////////////////////////////////////////////////////////////////////////////

int gmlbGraphicsLoadBitmap(const char *file, void **ppBitmap)
{
	*ppBitmap = load_bitmap(file, NULL);
	if (*ppBitmap)
		return 0;
	else
		return -1;
}

int  gmlbGraphicsLoadFont(const char *file, void **ppFont)
{
	*ppFont = load_font(file, NULL, NULL);
	if (*ppFont)
		return 0;
	else
		return -1;
}

/////////////////////////////////////////////////////////////////////////////

#define SAMPLE_VOLUME		(128)
#define MIDI_VOLUME			 (96)

int gmlbSoundInit()
{
	int rv = install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".");
	if (rv == 0)
		set_volume(SAMPLE_VOLUME, MIDI_VOLUME);

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













#ifdef _DEBUG
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
