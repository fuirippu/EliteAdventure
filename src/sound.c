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
#include <allegro.h>

#include "sound.h"
#include "alg_data.h"			// MIDI assets


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
extern DATAFILE *datafile;		// alg_gfx.c


#define SAMPLE_VOLUME		(128)
#define MIDI_VOLUME			 (96)

static int initialised = 0;

#define NUM_SAMPLES 14 
static struct sound_sample
{
 	SAMPLE *sample;
	char filename[256];
	int runtime;
	int timeleft;
} sample_list[NUM_SAMPLES] =
{
	{NULL, "launch.wav",	32, 0},
	{NULL, "crash.wav",		 7, 0},
	{NULL, "dock.wav",		36, 0},
	{NULL, "gameover.wav",	24, 0},
	{NULL, "pulse.wav",		 4, 0},
	{NULL, "hitem.wav",		 4, 0},
	{NULL, "explode.wav",	23, 0},
	{NULL, "ecm.wav",		23, 0},
	{NULL, "missile.wav",	25, 0},
	{NULL, "hyper.wav",		37, 0},
	{NULL, "incom1.wav",	 4, 0},
	{NULL, "incom2.wav",	 5, 0},
	{NULL, "beep.wav",		 2, 0},
	{NULL, "boop.wav",		 7, 0},
};
 
 
/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
int snd_sound_startup(const char *path)
{
	if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".") != 0)
		return 1;
	set_volume(SAMPLE_VOLUME, MIDI_VOLUME);

	char buf[128];
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		sprintf(buf, "%s%s", path, sample_list[i].filename);

		sample_list[i].sample = load_sample(buf);

		if (sample_list[i].sample == NULL)
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
			destroy_sample(sample_list[i].sample);
			sample_list[i].sample = NULL;
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
	play_sample(sample_list[sample_no].sample, 255, 128, 1000, FALSE);
}

void snd_play_midi(int midi_no, int repeat)
{
	if (!initialised)
		return;

	switch (midi_no)
	{
		case SND_ELITE_THEME:
			play_midi(datafile[THEME].dat, repeat);
			break;
		
		case SND_BLUE_DANUBE:
			play_midi(datafile[DANUBE].dat, repeat);
			break;
	}
}
void snd_stop_midi(void)
{
	if (initialised);
		play_midi(NULL, TRUE);
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
