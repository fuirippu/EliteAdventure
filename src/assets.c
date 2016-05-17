/*
 * assets.c - loading and saving of assets (graphics, audio)
 */

#include <stdio.h>

#include "gamelib.h"

#include "assets.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
void *ass_bitmaps[NUM_BITMAPS];
void *ass_fonts[NUM_FONTS];
#ifdef USE_ALG_AUDIO
void *ass_midis[NUM_MIDIS];
#endif // USE_ALG_AUDIO

static const int gdiPaletteBitmap = ass_bmp_scanner;    /// The palette will be set from
                                                        /// this bitmap when loaded
#ifdef WINDOWS
static const char *assetDir = "assets\\";
#else
static const char *assetDir = "assets/";
#endif

/// Filenames
static const char *bmp_files[NUM_BITMAPS] = {
    "blake",
    "ecm",
    "elitetx3",
    "greendot",
    "reddot",
    "missgrn",
    "missred",
    "missyell",
    "safe",
    "scanner"
};
static const char *font_files[NUM_FONTS] = {
    "verd2",
    "verd4",
    "font_01"
};
static const char *sample_files[NUM_SAMPLES] = {
    "launch",
    "crash",
    "dock",
    "gameover",
    "pulse",
    "hitem",
    "explode",
    "ecm",
    "missile",
    "hyper",
    "incom1",
    "incom2",
    "beep",
    "boop"
};
#ifdef USE_ALG_AUDIO
static const char *midi_files[NUM_MIDIS] = {
    "theme",
    "danube"
};
#endif // USE_ALG_AUDIO

/// File extensions
static const char *bmp_extnsn = ".bmp";
static const char *font_extnsn = ".pcx";

#ifdef USE_ALG_AUDIO
static const char *sample_extnsn = ".wav";
static const char *midi_extnsn = ".mid";
#endif // USE_ALG_AUDIO

#ifdef USE_ALSA
static const char *sample_extnsn = ".raw";
#endif // USE_ALSA

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

static int ass_load_bitmaps()
{
    char buf[128];

    for (int i = 0; i < NUM_BITMAPS; ++i)
    {
        sprintf(buf, "%s%s%s", assetDir, bmp_files[i], bmp_extnsn);
        if (gmlbGraphicsLoadBitmap(buf, &ass_bitmaps[i], (i == ass_bmp_scanner)) != 0)
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
    int rv = 0;
    char buf[128];
    for (int i = 0; i < NUM_FONTS; ++i)
    {
        sprintf(buf, "%s%s%s", assetDir, font_files[i], font_extnsn);
        if (i == 2)         /// ass_fnt_fui is a bmp
            sprintf(buf, "%s%s%s", assetDir, font_files[i], bmp_extnsn);

        rv = gmlbGraphicsLoadFont(buf, &ass_fonts[i]);
        if (rv != 0)
        {
            char msg[256];
            sprintf(msg, "Can't load %s", buf);
            gmlbBasicError(msg);
            return rv;
        }
    }
    return rv;
}

static int ass_load_samples()
{
    char buf[128];

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        sprintf(buf, "%s%s%s", assetDir, sample_files[i], sample_extnsn);
        if (gmlbSoundLoadSample(buf, i) != 0)
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
#ifdef USE_ALG_AUDIO
    char buf[128];

    for (int i = 0; i < NUM_MIDIS; ++i)
    {
        sprintf(buf, "%s%s%s", assetDir, midi_files[i], midi_extnsn);
        if (gmlbSoundLoadMidi(buf, &ass_midis[i]) != 0)
        {
            char msg[256];
            sprintf(msg, "Can't load %s", buf);
            gmlbBasicError(msg);
            return -1;
        }
    }
#endif // USE_ALG_AUDIO
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
    if (ass_load_samples() != 0)
        return -4;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////

void destroy_assets()
{
    for (int i = 0; i < NUM_BITMAPS; ++i)
        gmlbDestroyBitmap(ass_bitmaps[i]);

    for (int i = 0; i < NUM_FONTS; ++i)
        gmlbDestroyFont(ass_fonts[i]);

    for (int i = 0; i < NUM_SAMPLES; ++i)
        gmlbDestroySample(i);

#ifdef USE_ALG_AUDIO
    for (int i = 0; i < NUM_MIDIS; ++i)
        gmlbDestroyMidi(ass_midis[i]);
#endif // USE_ALG_AUDIO
}

