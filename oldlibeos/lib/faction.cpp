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

int Faction::load(std::string fname)
{
	GFFFile gff;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Faction::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Faction::load(MemSeg &memseg)
{
	return load(memseg.mem, memseg.size);
}

int Faction::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] = {"FactionList", "RepList"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) if(getFactions(gff, n[0], top)) return errcode;
	if(n[1] != NOTFOUND) if(getReputations(gff, n[1], top)) return errcode;

	return errcode = 0;
}

int Faction::getFactions(GFFFile &gff, uint32 fln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"FactionGlobal", "FactionParentID", "FactionName"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(fln, top, scts)) return ERR(gff);
	factions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		factions[i].global = false;
		factions[i].parentid = 0xFFFFFFFF;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) factions[i].global = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) factions[i].parentid = scts[i].values[n[1]];
		if(n[2] != NOTFOUND)
			if(gff.getExoString(n[2], scts[i], factions[i].name)) return ERR(gff);
	}
	return errcode = 0;
}

int Faction::getReputations(GFFFile &gff, uint32 rln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"FactionID1", "FactionID2", "FactionRep"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;
	uint32 id1, id2;

	reputations.resize(factions.size());
	for(uint32 i=0;i<factions.size();i++)
	{
		reputations[i].resize(factions.size());
		for(uint32 j=0;j<factions.size();j++) reputations[i][j] = 100;
	}
	if(gff.getList(rln, top, scts)) return ERR(gff);
	for(uint32 i=0;i<scts.size();i++)
	{
		id1 = id2 = 0xFFFFFFFF;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) id1 = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) id2 = scts[i].values[n[1]];
		if((id1 == 0xFFFFFFFF) || (id2 == 0xFFFFFFFF)) continue;
		if((id2 < reputations.size()) && (id1 < reputations[id2].size()))
			if(n[2] != NOTFOUND) reputations[id2][id1] = scts[i].values[n[2]];
	}
	return errcode = 0;
}

int Faction::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_FAC;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Faction::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_FAC;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Faction::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "FactionList")) return ERR(gff);
	for(uint32 i=0;i<factions.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "FactionGlobal", factions[i].global))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "FactionParentID",
					factions[i].parentid))
			return ERR(gff);
		if(gff.writeExoString(sct, "FactionName", factions[i].name))
			return ERR(gff);
	}
	if(gff.writeList(list, top, "RepList")) return ERR(gff);
	for(uint32 i=0;i<reputations.size();i++)
		for(uint32 j=0;j<reputations[i].size();j++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, i*reputations.size()+j)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "FactionID1", j)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "FactionID2", i)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "FactionRep", reputations[i][j]))
				return ERR(gff);
		}
	return errcode = 0;
}

// vim:tabstop=2
