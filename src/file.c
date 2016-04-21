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
 * file.c - serialization of config file and game saves
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elite.h"


/////////////////////////////////////////////////////////////////////////////
#ifdef WINDOWS
static const char *configFile = "config\\elite_adv.cfg";
#else
static const char *configFile = "config/elite_adv.cfg";
#endif
/////////////////////////////////////////////////////////////////////////////
int write_config_file()
{
	FILE *fp = fopen(configFile, "w");
	if (fp == NULL)
		return -1;

	int warn = 0;

	if (fprintf(fp, "%d\t\t\t# Game Speed, the lower the number the faster the game.\n", speed_cap) < 0)
		warn = (warn == 0) ? 1 : warn;
	if (fprintf(fp, "%d\t\t\t# Graphics: 0 = Solid, 1 = Wireframe\n", wireframe))
		warn = (warn == 0) ? 2 : warn;
	if (fprintf(fp, "%d\t\t\t# Anti-Alias Wireframe: 0 = Normal, 1 = Anti-Aliased\n", anti_alias_gfx))
		warn = (warn == 0) ? 3 : warn;
	if (fprintf(fp, "%d\t\t\t# Planet style: 0 = Wireframe, 1 = Green, 2 = Fancy\n", planet_render_style))
		warn = (warn == 0) ? 4 : warn;
	if (fprintf(fp, "%d\t\t\t# Planet Descriptions: 0 = Tree Grubs, 1 = Hoopy Casinos\n", hoopy_casinos))
		warn = (warn == 0) ? 5 : warn;
	if (fprintf(fp, "%d\t\t\t# Instant dock: 0 = off, 1 = on\n", instant_dock))
		warn = (warn == 0) ? 6 : warn;
	if (fprintf(fp, "%d\t\t\t# DirectX: 0 = off (use GDI), 1 = on\n", directx))
		warn = (warn == 0) ? 7 : warn;
	if (fprintf(fp, "%d\t\t\t# x360 controller: 0 = no, 1 = yes - extra controls :)\n", x360_controller))
		warn = (warn == 0) ? 8 : warn;

	fclose(fp);

	return warn;
}

/// Read a line; ignore blanks and comments, and strip white space.
static void read_cfg_line(char *str, int max_size, FILE *fp)
{
	char *s;

	do
	{	
		str = fgets(str, max_size, fp);
		for (s = str; *s; s++)					/* End of line at LF or # */
		{
			if ((*s == '\n') || (*s == '#'))
			{
				*s = '\0';
				break;
			} 		
		}
		if (s != str)							/* Trim any trailing white space */
		{
			s--;
			while (isspace(*s))
			{
				*s = '\0';
				if (s == str)
					break;
				s--;
			}
		}
	} while (*str == '\0');
}
int read_config_file()
{
	FILE *fp;
	char str[128];
	
	fp = fopen (configFile, "r");
	if (fp == NULL)
		return -1;

	int warn = 0;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &speed_cap) != 1)
		warn = (warn == 0) ? 1 : warn;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &wireframe) != 1)
		warn = (warn == 0) ? 2 : warn;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &anti_alias_gfx) != 1)
		warn = (warn == 0) ? 3 : warn;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &planet_render_style) != 1)
		warn = (warn == 0) ? 4 : warn;
	
	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &hoopy_casinos) != 1)
		warn = (warn == 0) ? 5 : warn;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &instant_dock) != 1)
		warn = (warn == 0) ? 6 : warn;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &directx) != 1)
		warn = (warn == 0) ? 7 : warn;

	read_cfg_line(str, sizeof(str), fp);
	if (sscanf(str, "%d", &x360_controller) != 1)
		warn = (warn == 0) ? 8 : warn;

	fclose(fp);

#ifndef WINDOWS
	directx = 0;
#endif

	return warn;
}

/////////////////////////////////////////////////////////////////////////////

static int checksum(unsigned char *block)
{
	int acc = 0x49;
	int carry = 0;
	for (int i = 0x49; i > 0; i--)
	{
		acc += block[i-1] + carry;
		carry = acc >> 8;
		acc &= 255;
		acc ^= block[i];
	}
	return acc;
}

