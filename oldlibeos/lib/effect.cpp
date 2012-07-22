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

Effect::Effect()
{
	exposed = iconshown = skiponload = false;
	type = subtype = numintegers = 0;
	creatorid = spellid = expireday = expiretime = 0;
	duration = 0.0;
	id = (uint64) -1;
}

int Effect::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"IsExposed", "IsIconShown", "SkipOnLoad", "Type", "SubType",
		 "NumIntegers", "CreatorId", "SpellId", "ExpireDay", "ExpireTime",
		 "Duration", "IntList", "FloatList", "StringList", "ObjectList", "Id"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) exposed = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) iconshown = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) skiponload = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) type = top.values[n[3]];
	if(n[4] != NOTFOUND) subtype = top.values[n[4]];
	if(n[5] != NOTFOUND) numintegers = top.values[n[5]];
	if(n[6] != NOTFOUND) creatorid = top.values[n[6]];
	if(n[7] != NOTFOUND) spellid = top.values[n[7]];
	if(n[8] != NOTFOUND) expireday = top.values[n[8]];
	if(n[9] != NOTFOUND) expiretime = top.values[n[9]];
	if(n[10] != NOTFOUND) duration = top.values[n[10]];
	if(n[11] != NOTFOUND) if(getInts(gff, n[11], top)) return errcode;
	if(n[12] != NOTFOUND) if(getFloats(gff, n[12], top)) return errcode;
	if(n[13] != NOTFOUND) if(getStrings(gff, n[13], top)) return errcode;
	if(n[14] != NOTFOUND) if(getObjects(gff, n[14], top)) return errcode;
	if(n[15] != NOTFOUND) id = top.values[n[15]];

	return errcode = 0;
}

int Effect::getInts(GFFFile &gff, uint32 iln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Value";
	if(gff.getList(iln, top, scts)) return ERR(gff);
	ints.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) ints[i] = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

int Effect::getFloats(GFFFile &gff, uint32 fln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Value";
	if(gff.getList(fln, top, scts)) return ERR(gff);
	floats.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) floats[i] = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

int Effect::getStrings(GFFFile &gff, uint32 sln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Value";
	if(gff.getList(sln, top, scts)) return ERR(gff);
	strings.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(gff.getExoString(aderrstr, scts[i], strings[i])) return ERR(gff);
	aderrstr.clear();
	return errcode = 0;
}

int Effect::getObjects(GFFFile &gff, uint32 oln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Value";
	if(gff.getList(oln, top, scts)) return ERR(gff);
	objects.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) objects[i] = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

int Effect::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;
	if(gff.writeVar(top, NWN_VAR_INT32, "IsExposed", exposed)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "IsIconShown", iconshown)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "SkipOnLoad", skiponload)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "Type", type)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "SubType", subtype)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "NumIntegers", numintegers))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "CreatorId", creatorid)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "SpellId", spellid)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "ExpireDay", expireday)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "ExpireTime", expiretime))
		return ERR(gff);
	if(id != ((uint64) -1))
		if(gff.writeVar(top, NWN_VAR_UINT64, "Id", id)) return ERR(gff);
	if(gff.writeFloat(top, "Duration", duration)) return ERR(gff);
	if(gff.writeList(list, top, "IntList")) return ERR(gff);
	for(uint32 i=0;i<ints.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 3)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "Value", ints[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, "FloatList")) return ERR(gff);
	for(uint32 i=0;i<floats.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 4)) return ERR(gff);
		if(gff.writeFloat(sct, "Value", floats[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, "StringList")) return ERR(gff);
	for(uint32 i=0;i<strings.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 5)) return ERR(gff);
		if(gff.writeExoString(sct, "Value", strings[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, "ObjectList")) return ERR(gff);
	for(uint32 i=0;i<objects.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 6)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "Value", objects[i])) return ERR(gff);
	}
	return errcode = 0;
}

// vim:tabstop=2
