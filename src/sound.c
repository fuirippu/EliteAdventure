/*
 * Elite - The New Kind.
 *
 * Reverse engineered from the BBC disk version of Elite.
 * Additional material by C.J.Pinder.
 *
 * The original Elite code is (C) I.Bell & D.Braben 1984.
 * This version re-engineered in C by C.J.Pinder 1999-2001.
 *
 * email: <christian@newkind.co.uk>
 *
 *
 */


#include <stdio.h>

#include "gamelib.h"

#include "sound.h"
#include "assets.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
static int initialised = 0;

static const char *sampleExt = ".wav";

#define NUM_SAMPLES 14 
static struct sound_sample
{
	void *sample;
	char filename[256];
	int runtime;
	int timeleft;
} sample_list[NUM_SAMPLES] =
{
	{NULL, "launch",	32, 0},
	{NULL, "crash",		 7, 0},
	{NULL, "dock",		36, 0},
	{NULL, "gameover",	24, 0},
	{NULL, "pulse",		 4, 0},
	{NULL, "hitem",		 4, 0},
	{NULL, "explode",	23, 0},
	{NULL, "ecm",		23, 0},
	{NULL, "missile",	25, 0},
	{NULL, "hyper",		37, 0},
	{NULL, "incom1",	 4, 0},
	{NULL, "incom2",	 5, 0},
	{NULL, "beep",		 2, 0},
	{NULL, "boop",		 7, 0},
};
 
 
/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
int snd_sound_startup()
{
	int rv = gmlbSoundInit();
	if (rv != 0)
	{
		gmlbBasicError("Error setup sound");
		return -1;
	}

	char buf[128];
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		sprintf(buf, "%s%s%s", assetDir, sample_list[i].filename, sampleExt);

		rv = gmlbSoundLoadSample(buf, &(sample_list[i].sample));
		if (rv != 0)
			return i + 1;
	}
	initialised = 1;

	return 0;
}

void snd_sound_shutdown(void)
{
	if (initialised == 0)
		return;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		if (sample_list[i].sample != NULL)
		{
			gmlbSoundUnloadSample(&sample_list[i].sample);
		}
	}
}


void snd_play_sample(int sample_no)
{
	if (!initialised)
		return;

	if (sample_list[sample_no].timeleft != 0)
		return;

	sample_list[sample_no].timeleft = sample_list[sample_no].runtime;	
	gmlbSoundPlaySample(sample_list[sample_no].sample);
}

void snd_play_midi(int midi_no, int repeat)
{
	if (!initialised)
		return;

	if ((midi_no < 0) || (midi_no > (NUM_MIDIS - 1)))
		return;

	gmlbSoundPlayMidi(ass_midis[midi_no]);
}
void snd_stop_midi(void)
{
	if (initialised)
		gmlbSoundStopMidi();
}


void snd_update_sound(void)
{
	int i;

	for (i = 0; i < NUM_SAMPLES; i++)
	{
		if (sample_list[i].timeleft > 0)
			sample_list[i].timeleft--;
	}
}
