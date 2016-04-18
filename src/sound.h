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
 * sound.h
 */
 
#ifndef SOUND_H
#define SOUND_H

/////////////////////////////////////////////////////////////////////////////

void snd_play_sample(int sample_no);
void snd_play_midi(int midi_no);
void snd_stop_midi(void);

/// Audio update routine, called at start of main same loop
void snd_update_sound(void);

/////////////////////////////////////////////////////////////////////////////


#endif		// #ifndef SOUND_H
