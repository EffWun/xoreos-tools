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

ScriptEvent::ScriptEvent()
{
	stype = "EventType";
	sint = "IntList";
	sfloat = "FloatList";
	sstr = "StringList";
	sobj = "ObjectList";
	sintv = "Parameter";
	sfloatv = "Parameter";
	sstrv = "Parameter";
	sobjv = "Parameter";
	sid = 105;
	type = 0;
}

int ScriptEvent::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] = {stype, sint, sfloat, sstr, sobj};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) type = top.values[n[0]];
	if(n[1] != NOTFOUND) if(getInts(gff, n[1], top)) return errcode;
	if(n[2] != NOTFOUND) if(getFloats(gff, n[2], top)) return errcode;
	if(n[3] != NOTFOUND) if(getStrings(gff, n[3], top)) return errcode;
	if(n[4] != NOTFOUND) if(getObjects(gff, n[4], top)) return errcode;
	return errcode = 0;
}

int ScriptEvent::getInts(GFFFile &gff, uint32 iln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = sintv;
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

int ScriptEvent::getFloats(GFFFile &gff, uint32 fln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = sfloatv;
	if(gff.getList(fln, top, scts)) return ERR(gff);
	floats.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) floats[i] = *((float32*) &scts[i].values[n]);
	}
	aderrstr.clear();
	return errcode = 0;
}

int ScriptEvent::getStrings(GFFFile &gff, uint32 sln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = sstrv;
	if(gff.getList(sln, top, scts)) return ERR(gff);
	strings.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(gff.getExoString(aderrstr, scts[i], strings[i])) return ERR(gff);
	aderrstr.clear();
	return errcode = 0;
}

int ScriptEvent::getObjects(GFFFile &gff, uint32 oln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = sobjv;
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

int ScriptEvent::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeVar(top, NWN_VAR_UINT16, stype, type)) return ERR(gff);
	if(gff.writeList(list, top, sint)) return ERR(gff);
	for(uint32 i=0;i<ints.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, sid)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, sintv, ints[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, sfloat)) return ERR(gff);
	for(uint32 i=0;i<floats.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, sid)) return ERR(gff);
		if(gff.writeFloat(sct, sfloatv, floats[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, sstr)) return ERR(gff);
	for(uint32 i=0;i<strings.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, sid)) return ERR(gff);
		if(gff.writeExoString(sct, sstrv, strings[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, sobj)) return ERR(gff);
	for(uint32 i=0;i<objects.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, sid)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, sobjv, objects[i]))
			return ERR(gff);
	}
	return errcode = 0;
}

void ScriptEvent::registerLabels(std::string stype, std::string sint, std::string sfloat,
		std::string sstr, std::string sobj, std::string sintv, std::string sfloatv, std::string sstrv,
		std::string sobjv)
{
	this->stype = stype;
	this->sint = sint;
	this->sfloat = sfloat;
	this->sstr = sstr;
	this->sobj = sobj;
	this->sintv = sintv;
	this->sfloatv = sfloatv;
	this->sstrv = sstrv;
	this->sobjv = sobjv;
}

void ScriptEvent::registerSid(uint32 id)
{
	sid = id;
}

// vim:tabstop=2
