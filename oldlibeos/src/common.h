/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
