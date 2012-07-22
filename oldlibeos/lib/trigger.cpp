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

Trigger::Trigger()
{
	objectstate = NWN_OBJECT_INSTANCE;
	objectid = INVALID_OBJECT_ID;
	trapflag = traponeshot = trapdetectable = trapdisarmable = false;
	cursor = traptype = faction = paletteid = loadscreenid = portraitid = 0;
	highlightheight = x = y = z = dirx = diry = 0.0;
	trapdetectdc = trapdisarmdc = 1;
	type = TRIGGER_TYPE_GENERIC;
	linkedtoflags = TRIGGER_LINK_NOTHING;
	creatorid = INVALID_OBJECT_ID;
}

int Trigger::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Trigger::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Trigger::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"TrapFlag", "TrapOneShot", "TrapDetectable", "TrapDisarmable", "Type",
		 "Cursor", "PaletteID", "LinkedToFlags", "TrapType", "TrapDetectDC",
		 "DisarmDC", "LoadScreenID", "PortraitId", "CreatorId", "Faction",
		 "ObjectId", "HighlightHeight", "XPosition", "YPosition", "ZPosition",
		 "XOrientation", "YOrientation", "Tag", "Comment", "LinkedTo", "OnClick",
		 "OnDisarm", "OnTrapTriggered", "ScriptHeartbeat", "ScriptOnEnter",
		 "ScriptOnExit", "ScriptUserDefine", "TemplateResRef", "LocalizedName",
		 "Geometry", "ActionList", "VarTable"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) trapflag = top.values[n[0]];
	if(n[1] != NOTFOUND) traponeshot = top.values[n[1]];
	if(n[2] != NOTFOUND) trapdetectable = top.values[n[2]];
	if(n[3] != NOTFOUND) trapdisarmable = top.values[n[3]];
	if(n[4] != NOTFOUND) type = top.values[n[4]];
	if(n[5] != NOTFOUND) cursor = top.values[n[5]];
	if(n[6] != NOTFOUND) paletteid = top.values[n[6]];
	if(n[7] != NOTFOUND) linkedtoflags = top.values[n[7]];
	if(n[8] != NOTFOUND) traptype = top.values[n[8]];
	if(n[9] != NOTFOUND) trapdetectdc = top.values[n[9]];
	if(n[10] != NOTFOUND) trapdisarmdc = top.values[n[10]];
	if(n[11] != NOTFOUND) loadscreenid = top.values[n[11]];
	if(n[12] != NOTFOUND) portraitid = top.values[n[12]];
	if(n[13] != NOTFOUND) creatorid = top.values[n[13]];
	if(n[14] != NOTFOUND) faction = top.values[n[14]];
	if(n[15] != NOTFOUND) objectid = top.values[n[15]];
	if(n[16] != NOTFOUND) highlightheight = *((float32*) &top.values[n[16]]);
	if(n[17] != NOTFOUND) x = *((float32*) &top.values[n[17]]);
	if(n[18] != NOTFOUND) y = *((float32*) &top.values[n[18]]);
	if(n[19] != NOTFOUND) z = *((float32*) &top.values[n[19]]);
	if(n[20] != NOTFOUND) dirx = *((float32*) &top.values[n[20]]);
	if(n[21] != NOTFOUND) diry = *((float32*) &top.values[n[21]]);
	if(n[22] != NOTFOUND)
		if(gff.getExoString(n[22], top, tag)) return ERR(gff);
	if(n[23] != NOTFOUND)
		if(gff.getExoString(n[23], top, comment)) return ERR(gff);
	if(n[24] != NOTFOUND)
		if(gff.getExoString(n[24], top, linkedto)) return ERR(gff);
	if(n[25] != NOTFOUND)
		if(gff.getResRef(n[25], top, onclick)) return ERR(gff);
	if(n[26] != NOTFOUND)
		if(gff.getResRef(n[26], top, ondisarm)) return ERR(gff);
	if(n[27] != NOTFOUND)
		if(gff.getResRef(n[27], top, ontraptriggered)) return ERR(gff);
	if(n[28] != NOTFOUND)
		if(gff.getResRef(n[28], top, scriptheartbeat)) return ERR(gff);
	if(n[29] != NOTFOUND)
		if(gff.getResRef(n[29], top, scriptonenter)) return ERR(gff);
	if(n[30] != NOTFOUND)
		if(gff.getResRef(n[30], top, scriptonexit)) return ERR(gff);
	if(n[31] != NOTFOUND)
		if(gff.getResRef(n[31], top, scriptuserdefine)) return ERR(gff);
	if(n[32] != NOTFOUND)
		if(gff.getResRef(n[32], top, templateresref)) return ERR(gff);
	if(n[33] != NOTFOUND)
		if(gff.getExoLocString(n[33], top, name)) return ERR(gff);
	if(n[34] != NOTFOUND) if(getGeometry(gff, n[34], top)) return errcode;
	if(n[35] != NOTFOUND) if(getActions(gff, n[16], top)) return errcode;
	if(n[36] != NOTFOUND)
		if(variables.load(gff, n[36], top)) return ERR(variables);
	checkRanges();

	return errcode = 0;
}

