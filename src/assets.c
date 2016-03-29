/*
 * assets.c
 */

#include <stdio.h>
//#include <allegro.h>
#include "gamelib.h"

#include "assets.h"



//void debug_out(const char *str);		// obcomp.c


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
const char *assetDir = "assets\\";

void *ass_bitmaps[NUM_BITMAPS];
void *ass_midis[NUM_MIDIS];
void *ass_fonts[NUM_FONTS];

static const char *rawDataDir = "data\\";

static const char *bmp_files[NUM_BITMAPS] = {
	"blake",
	"ecm",
	"elitetx3",
	"greendot",
	"reddot",
	"missgrn",
	"missred",
	"missyell",
	"safe"
};
static const char *midi_files[NUM_MIDIS] = {
	"theme",
	"danube"
};
static const char *font_files[NUM_FONTS] = {
	"verd2",
	"verd4"
};

static const char *bmp_extnsn = ".bmp";
static const char *midi_extnsn = ".mid";
static const char *font_extnsn = ".pcx";

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

static int ass_load_bitmaps()
{
	char buf[128];

	for (int i = 0; i < NUM_BITMAPS; ++i)
	{
		sprintf(buf, "%s%s%s%s", assetDir, rawDataDir, bmp_files[i], bmp_extnsn);
		if (gmlbGraphicsLoadBitmap(buf, &ass_bitmaps[i]) != 0)
		{
			char msg[256];
			sprintf(msg, "Can't load %s", buf);
			gmlbBasicError(msg);
			return -1;
		}
	}
	return 0;
}

static int ass_load_midis()
{
	char buf[128];

	for (int i = 0; i < NUM_MIDIS; ++i)
	{
		sprintf(buf, "%s%s%s%s", assetDir, rawDataDir, midi_files[i], midi_extnsn);
		if (gmlbSoundLoadMidi(buf, &ass_midis[i]) != 0)
		{
			char msg[256];
			sprintf(msg, "Can't load %s", buf);
			gmlbBasicError(msg);
			return -1;
		}
	}
	return 0;
}

static int ass_load_fonts()
{
	char buf[128];

	for (int i = 0; i < NUM_FONTS; ++i)
	{
		sprintf(buf, "%s%s%s%s", assetDir, rawDataDir, font_files[i], font_extnsn);
		if (gmlbGraphicsLoadFont(buf, &ass_fonts[i]) != 0)
		{
			char msg[256];
			sprintf(msg, "Can't load %s", buf);
			gmlbBasicError(msg);
			return -1;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

int load_assets()
{
	if (ass_load_bitmaps() != 0)
		return -1;
	if (ass_load_midis() != 0)
		return -2;
	if (ass_load_fonts() != 0)
		return -3;

	return 0;
}
