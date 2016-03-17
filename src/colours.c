#include <stdio.h>

#include "gfx.h"


/// Colours in code are now implemented as indices into the
/// global palette, pColours.
///
/// This allows initialisation to configure a "palette of
/// palette-indices" for GDI, or a palette or ARGB for directx,
/// by assignment of pColours to either golours_gdi or
/// colours_dx.


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
const int *pColours = NULL;


static const int colours_gdi[NUM_COLOURS] = {
	  0,  28, 255,  39,  49,  11, 248, 235,
	234, 237,  45,  46, 133,   4,   1,  71,
	242,  37,  39,  89, 160, 251,  76,  77,
	122,   2,  17,  86, 183, 102, 134, 167,
	213,  83, 249,
// Anti-aliasing entries...
	235, 236, 237, 238, 239, 240, 241, 242,

	150,  64, 104, 125,  85,  73,  39,  17,
	 69,  64
};

static const int colours_dx[NUM_COLOURS] = {
	//GFX_COL_BLACK
		0x000000, 0x9C0000, 0xFFFFFF, 0xDEB500, 0xCE0000, 0x00FFCE, 0x808080, 0x777777,
	//GFX_COL_GREY_3
		0x5F5F5F, 0x969696, 0x0149A3, 0x00008C, 0x000099, 0x000080, 0x800000, 0xFF3333,
	//GFX_COL_WHITE_2
		0xE3E3E3, 0xCE9C00, 0xDEB500, 0xFFCC33, 0xFFFF99, 0xFFFF00, 0xCC6633, 0xFF6633,
	//GFX_ORANGE_3
		0xFFCC66, 0x008000, 0x63AD00, 0x66CC33, 0xFF99CC, 0x333366, 0x333399, 0x3333CC,
	//GFX_COL_SNES_213
		0x6699FF, 0xFF9933, 0xFF0000,
	//GFX_COL_AA_0
		0x777777, 0x868686, 0x969696, 0xCBCBCB, 0xB2B2B2, 0xD7D7D7, 0xDDDDDD, 0xE3E3E3,
	//GFX_COL_FUI
		0x3EB489, 0xCC0033, 0x993366, 0x99FF66, 0x33CC33, 0x336633, 0xF0B030, 0xD09010,
	//GFX_COL_BAR_MED2
		0x901010, 0xC41E3A
};


