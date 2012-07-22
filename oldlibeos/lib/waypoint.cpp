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

Waypoint::Waypoint()
{
	objectstate = NWN_OBJECT_INSTANCE;
	objectid = INVALID_OBJECT_ID;
	hasmapnote = mapnoteenabled = false;
	appearance = paletteid = 0;
	x = y = z = dirx = diry = 0.0;
}

int Waypoint::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Waypoint::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Waypoint::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"HasMapNote", "MapNoteEnabled", "Appearance", "PaletteID", "ObjectId",
		 "XPosition", "YPosition", "ZPosition", "XOrientation", "YOrientation",
		 "Tag", "Comment", "TemplateResRef", "LocalizedName", "Description",
		 "MapNote", "ActionList", "VarTable"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) hasmapnote = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) mapnoteenabled = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) appearance = top.values[n[2]];
	if(n[3] != NOTFOUND) paletteid = top.values[n[3]];
	if(n[4] != NOTFOUND) objectid = top.values[n[4]];
	if(n[5] != NOTFOUND) x = *((float32*) &top.values[n[5]]);
	if(n[6] != NOTFOUND) y = *((float32*) &top.values[n[6]]);
	if(n[7] != NOTFOUND) z = *((float32*) &top.values[n[7]]);
	if(n[8] != NOTFOUND) dirx = *((float32*) &top.values[n[8]]);
	if(n[9] != NOTFOUND) diry = *((float32*) &top.values[n[9]]);
	if(n[10] != NOTFOUND)
		if(gff.getExoString(n[10], top, tag)) return ERR(gff);
	if(n[11] != NOTFOUND)
		if(gff.getExoString(n[11], top, comment)) return ERR(gff);
	if(n[12] != NOTFOUND)
		if(gff.getResRef(n[12], top, templateresref)) return ERR(gff);
	if(n[13] != NOTFOUND)
		if(gff.getExoLocString(n[13], top, name)) return ERR(gff);
	if(n[14] != NOTFOUND)
		if(gff.getExoLocString(n[14], top, description)) return ERR(gff);
	if(n[15] != NOTFOUND)
		if(gff.getExoLocString(n[15], top, mapnote)) return ERR(gff);
	if(n[16] != NOTFOUND) if(getActions(gff, n[16], top)) return errcode;
	if(n[17] != NOTFOUND)
		if(variables.load(gff, n[17], top)) return ERR(variables);
	checkRanges();

	return errcode = 0;
}

int Waypoint::getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(aln, top, scts)) return ERR(gff);
	actions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(actions[i].load(gff, scts[i])) return ERR(actions[i]);
	return errcode = 0;
}

void Waypoint::checkRanges(void)
{
	if(tag.length() > 32) tag.erase(32);
}

int Waypoint::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTW;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Waypoint::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTW;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Waypoint::save(GFFFile &gff, uint32 top)
{
	uint32 list;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "HasMapNote", hasmapnote))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "MapNoteEnabled", mapnoteenabled))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Appearance", appearance))
		return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeExoString(top, "LinkedTo", "")) return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocalizedName", name)) return ERR(gff);
	if(gff.writeExoLocString(top, "Description", description)) return ERR(gff);
	if(gff.writeExoLocString(top, "MapNote", mapnote)) return ERR(gff);

	switch(objectstate)
	{
		case NWN_OBJECT_BLUEPRINT:
			if(gff.writeVar(top, NWN_VAR_INT8, "PaletteID", paletteid))
				return ERR(gff);
			if(gff.writeExoString(top, "Comment", comment)) return ERR(gff);
			break;
		case NWN_OBJECT_SAVE:
			if(gff.writeVar(top, NWN_VAR_UINT32, "ObjectId", objectid))
				return ERR(gff);
			if(writeActions(gff, top)) return errcode;
		case NWN_OBJECT_INSTANCE:
			if(gff.writeFloat(top, "XPosition", x)) return ERR(gff);
			if(gff.writeFloat(top, "YPosition", y)) return ERR(gff);
			if(gff.writeFloat(top, "ZPosition", z)) return ERR(gff);
			if(gff.writeFloat(top, "XOrientation", dirx)) return ERR(gff);
			if(gff.writeFloat(top, "YOrientation", diry)) return ERR(gff);
			break;
	}
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Waypoint::writeActions(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "ActionList")) return ERR(gff);
	for(uint32 i=0;i<actions.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(actions[i].save(gff, sct)) return ERR(actions[i]);
	}
	return errcode = 0;
}

// vim:tabstop=2
