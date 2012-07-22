/* libeos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <eos.h>

int GameDefaults::load(std::string basedir)
{
	std::string keyfnames[] =
		{"xp2patch.key", "xp2.key", "xp1patch.key", "xp1.key", "chitin.key"};
	std::vector<KEYFile> keys;
	Finder finder;

	finder.load = NULL;
	this->basedir = basedir;
	for(uint32 i=0;i<(sizeof(keyfnames)/sizeof(std::string));i++)
	{
		uint32 n;
		int r;

		finder.keys.resize((n=finder.keys.size())+1);
		r = finder.keys[n].open(basedir + "/" + keyfnames[i]);
		if(r && (keyfnames[i] != "chitin.key"))
		{
			finder.keys.resize(finder.keys.size()-1);
			continue;
		}
		else if(r && (keyfnames[i] == "chitin.key")) return ERR(finder.keys[n]);
		if(finder.keys[n].init()) return ERR(finder.keys[n]);
	}

	if(load2da(finder)) return errcode;
	return errcode = 0;
}

int GameDefaults::findRes(Finder &finder)
{
	for(uint32 i=0;i<finder.keys.size();i++)
	{
		for(uint32 j=0;j<finder.keys[i].bifcount;j++)
		{
			uint32 n=finder.bifs.size();
			finder.bifs.push_back(finder.keys[i].fnames[j]);
			finder.founds.resize(n+1);
			for(uint32 k=0;k<finder.keys[i].getNumResources(j);k++)
			{
				std::string resref;
				NWNFileType type;
				uint32 idx;
				if(finder.keys[i].getKeyEntry(j, k, resref, type, idx))
					return ERR(finder.keys[i]);
				if(type != finder.restype) continue;
				for(uint32 l=0;l<finder.resnames.size();l++)
				{
					if(resref == finder.resnames[l])
					{
						uint32 m;
						finder.founds[n].resize((m=finder.founds[n].size())+1);
						finder.founds[n][m].n = idx;
						finder.founds[n][m].strn = finder.residx[l];
						finder.resnames.erase(finder.resnames.begin()+l);
						finder.residx.erase(finder.residx.begin()+l);
						l--;
					}
				}
			}
			if(finder.founds[n].empty())
			{
				finder.founds.resize(n);
				finder.bifs.resize(n);
			}
		}
	}
	return errcode = 0;
}

int GameDefaults::loadRes(Finder &finder)
{
	for(uint32 i=0;i<finder.bifs.size();i++)
	{
		BIFFile bif;

		if(bif.open(basedir + "/" + finder.bifs[i])) return ERR(bif);
		if(bif.init()) return ERR(bif);
		for(uint32 j=0;j<finder.founds[i].size();j++)
		{
			uint32 n;
			MemSeg mem;
			if(bif.dumpEntryIntoMem(finder.founds[i][j].n, mem)) return ERR(bif);
			if(finder.load)
				if((this->*finder.load)(mem, finder.founds[i][j].strn)) return errcode;
		}
	}
	return errcode = 0;
}

int GameDefaults::load2da(Finder &finder)
{
	std::string strs[] =
		{"gender", "racialtypes", "classes", "appearance", "baseitems", "armor",
		 "bodybag", "creaturesize", "creaturespeed", "domains", "feat",
		 "itempropdef", "itemprops", "itemvalue", "masterfeats", "metamagic",
		 "phenotype", "portraits", "skills", "spellschools", "spells"};

	finder.restype = NWN_FILE_2DA;
	finder.load = &GameDefaults::load2daMem;
	finder.resnames.resize(sizeof(strs)/sizeof(std::string));
	finder.residx.resize(finder.resnames.size());
	for(uint32 i=0;i<(sizeof(strs)/sizeof(std::string));i++)
		finder.resnames[finder.residx[i] = i] = strs[i];
	if(findRes(finder)) return errcode;
	if(loadRes(finder)) return errcode;
	clearFinder(finder);
	return errcode = 0;
}

int GameDefaults::load2daMem(MemSeg &mem, uint32 idx)
{
	TwoDAFile *cls[] =
		{&gender, &racialtypes, &classes, &appearance, &baseitems, &armor,
		 &bodybag, &creaturesize, &creaturespeed, &domains, &feat,
		 &itempropdef, &itemprops, &itemvalue, &masterfeats, &metamagic,
		 &phenotype, &portraits, &skills, &spellschools, &spells};

	if(cls[idx]->open(mem)) return ERR((*cls[idx]));
	if(cls[idx]->init()) return ERR((*cls[idx]));
	return errcode = 0;
}

void GameDefaults::clearFinder(Finder &finder)
{
	finder.bifs.clear();
	finder.resnames.clear();
	finder.residx.clear();
	for(uint32 i=0;i<finder.founds.size();i++) finder.founds[i].clear();
	finder.founds.clear();
	finder.load = NULL;
}

// vim:tabstop=2