int Trigger::getGeometry(GFFFile &gff, uint32 gln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"PointX", "PointY", "PointZ"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(gln, top, scts)) return ERR(gff);
	points.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&points[i], 0, sizeof(Point));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) points[i].x = *((float32*) &scts[i].values[n[0]]);
		if(n[1] != NOTFOUND) points[i].y = *((float32*) &scts[i].values[n[1]]);
		if(n[2] != NOTFOUND) points[i].z = *((float32*) &scts[i].values[n[2]]);
	}
	return errcode = 0;
}

int Trigger::getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(aln, top, scts)) return ERR(gff);
	actions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(actions[i].load(gff, scts[i])) return ERR(actions[i]);
	return errcode = 0;
}

void Trigger::checkRanges(void)
{
	if(linkedtoflags > 2) linkedtoflags = 2;
	if(tag.length() > 32) tag.erase(32);
	if(type > 2) type = 2;
	switch(type)
	{
		case TRIGGER_TYPE_GENERIC:
			break;
		case TRIGGER_TYPE_TRANSITION:
			trapflag = false;
			if(!linkedtoflags) linkedtoflags = 1;
			break;
		case TRIGGER_TYPE_TRAP:
			trapflag = true;
			break;
	}
}

int Trigger::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTT;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Trigger::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTT;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Trigger::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapFlag", trapflag)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapOneShot", traponeshot))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapDetectable", trapdetectable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapDisarmable", trapdisarmable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Type", type)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Cursor", cursor)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "LinkedToFlags", linkedtoflags))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapType", traptype)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapDetectDC", trapdetectdc))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "DisarmDC", trapdisarmdc))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "LoadScreenID", loadscreenid))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "PortraitId", portraitid))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Faction", faction)) return ERR(gff);
	if(gff.writeFloat(top, "HighlightHeight", highlightheight)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeExoString(top, "LinkedTo", linkedto)) return ERR(gff);
	if(gff.writeResRef(top, "OnClick", onclick)) return ERR(gff);
	if(gff.writeResRef(top, "OnDisarm", ondisarm)) return ERR(gff);
	if(gff.writeResRef(top, "OnTrapTriggered", ontraptriggered)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptHeartbeat", scriptheartbeat)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptOnEnter", scriptonenter)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptOnExit", scriptonexit)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptUserDefine", scriptuserdefine))
		return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocalizedName", name)) return ERR(gff);
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
			if(gff.writeVar(top, NWN_VAR_UINT32, "CreatorId", creatorid))
				return ERR(gff);
			if(writeActions(gff, top)) return errcode;
		case NWN_OBJECT_INSTANCE:
			if(gff.writeFloat(top, "XPosition", x)) return ERR(gff);
			if(gff.writeFloat(top, "YPosition", y)) return ERR(gff);
			if(gff.writeFloat(top, "ZPosition", z)) return ERR(gff);
			if(gff.writeFloat(top, "XOrientation", dirx)) return ERR(gff);
			if(gff.writeFloat(top, "YOrientation", diry)) return ERR(gff);
			if(gff.writeList(list, top, "Geometry")) return ERR(gff);
			for(uint32 i=0;i<points.size();i++)
			{
				if(gff.writeListStruct(sct, list)) return ERR(gff);
				if(gff.writeSetId(sct, 3)) return ERR(gff);
				if(gff.writeFloat(sct, "PointX", points[i].x)) return ERR(gff);
				if(gff.writeFloat(sct, "PointY", points[i].y)) return ERR(gff);
				if(gff.writeFloat(sct, "PointZ", points[i].z)) return ERR(gff);
			}
			break;
	}
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Trigger::writeActions(GFFFile &gff, uint32 top)
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
