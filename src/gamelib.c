#include <stdio.h>

#include <allegro.h>

#include "gamelib.h"

#include "keyboard.h"


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

int gmlbInit()
{
	int rv = allegro_init();
	if (rv == 0)
	{
		install_keyboard();		// "very unlikely to fail" [Allegro manual]
		install_timer();		// "very unlikely to fail" [Allegro manual]
		install_mouse();									// mouse is optional
	}
	return rv;
}
/////////////////////////////////////////////////////////////////////////////

#pragma region Keyboard
void gmlbKeyboardPoll()
{
	poll_keyboard();

	kbd_F1_pressed = key[KEY_F1];
	kbd_F2_pressed = key[KEY_F2];
	kbd_F3_pressed = key[KEY_F3];
	kbd_F4_pressed = key[KEY_F4];
	kbd_F5_pressed = key[KEY_F5];
	kbd_F6_pressed = key[KEY_F6];
	kbd_F7_pressed = key[KEY_F7];
	kbd_F8_pressed = key[KEY_F8];
	kbd_F9_pressed = key[KEY_F9];
	kbd_F10_pressed = key[KEY_F10];
	kbd_F11_pressed = key[KEY_F11];
	kbd_F12_pressed = key[KEY_F12];

	kbd_y_pressed = key[KEY_Y];
	kbd_n_pressed = key[KEY_N];

	kbd_fire_pressed = key[KEY_A];
	kbd_ecm_pressed = key[KEY_E];
	kbd_energy_bomb_pressed = key[KEY_TAB];
	kbd_hyperspace_pressed = key[KEY_H];
	kbd_ctrl_pressed = (key[KEY_LCONTROL]) || (key[KEY_RCONTROL]);
	kbd_jump_pressed = key[KEY_J];
	kbd_escape_pressed = key[KEY_ESC];

	kbd_dock_pressed = key[KEY_C];
	kbd_d_pressed = key[KEY_D];
	kbd_o_pressed = key[KEY_O];
	kbd_find_pressed = key[KEY_F];

	kbd_fire_missile_pressed = key[KEY_M];
	kbd_target_missile_pressed = key[KEY_T];
	kbd_unarm_missile_pressed = key[KEY_U];

	kbd_pause_pressed = key[KEY_P];
	kbd_resume_pressed = key[KEY_R];

	kbd_inc_speed_pressed = key[KEY_SPACE];
	kbd_dec_speed_pressed = key[KEY_SLASH];

	kbd_up_pressed = key[KEY_S] || key[KEY_UP];
	kbd_down_pressed = key[KEY_X] || key[KEY_DOWN];
	kbd_left_pressed = key[KEY_COMMA] || key[KEY_LEFT];
	kbd_right_pressed = key[KEY_STOP] || key[KEY_RIGHT];

	kbd_enter_pressed = key[KEY_ENTER];
	kbd_backspace_pressed = key[KEY_BACKSPACE];
	kbd_space_pressed = key[KEY_SPACE];

#ifdef _DEBUG
	kbd_dbg_pressed = key[KEY_B];
#endif

	while (keypressed())
		readkey();
}

int gmlbKeyboardReadKey()
{
	int keynum = readkey();
	int keycode = keynum >> 8;
	int keyasc = keynum & 255;

	if (keycode == KEY_ENTER)
		keyasc = 0x0A;

	return keyasc;
}
#pragma endregion

GmlbJoystick joystick;

int gmlbJoystickInit()
{
	int rv = 0;
	if (install_joystick(JOY_TYPE_AUTODETECT) == 0)
		rv = (num_joysticks > 0);			// joystick is optional

	return rv;
}

void gmlbJoystickPoll()
{
	memset(&joystick, 0, sizeof(GmlbJoystick));

	poll_joystick();

	if (joy[0].stick[0].axis[1].d1)
		joystick.up = 1;
	if (joy[0].stick[0].axis[1].d2)
		joystick.down = 1;
	if (joy[0].stick[0].axis[0].d1)
		joystick.left = 1;;
	if (joy[0].stick[0].axis[0].d2)
		joystick.right = 1;

	if (joy[0].stick[1].axis[1].d1)
		joystick.d_up = 1;
	if (joy[0].stick[1].axis[1].d2)
		joystick.d_down = 1;
	if (joy[0].stick[1].axis[0].d1)
		joystick.d_left = 1;
	if (joy[0].stick[1].axis[0].d2)
		joystick.d_right = 1;

	if (joy[0].button[0].b)
		joystick.fire0 = 1;
	if (joy[0].button[1].b)
		joystick.fire1 = 1;
	if (joy[0].button[2].b)
		joystick.fire2 = 1;
	if (joy[0].button[3].b)
		joystick.fire3 = 1;
	if (joy[0].button[4].b)
		joystick.fire4 = 1;
	if (joy[0].button[5].b)
		joystick.fire5 = 1;
	if (joy[0].button[7].b)
		joystick.fire7 = 1;
	if (joy[0].button[9].b)
		joystick.fire9 = 1;
}

/////////////////////////////////////////////////////////////////////////////

#pragma region Graphics
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

void gmlbGraphicsSetXorMode(int i)
{
	xor_mode(i);
}
#pragma endregion

/////////////////////////////////////////////////////////////////////////////

#pragma region Audio
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
#pragma endregion

/////////////////////////////////////////////////////////////////////////////

char *gmlbFileNameFromPath(const char *path)
{
	return get_filename(path);
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
