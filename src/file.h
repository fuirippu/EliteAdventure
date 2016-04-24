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
 * file.h - serialization of config file and game saves
 */

#ifndef FILE_H
#define FILE_H


extern const int kFileConfigValueAltered;


int write_config_file();
int read_config_file();

int save_commander_file(const char *path);
int load_commander_file(const char *path);


#endif      // #ifndef FILE_H
