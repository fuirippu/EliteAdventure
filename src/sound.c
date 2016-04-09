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
#include "assets.h"


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
static struct sound_sample
{
	int runtime;
	int timeleft;
} sample_list[NUM_SAMPLES] =
{
	{ 32, 0 },			// launch
	{  7, 0 },			// crash
	{ 36, 0 },			// dock
	{ 24, 0 },			// gameover
	{  4, 0 },			// pulse
	{  4, 0 },			// hitem
	{ 23, 0 },			// explode
	{ 23, 0 },			// ecm
	{ 25, 0 },			// missile
	{ 37, 0 },			// hyper
	{  4, 0 },			// incom1
	{  5, 0 },			// incom2
	{  2, 0 },			// beep
	{  7, 0 },			// boop
};
 
 
/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void snd_play_sample(int sample_no)
{
	if ((sample_no < 0) || (sample_no >(NUM_SAMPLES - 1)))
		return;

	if (sample_list[sample_no].timeleft != 0)
		return;

	sample_list[sample_no].timeleft = sample_list[sample_no].runtime;	
	gmlbSoundPlaySample(ass_samples[sample_no]);
}

void snd_play_midi(int midi_no, int repeat)
{
	if ((midi_no < 0) || (midi_no > (NUM_MIDIS - 1)))
		return;

	gmlbSoundPlayMidi(ass_midis[midi_no]);
}
void snd_stop_midi(void)
{
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
