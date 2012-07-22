/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#define GFFTAGS 17
#define ERFTAGS 3
#define KEYTAGS 2
#define BIFTAGS 2
#define SSFTAGS 1
#define KEYBIFTAGS 3
#define TLKTAGS 1
#define LANGS 12

#include <fstream>
#include "xmlstuff.h"

bool printerr(std::string error);
bool printwarn(std::string error);
bool printnwnerr(NWNFile &nwn);
bool init(NWNFile &nwn, const char *fname, bool silent=false);
bool init(NWNFile &nwn, uint8 *mem, uint32 size);
bool deInit(NWNFile &nwn);
void doindent(std::ofstream &f, int indent);
void replace(std::string &haystack, std::string needle, std::string replacer);
std::string quotestr(std::string str);
std::string unquotestr(std::string str);
bool checkrecursion(int sct, std::vector<int> &scts);
std::string rsubstr(std::string str, int n);
void printxmlpos(XMLFile &xml);
bool printxmlerr(XMLFile &xml, std::string error);
bool checkwerr(std::ofstream &f);
int getpos(char *haystack[], int haystacksize, std::string needle, int hayoff=0);
char *getbasename(char *filename);

#endif // _COMMON_H_

// vim:tabstop=2
