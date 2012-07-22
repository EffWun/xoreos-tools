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

int Journal::load(std::string fname)
{
	GFFFile gff;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Journal::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Journal::load(MemSeg &memseg)
{
	return load(memseg.mem, memseg.size);
}

int Journal::load(GFFFile &gff, GFFFile::Struct &top)
{
	uint32 n;

	aderrstr = "Categories";
	if((n = gff.getIdxByLabel(aderrstr, top)) == ERROR) return ERR(gff);
	if(n != NOTFOUND) if(getCategories(gff, n, top)) return errcode;

	return errcode = 0;
}

int Journal::getCategories(GFFFile &gff, uint32 cln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Priority", "XP", "Tag", "Comment", "Name", "EntryList"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(cln, top, scts)) return ERR(gff);
	categories.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		categories[i].priority = JOURNAL_PRIO_MEDIUM;
		categories[i].xp = 0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) categories[i].priority = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) categories[i].xp = scts[i].values[n[1]];
		if(n[2] != NOTFOUND)
			if(gff.getExoString(n[2], scts[i], categories[i].tag)) return ERR(gff);
		if(n[3] != NOTFOUND)
			if(gff.getExoString(n[3], scts[i], categories[i].comment)) return ERR(gff);
		if(n[4] != NOTFOUND)
			if(gff.getExoLocString(n[4], scts[i], categories[i].name)) return ERR(gff);
		if(n[5] != NOTFOUND)
			if(getEntries(gff, n[5], scts[i], categories[i].entries)) return errcode;
	}
	return errcode = 0;
}

int Journal::getEntries(GFFFile &gff, uint32 eln, GFFFile::Struct &top,
		std::vector<Entry> &entries)
{
	std::string tlabels[] = {"End", "ID", "Text"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(eln, top, scts)) return ERR(gff);
	entries.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		entries[i].end = false;
		entries[i].id = 0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) entries[i].end = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) entries[i].id = scts[i].values[n[1]];
		if(n[2] != NOTFOUND)
			if(gff.getExoLocString(n[2], scts[i], entries[i].text)) return ERR(gff);
	}
	return errcode = 0;
}

int Journal::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_JRL;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Journal::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_JRL;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Journal::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "Categories")) return ERR(gff);
	for(uint32 i=0;i<categories.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "Picture", 0xFFFF)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "Priority", categories[i].priority))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "XP", categories[i].xp))
			return ERR(gff);
		if(gff.writeExoString(sct, "Tag", categories[i].tag)) return ERR(gff);
		if(gff.writeExoString(sct, "Comment", categories[i].comment))
			return ERR(gff);
		if(gff.writeExoLocString(sct, "Name", categories[i].name)) return ERR(gff);
		if(writeEntries(gff, sct, categories[i].entries)) return errcode;
	}
	return errcode = 0;
}

int Journal::writeEntries(GFFFile &gff, uint32 top, std::vector<Entry> &entries)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "EntryList")) return ERR(gff);
	for(uint32 i=0;i<entries.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "End", entries[i].end))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "ID", entries[i].id)) return ERR(gff);
		if(gff.writeExoLocString(sct, "Text", entries[i].text)) return ERR(gff);
	}
	return errcode = 0;
}

// vim:tabstop=2
