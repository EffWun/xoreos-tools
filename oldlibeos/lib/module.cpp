/* libeos 0.0.1
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <eos.h>

void printmemdata(const char *str)
{
	char buf[4096], name[256];
	FILE *f;

	snprintf(name, 256, "/proc/%d/status", getpid());
	if(!(f = fopen(name, "r"))) return;
	while(!feof(f))
	{
		fgets(buf, 4096, f);
		if(!strncmp(buf, "VmData:", 7))
			fprintf(stderr, "%s %s", str, buf);
	}
}

int Module::load(std::string filen)
{
	ERFFile mod;
	MemSeg mem;
	uint32 n, j;
	std::vector<uint32> dlgn;
	std::vector<AreaFile> areafiles;

	if(mod.open(filen)) return ERR(mod);
	if(mod.type != NWN_FILE_MOD) return errcode = 11;
	if(mod.init()) return ERR(mod);

	printmemdata("vorIFO");
	fprintf(stderr, "ifo\n");
	if((n = mod.getEntryNum("module", NWN_FILE_IFO)) == NOTFOUND)
			return errcode = 38;
	if(mod.dumpEntryIntoMem(n, mem)) return ERR(mod);
	if(ifo.load(mem)) return ERR(ifo);
	mem.deAlloc();
	printmemdata("nachIFO");

	printmemdata("vorFAC");
	fprintf(stderr, "fac\n");
	if((n = mod.getEntryNum("repute", NWN_FILE_FAC)) == NOTFOUND)
			return errcode = 39;
	if(mod.dumpEntryIntoMem(n, mem)) return ERR(mod);
	if(factions.load(mem)) return ERR(factions);
	mem.deAlloc();
	printmemdata("nachFAC");

	printmemdata("vorJRL");
	fprintf(stderr, "jrl\n");
	if((n = mod.getEntryNum("module", NWN_FILE_JRL)) == NOTFOUND)
			return errcode = 39;
	if(mod.dumpEntryIntoMem(n, mem)) return ERR(mod);
	if(journal.load(mem)) return ERR(journal);
	mem.deAlloc();
	printmemdata("nachJRL");

	printmemdata("vorRES");
	fprintf(stderr, "resss\n");
	for(uint32 i=0;i<mod.restypes.size();i++)
		switch(mod.restypes[i])
		{
			case NWN_FILE_DLG:
				dlgn.push_back(i);
				break;
			case NWN_FILE_ARE:
				for(j=0;j<areafiles.size();j++)
					if(areafiles[j].resref == mod.resrefs[i]) break;
				if(j == areafiles.size())
				{
					areafiles.resize((j=areafiles.size())+1);
					areafiles[j].are = areafiles[j].git = areafiles[j].gic = 0xFFFFFFFF;
					areafiles[j].resref = mod.resrefs[i];
				}
				areafiles[j].are = i;
				break;
			case NWN_FILE_GIT:
				for(j=0;j<areafiles.size();j++)
					if(areafiles[j].resref == mod.resrefs[i]) break;
				if(j == areafiles.size())
				{
					areafiles.resize((j=areafiles.size())+1);
					areafiles[j].are = areafiles[j].git = areafiles[j].gic = 0xFFFFFFFF;
					areafiles[j].resref = mod.resrefs[i];
				}
				areafiles[j].git = i;
				break;
			case NWN_FILE_GIC:
				for(j=0;j<areafiles.size();j++)
					if(areafiles[j].resref == mod.resrefs[i]) break;
				if(j == areafiles.size())
				{
					areafiles.resize((j=areafiles.size())+1);
					areafiles[j].are = areafiles[j].git = areafiles[j].gic = 0xFFFFFFFF;
					areafiles[j].resref = mod.resrefs[i];
				}
				areafiles[j].gic = i;
				break;
			default:
				break;
		}

	printmemdata("vorAREAS");
	fprintf(stderr, "areas!\n");
	areas.resize(areafiles.size());
	for(uint32 i=0;i<areafiles.size();i++)
	{
		MemSeg are, git, gic;
		if(areafiles[i].are != 0xFFFFFFFF)
			if(mod.dumpEntryIntoMem(areafiles[i].are, are)) return ERR(mod);
		if(areafiles[i].git != 0xFFFFFFFF)
			if(mod.dumpEntryIntoMem(areafiles[i].git, git)) return ERR(mod);
		if(areafiles[i].gic != 0xFFFFFFFF)
			if(mod.dumpEntryIntoMem(areafiles[i].gic, gic)) return ERR(mod);
		if(areas[i].load(are, git, gic)) return ERR(areas[i]);
	}
	printmemdata("vorDIALOGS");
	fprintf(stderr, "dialogs!\n");
	dialogs.resize(dlgn.size());
	for(uint32 i=0;i<dlgn.size();i++)
	{
		if(mod.dumpEntryIntoMem(dlgn[i], mem)) return ERR(mod);
		if(dialogs[i].load(mem)) return ERR(dialogs[i]);
		mem.deAlloc();
	}
	printmemdata("nachDIALOGS");
	return errcode = 0;
}

int Module::save(std::string filen)
{
	return errcode = 0;
}

// vim:tabstop=2
