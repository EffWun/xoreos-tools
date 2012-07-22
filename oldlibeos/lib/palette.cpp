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

Palette::Palette()
{
	restype = NWN_FILE_UNDEFINED;
	palettestate = NWN_PALETTE_SKELETON;
}

int Palette::load(std::string fname)
{
	GFFFile gff;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Palette::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Palette::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] = {"RESTYPE", "TILESETRESREF", "MAIN"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) restype = (NWNFileType) top.values[n[0]];
	if(n[1] != NOTFOUND)
		if(gff.getResRef(n[1], top, tilesetresref)) return ERR(gff);
	if(n[2] != NOTFOUND) if(getNodes(gff, n[2], top, nodes)) return ERR(gff);

	return errcode = 0;
}

int Palette::getNodes(GFFFile &gff, uint32 nln, GFFFile::Struct &top,
		std::vector<TreeNode *> &nodel)
{
	std::string tlabels[] =
		{"TYPE", "ID", "STRREF", "CR", "DELETE_ME",
		 "RESREF", "FACTION", "NAME", "LIST"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(nln, top, scts)) return ERR(gff);
	nodel.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		nodel[i] = new TreeNode;
		nodel[i]->type = nodel[i]->id = 255;
		nodel[i]->strref = 0xFFFFFFFF;
		nodel[i]->cr = -1.0;
		palettestate = (NWNPaletteState) scts[i].id;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) nodel[i]->type = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) nodel[i]->id = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) nodel[i]->strref = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) nodel[i]->cr = *((float32 *) &scts[i].values[n[3]]);
		if(n[4] != NOTFOUND)
			if(gff.getExoString(n[4], scts[i], nodel[i]->deleteme)) return ERR(gff);
		if(n[5] != NOTFOUND)
			if(gff.getResRef(n[5], scts[i], nodel[i]->resref)) return ERR(gff);
		if(n[6] != NOTFOUND)
			if(gff.getExoString(n[6], scts[i], nodel[i]->faction)) return ERR(gff);
		if(n[7] != NOTFOUND)
			if(gff.getExoString(n[7], scts[i], nodel[i]->name)) return ERR(gff);
		if(n[8] != NOTFOUND)
			if(getNodes(gff, n[8], scts[i], nodel[i]->nodes)) return ERR(gff);
	}
	return errcode = 0;
}

int Palette::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_ITP;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Palette::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_ITP;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Palette::save(GFFFile &gff, uint32 top)
{
	if(palettestate == NWN_PALETTE_SKELETON)
	{
		if(gff.writeVar(top, NWN_VAR_UINT16, "RESTYPE", (uint16) restype))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "NEXT_USEABLE_ID", nodes.size()))
			return ERR(gff);
		if(!tilesetresref.empty())
			if(gff.writeResRef(top, "TILESETRESREF", tilesetresref)) return ERR(gff);
	}
	if(writeNodes(gff, top, nodes, "MAIN", (uint32) palettestate)) return errcode;
	return errcode = 0;
}

int Palette::writeNodes(GFFFile &gff, uint32 top, std::vector<TreeNode *> &nodel,
				std::string lname, uint32 sid)
{
	uint32 list, sct;

	if(gff.writeList(list, top, lname)) return ERR(gff);
	for(uint8 i=0;i<nodel.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, sid)) return ERR(gff);
		if(nodel[i]->type != 255)
			if(gff.writeVar(sct, NWN_VAR_INT8, "TYPE", nodel[i]->type))
				return ERR(gff);
		if(nodel[i]->id != 255)
			if(gff.writeVar(sct, NWN_VAR_INT8, "ID", nodel[i]->id))
				return ERR(gff);
		if(nodel[i]->strref != 0xFFFFFFFF)
			if(gff.writeVar(sct, NWN_VAR_UINT32, "STRREF", nodel[i]->strref))
				return ERR(gff);
		if(nodel[i]->cr != -1.0)
			if(gff.writeFloat(sct, "CR", nodel[i]->cr)) return ERR(gff);
		if(!nodel[i]->name.empty())
			if(gff.writeExoString(sct, "NAME", nodel[i]->name)) return ERR(gff);
		if(!nodel[i]->deleteme.empty())
			if(gff.writeExoString(sct, "DELETEME", nodel[i]->deleteme))
				return ERR(gff);
		if(!nodel[i]->faction.empty())
			if(gff.writeExoString(sct, "FACTION", nodel[i]->faction)) return ERR(gff);
		if(!nodel[i]->resref.empty())
			if(gff.writeResRef(sct, "RESREF", nodel[i]->resref)) return ERR(gff);
		if(writeNodes(gff, sct, nodel[i]->nodes, "LIST", sid)) return errcode;
	}
	return errcode = 0;
}

// vim:tabstop=2
