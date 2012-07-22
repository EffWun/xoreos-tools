/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <cstdio>
#include <cerrno>
#include <fstream>
#include <eos.h>
#include "xmlstuff.h"
#include "common.h"
#include "info.h"
#include "extract.h"
#include "explode.h"
#include "assemble.h"

#include <cstdlib>

extern char *langs[];
extern std::string basedir;
extern int numdata;

void disphelp(char *fname);
bool teststuff(void);
void traversedlg(Dialog::Entry *entry, uint32 in, TLKFile &tlk, std::string act="", bool end=false);
bool eval(int mode, int n, int argc, char **argv);
bool (*modefuncs[])(char *) =
	{infonwn, infoerf, infotwoda, infokey, infobif, infogff, extracterf,
	 explodegff, assemblegff, explodeerf, assembleerf, explodebif, explodekey,
	 assemblekey, assemblebif, infossf, explodessf, assemblessf, explodekeybif,
	 assemblekeybif, infotlk, explodetlk, assembletlk};

int main(int argc, char **argv)
{
	if(argc < 3) disphelp(getbasename(argv[0]));
	return eval(-1, 1, argc, argv)-1;
}

bool eval(int mode, int n, int argc, char **argv)
{
	if(n >= argc) disphelp(getbasename(argv[0]));
	if(mode == -1) {
		if(!strcmp(argv[n], "info-nwn")) return eval(0, 2, argc, argv);
		else if(!strcmp(argv[n], "info-erf")) return eval(1, 2, argc, argv);
		else if(!strcmp(argv[n], "info-2da")) return eval(2, 2, argc, argv);
		else if(!strcmp(argv[n], "info-key")) return eval(3, 2, argc, argv);
		else if(!strcmp(argv[n], "info-bif")) return eval(4, 2, argc, argv);
		else if(!strcmp(argv[n], "info-gff")) return eval(5, 2, argc, argv);
		else if(!strcmp(argv[n], "extract-erf")) return eval(6, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-gff")) return eval(7, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-gff")) return eval(8, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-erf")) return eval(9, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-erf")) return eval(10, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-bif")) return eval(11, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-key")) return eval(12, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-key")) return eval(13, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-bif")) return eval(14, 2, argc, argv);
		else if(!strcmp(argv[n], "info-ssf")) return eval(15, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-ssf")) return eval(16, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-ssf")) return eval(17, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-keybif")) return eval(18, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-keybif")) return eval(19, 2, argc, argv);
		else if(!strcmp(argv[n], "info-tlk")) return eval(20, 2, argc, argv);
		else if(!strcmp(argv[n], "explode-tlk")) return eval(21, 2, argc, argv);
		else if(!strcmp(argv[n], "assemble-tlk")) return eval(22, 2, argc, argv);
		else disphelp(getbasename(argv[0]));
	}
	for(int i=n;i<argc;i++)
		if(!strcmp(argv[i], "--basedir"))
		{
			if(i >= (argc-1)) disphelp(getbasename(argv[0]));
			else basedir = argv[++i];
			if(basedir[basedir.length()-1] != '/') basedir += "/";
		}
		else if(!modefuncs[mode](argv[i])) return false;
	return true;
}

void disphelp(char *fname)
{
	std::cout << "Usage: " << fname << " <mode> [options] <file> [<...>]\n\n";
	std::cout << "Modes:\n";
	std::cout << "info-nwn	Generic NWN file informations\n";
	std::cout << "info-2da	2DA file informations\n";
	std::cout << "info-erf	ERF file informations\n";
	std::cout << "info-key	KEY file informations\n";
	std::cout << "info-bif	BIF file informations\n";
	std::cout << "info-gff	GFF file informations\n";
	std::cout << "info-ssf	SSF file informations\n";
	std::cout << "info-tlk	TLK file informations\n";
	std::cout << "extract-erf	Extract a ERF file\n";
	std::cout << "explode-erf	Explode a ERF file recursively into hand-editable files\n";
	std::cout << "explode-key	Explode a KEY file into a hand-editable file\n";
	std::cout << "explode-bif	Explode a BIF file recursively into hand-editable files\n";
	std::cout << "explode-keybif	Explode a KEY and its BIFs recursively into hand-editable files\n";
	std::cout << "explode-gff	Explode a GFF file into hand-editable files\n";
	std::cout << "explode-ssf	Explode a SSF file into a hand-editable file\n";
	std::cout << "explode-tlk	Explode a TLK file into a hand-editable file\n";
	std::cout << "assemble-erf	Assemble previously exploded ERF\n";
	std::cout << "assemble-key	Assemble previously exploded KEY\n";
	std::cout << "assemble-bif	Assemble previously exploded BIF\n";
	std::cout << "assemble-keybif	Assemble previously exploded KEY and its BIFs\n";
	std::cout << "assemble-gff	Assemble previously exploded GFF\n";
	std::cout << "assemble-ssf	Assemble previously exploded SSF\n";
	std::cout << "assemble-tlk	Assemble previously exploded TLK\n";
	std::cout << "\nOptions:\n";
	std::cout << "--basedir <dir>	Set the base directory\n";
	std::cout << "               	(Used for explode-keybif and assemble-keybif)\n";
	exit(-1);
}

/*
void traversedlg(Dialog::Entry *entry, uint32 in, TLKFile &tlk, std::string act, bool end)
{
	static bool inited=false;

	if(!entry)
	{
		fprintf(stderr, "[1m[31mNULL![0m\n");
		return;
	}
	for(uint32 i=0;i<in;i++) printf(" ");
	printf("- %d %d:", entry->pc, entry->id);
	if(entry->text.stringref != 4294967295U)
	{
		if(!inited)
		{
			tlk.deInit();
			inited = init(tlk, NWNBASEDIR "dialog.tlk", true);
		}
		if(inited)
		{
			std::string tmp;
			if(tlk.getString(entry->text.stringref, tmp))
				printf("%s", tlk.getStrError().c_str());
			else printf("%s", tmp.c_str());
		}
	}
	else printf("%s", entry->text.str[0].c_str());
	if(!entry->script.empty()) printf(" (%s)", entry->script.c_str());
	if(act != "") printf(" {%s}", act.c_str());
	if(end)
	{
		printf("  [1m[32m\\v[0m\n");
		return;
	}
	else printf("\n");
	for(uint32 i=0;i<entry->replies.size();i++)
		traversedlg(entry->replies[i].child, in+2, tlk, entry->replies[i].active,
				entry->replies[i].link);
}
*/

// vim:tabstop=2