int save_commander_file(const char *path)
{
	FILE *fp = fopen(path, "wb");
	if (fp == NULL)
		return 1;
	
	unsigned char block[256];
	block[0x00]  = cmdr.mission;
	block[0x01]  = docked_planet.d;
	block[0x02]  = docked_planet.b;
	block[0x03]  = cmdr.galaxy.a;
	block[0x04]  = cmdr.galaxy.b;
	block[0x05]  = cmdr.galaxy.c;
	block[0x06]  = cmdr.galaxy.d;
	block[0x07]  = cmdr.galaxy.e;
	block[0x08]  = cmdr.galaxy.f;
	block[0x09]  = (cmdr.credits >> 24) & 255;
	block[0x0a] = (cmdr.credits >> 16) & 255;
	block[0x0b] = (cmdr.credits >> 8) & 255;
	block[0x0b] = cmdr.credits & 255;
	block[0x0d] = cmdr.fuel;
	block[0x0e] = 4;
	block[0x0f] = cmdr.galaxy_number;
	block[0x10] = cmdr.front_laser;
	block[0x11] = cmdr.rear_laser;
	block[0x12] = cmdr.left_laser;
	block[0x13] = cmdr.right_laser;
	block[0x14] = 0;
	block[0x15] = 0;
	block[0x16] = cmdr.cargo_capacity + 2;

	for (int i = 0; i < NO_OF_STOCK_ITEMS; i++)
		block[0x17 + i] = cmdr.current_cargo[i];
	
	block[0x28] = cmdr.ecm ? 255 : 0;
	block[0x29] = cmdr.fuel_scoop ? 255 : 0;
	block[0x2a] = cmdr.energy_bomb ? 0x7F : 0;
	block[0x2b] = cmdr.energy_unit;
	block[0x2c] = cmdr.docking_computer ? 255 : 0;
	block[0x2d] = cmdr.galactic_hyperdrive ? 255 : 0;
	block[0x2e] = cmdr.escape_pod ? 255 : 0;
	block[0x2f] = VERSION_MAJOR;
	block[0x30] = VERSION_MINOR;
	block[0x31] = 0;
	block[0x32] = 0;
	block[0x33] = cmdr.missiles;
	block[0x34] = cmdr.legal_status;
	
	for (int i = 0; i < NO_OF_STOCK_ITEMS; i++)
		block[0x35 + i] = stock_market[i].current_quantity;
	
	block[0x46] = cmdr.market_rnd;
	block[0x47] = cmdr.score & 255;
	block[0x48] = cmdr.score >> 8;
	block[0x49] = 0x20;

	int chk = checksum(block);
	
	block[0x4a] = chk ^ 0xA9;
	block[0x4b] = chk;
	
	block[0x4c] = cmdr.vga_scanner;
	block[0x4d] = cmdr.obc;
	block[0x4e] = cmdr.audio_scanner;

	for (int i = 0x4f; i < 0x100; i++)
		block[i] = 0;

	if (fwrite(block, 256, 1, fp) != 1)
		return 1;
		
	if (fclose(fp) == EOF)
		return 1;	

	return 0;
}

int load_commander_file(const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return 1;

	unsigned char block[256];
	if (fread(block, 256, 1, fp) != 1)
		return 1;

	int chk = checksum(block);
	if ((block[0x4a] != (chk ^ 0xA9)) || (block[0x4b] != chk))
		return 1;
	
	saved_cmdr.mission = block[0x00];

	saved_cmdr.ship_x = block[0x01];
	saved_cmdr.ship_y = block[0x02];
	
	saved_cmdr.galaxy.a = block[0x03];
	saved_cmdr.galaxy.b = block[0x04];
	saved_cmdr.galaxy.c = block[0x05];
	saved_cmdr.galaxy.d = block[0x06];
	saved_cmdr.galaxy.e = block[0x07];
	saved_cmdr.galaxy.f = block[0x08];;
	
	saved_cmdr.credits = block[0x09] << 24;
	saved_cmdr.credits += block[0x0a] << 16;
	saved_cmdr.credits += block[0x0b] << 8;
	saved_cmdr.credits += block[0x0c];

	saved_cmdr.fuel = block[0x0d];

	saved_cmdr.galaxy_number = block[0x0f];
	saved_cmdr.front_laser = block[0x10];
	saved_cmdr.rear_laser = block[0x11];
	saved_cmdr.left_laser = block[0x12];
	saved_cmdr.right_laser = block[0x13];

	saved_cmdr.cargo_capacity = block[0x16] - 2;

	for (int i = 0; i < NO_OF_STOCK_ITEMS; i++)
		saved_cmdr.current_cargo[i] = block[0x17 + i];
	
	saved_cmdr.ecm = block[0x28];
	saved_cmdr.fuel_scoop = block[0x29];
	saved_cmdr.energy_bomb = block[0x2a];
	saved_cmdr.energy_unit = block[0x2b];
	saved_cmdr.docking_computer = block[0x2c];
	saved_cmdr.galactic_hyperdrive = block[0x2d];
	saved_cmdr.escape_pod = block[0x2e];
	saved_cmdr.version_major = block[0x2f];
	saved_cmdr.version_minor = block[0x30];
	saved_cmdr.missiles = block[0x33];
	saved_cmdr.legal_status = block[0x34];
	
	for (int i = 0; i < NO_OF_STOCK_ITEMS; i++)
		saved_cmdr.station_stock[i] = block[0x35 + i];
	
	saved_cmdr.market_rnd = block[0x46];

	saved_cmdr.score = block[0x47];
	saved_cmdr.score += block[0x48] << 8;

	saved_cmdr.vga_scanner = block[0x4c];
	saved_cmdr.obc = block[0x4d];
	saved_cmdr.audio_scanner = block[0x4e];

	if (fclose(fp) == EOF)
		return 1;	

	return 0;
}
