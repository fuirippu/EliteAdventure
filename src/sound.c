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

/*
 * sound.c
 */

#include <stdlib.h>
#include <allegro.h>
#include "sound.h"
#include "alg_data.h" 
#include "file.h"

#define NUM_SAMPLES 14 

extern DATAFILE *datafile;

static int sound_on;

static struct sound_sample
{
 	SAMPLE *sample;
	char filename[256];
	int runtime;
	int timeleft;
} sample_list[NUM_SAMPLES] =
{
	{NULL, DIRNAME_ASSETS "launch.wav",	32, 0},
	{NULL, DIRNAME_ASSETS "crash.wav",		 7, 0},
	{NULL, DIRNAME_ASSETS "dock.wav",		36, 0},
	{NULL, DIRNAME_ASSETS "gameover.wav",	24, 0},
	{NULL, DIRNAME_ASSETS "pulse.wav",		 4, 0},
	{NULL, DIRNAME_ASSETS "hitem.wav",		 4, 0},
	{NULL, DIRNAME_ASSETS "explode.wav",	23, 0},
	{NULL, DIRNAME_ASSETS "ecm.wav",		23, 0},
	{NULL, DIRNAME_ASSETS "missile.wav",	25, 0},
	{NULL, DIRNAME_ASSETS "hyper.wav",		37, 0},
	{NULL, DIRNAME_ASSETS "incom1.wav",	 4, 0},
	{NULL, DIRNAME_ASSETS "incom2.wav",	 5, 0},
	{NULL, DIRNAME_ASSETS "beep.wav",		 2, 0},
	{NULL, DIRNAME_ASSETS "boop.wav",		 7, 0},
};
 
 
void snd_sound_startup (void)
{
	int i;

 	/* Install a sound driver.. */
	sound_on = 1;
	
	if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".") != 0)
	{
		sound_on = 0;
		return;
	}

	/* Load the sound samples... */

	for (i = 0; i < NUM_SAMPLES; i++)
	{
		sample_list[i].sample = load_sample(sample_list[i].filename);
	}
}
 

void snd_sound_shutdown (void)
{
	int i;

	if (!sound_on)
		return;

	for (i = 0; i < NUM_SAMPLES; i++)
	{
		if (sample_list[i].sample != NULL)
		{
			destroy_sample (sample_list[i].sample);
			sample_list[i].sample = NULL;
		}
	}
}


void snd_play_sample (int sample_no)
{
	if (!sound_on)
		return;

	if (sample_list[sample_no].timeleft != 0)
		return;

	sample_list[sample_no].timeleft = sample_list[sample_no].runtime;
		
	play_sample (sample_list[sample_no].sample, 255, 128, 1000, FALSE);
}


void snd_update_sound (void)
{
	int i;
	
	for (i = 0; i < NUM_SAMPLES; i++)
	{
		if (sample_list[i].timeleft > 0)
			sample_list[i].timeleft--;
	}
}


void snd_play_midi (int midi_no, int repeat)
{
	if (!sound_on)
		return;
	
	switch (midi_no)
	{
		case SND_ELITE_THEME:
			play_midi (datafile[THEME].dat, repeat);
			break;
		
		case SND_BLUE_DANUBE:
			play_midi (datafile[DANUBE].dat, repeat);
			break;
	}
}


void snd_stop_midi (void)
{
	if (sound_on);
		play_midi (NULL, TRUE);
}