// Palette used by GDI, from scanner.bmp...
/*
[000] 00 00 00  [016] 63 31 00  [032] B5 84 00  [048] DB 6D 00  [064] CC 00 33  [080] 66 99 33  [096] 33 00 66  [112] 00 99 66
[001] 80 00 00  [017] 63 AD 00  [033] BD 8C 00  [049] CE 00 00  [065] FF 00 33  [081] 99 99 33  [097] 66 00 66  [113] 33 99 66
[002] 00 80 00  [018] 63 FF 63  [034] BD BD BD  [050] B0 B0 B0  [066] 00 33 33  [082] CC 99 33  [098] 99 00 66  [114] 66 99 66
[003] 80 80 00  [019] 6B 42 08  [035] C6 C6 C6  [051] 80 40 00  [067] 33 33 33  [083] FF 99 33  [099] CC 00 66  [115] 99 99 66
[004] 00 00 80  [020] 7B 4A 08  [036] CE 00 CE  [052] FF FF 80  [068] 66 33 33  [084] 00 CC 33  [100] FF 00 66  [116] CC 99 66
[005] 80 00 80  [021] 7B 7B 7B  [037] CE 9C 00  [053] 66 CC 00  [069] 99 33 33  [085] 33 CC 33  [101] 00 33 66  [117] FF 99 66
[006] 00 80 80  [022] 84 52 08  [038] D6 A5 00  [054] 99 CC 00  [070] CC 33 33  [086] 66 CC 33  [102] 33 33 66  [118] 00 CC 66
[007] C0 C0 C0  [023] 84 84 84  [039] DE B5 00  [055] CC CC 00  [071] FF 33 33  [087] 99 CC 33  [103] 66 33 66  [119] 33 CC 66
[008] C0 DC C0  [024] 8C 5A 08  [040] EF EF EF  [056] FF CC 00  [072] 00 66 33  [088] CC CC 33  [104] 99 33 66  [120] 99 CC 66
[009] A6 CA F0  [025] 8C AD EF  [041] 60 60 60  [057] 66 FF 00  [073] 33 66 33  [089] FF CC 33  [105] CC 33 66  [121] CC CC 66
[010] 00 21 CE  [026] 8C EF 00  [042] 00 9D 9D  [058] 99 FF 00  [074] 66 66 33  [090] 33 FF 33  [106] FF 33 66  [122] FF CC 66
[011] 00 FF CE  [027] 94 63 00  [043] 85 24 F0  [059] CC FF 00  [075] 99 66 33  [091] 66 FF 33  [107] 00 66 66  [123] 00 FF 66
[012] 42 42 42  [028] 9C 00 00  [044] 00 75 75  [060] 00 00 33  [076] CC 66 33  [092] 99 FF 33  [108] 33 66 66  [124] 33 FF 66
[013] 42 63 00  [029] A5 6B 00  [045] 01 49 A3  [061] 33 00 33  [077] FF 66 33  [093] CC FF 33  [109] 66 66 66  [125] 99 FF 66
[014] 42 63 CE  [030] AD 52 00  [046] 00 00 8C  [062] 66 00 33  [078] 00 99 33  [094] FF FF 33  [110] 99 66 66  [126] CC FF 66
[015] 42 CE 42  [031] AD 7B 00  [047] DF B3 02  [063] 99 00 33  [079] 33 99 33  [095] 00 00 66  [111] CC 66 66  [127] FF 00 CC

[128] CC 00 FF  [144] 33 99 99  [160] FF FF 99  [176] CC 66 CC  [192] 66 FF 99  [208] 99 66 FF  [224] 66 FF CC  [240] D7 D7 D7
[129] 00 99 99	[145] 66 99 99	[161] 00 00 CC	[177] FF 66 99	[193] 99 FF CC	[209] CC 66 FF	[225] 99 FF FF	[241] DD DD DD
[130] 99 33 99	[146] 99 99 99	[162] 33 00 99	[178] 00 99 CC	[194] CC FF CC	[210] FF 66 CC	[226] CC FF FF	[242] E3 E3 E3
[131] 99 00 99	[147] CC 99 99	[163] 66 00 CC	[179] 33 99 CC	[195] FF FF CC	[211] 00 99 FF	[227] FF 66 66	[243] EA EA EA
[132] CC 00 99	[148] FF 99 99	[164] 99 00 CC	[180] 66 99 CC	[196] 33 00 CC	[212] 33 99 FF	[228] 66 FF 66	[244] F1 F1 F1
[133] 00 00 99	[149] 00 CC 99	[165] CC 00 CC	[181] 99 99 CC	[197] 66 00 FF	[213] 66 99 FF	[229] FF FF 66	[245] F8 F8 F8
[134] 33 33 99	[150] 33 CC 99	[166] 00 33 99	[182] CC 99 CC	[198] 99 00 FF	[214] 99 99 FF	[230] 66 66 FF	[246] FF FB F0
[135] 66 00 99	[151] 66 CC 66	[167] 33 33 CC	[183] FF 99 CC	[199] 00 33 CC	[215] CC 99 FF	[231] FF 66 FF	[247] A0 A0 A4
[136] CC 33 99	[152] 99 CC 99	[168] 66 33 CC	[184] 00 CC CC	[200] 33 33 FF	[216] FF 99 FF	[232] 66 FF FF	[248] 80 80 80
[137] FF 00 99	[153] CC CC 99	[169] 99 33 CC	[185] 33 CC CC	[201] 66 33 FF	[217] 00 CC FF	[233] A5 00 21	[249] FF 00 00
[138] 00 66 99	[154] FF CC 99	[170] CC 33 CC	[186] 66 CC CC	[202] 99 33 FF	[218] 33 CC FF	[234] 5F 5F 5F	[250] 00 FF 00
[139] 33 66 99	[155] 00 FF 99	[171] FF 33 CC	[187] 99 CC CC	[203] CC 33 FF	[219] 66 CC FF	[235] 77 77 77	[251] FF FF 00
[140] 66 33 99	[156] 33 FF 99	[172] 00 66 CC	[188] CC CC CC	[204] FF 33 FF	[220] 99 CC FF	[236] 86 86 86	[252] 00 00 FF
[141] 99 66 99	[157] 66 CC 99	[173] 33 66 CC	[189] FF CC CC	[205] 00 66 FF	[221] CC CC FF	[237] 96 96 96	[253] FF 00 FF
[142] CC 66 99	[158] 99 FF 99	[174] 66 66 99	[190] 00 FF CC	[206] CE 00 00	[222] FF CC FF	[238] CB CB CB	[254] 00 FF FF
[143] FF 33 99	[159] CC FF 99	[175] 99 66 CC	[191] 33 FF CC	[207] 66 66 CC	[223] 33 FF FF	[239] B2 B2 B2	[255] FF FF FF
*/


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void setColours(int use_directx)
{
	if (use_directx == 1)
		pColours = colours_dx;
	else
		pColours = colours_gdi;
}
