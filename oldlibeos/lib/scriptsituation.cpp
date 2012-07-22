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

ScriptSituation::ScriptSituation()
{
	codesize = ipointer = secpointer = 0;
	stack.basepointer = stack.stackpointer = stack.totalsize = 0;
}

int ScriptSituation::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"InstructionPtr", "SecondaryPtr", "Code", "Name", "Stack"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) ipointer = top.values[n[0]];
	if(n[1] != NOTFOUND) secpointer = top.values[n[1]];
	if(n[2] != NOTFOUND) if(gff.getRaw(n[2], top, code)) return ERR(gff);
	if(n[3] != NOTFOUND)
		if(gff.getExoString(n[3], top, name)) return ERR(gff);
	if(n[4] != NOTFOUND) if(getStack(gff, top.values[n[4]])) return errcode;
	return errcode = 0;
}

void ScriptSituation::groundStackElement(StackElement &element)
{
	element.type = NWN_STYPE_UNDEFINED;
	element.valint = element.valobject = 0;
	element.valfloat = 0.0;
	memset(&element.vallocation, 0, sizeof(Location));
	element.valevent.type = 0;
	element.valtalent.multiclass = element.valtalent.casterlevel = 0;
	element.valtalent.metatype = element.valtalent.id = 0;
	element.valtalent.type = element.valtalent.itempropertyindex = 0;
	element.valtalent.item = 0;
}

int ScriptSituation::getStack(GFFFile &gff, uint32 ssn)
{
	std::string tlabels[] =
		{"BasePointer", "StackPointer", "TotalSize", "Stack"};
	std::vector<uint32> n;
	GFFFile::Struct ssct;

	if(gff.getStruct(ssn, ssct)) return ERR(gff);
	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				ssct, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) stack.basepointer = ssct.values[n[0]];
	if(n[1] != NOTFOUND) stack.stackpointer = ssct.values[n[1]];
	if(n[2] != NOTFOUND) stack.totalsize = ssct.values[n[2]];
	if(n[3] != NOTFOUND) if(getStackElements(gff, n[4], ssct)) return errcode;
	return errcode = 0;
}

int ScriptSituation::getStackElements(GFFFile &gff, uint32 seln,
		GFFFile::Struct &top)
{
	std::string tlabels[] = {"Type", "Value"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(seln, top, scts)) return ERR(gff);
	stack.elements.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		groundStackElement(stack.elements[i]);
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND)
			stack.elements[i].type = (NWNStackElementType) scts[i].values[n[0]];
		if(n[1] != NOTFOUND)
		{
			switch(stack.elements[i].type)
			{
				case NWN_STYPE_INT:
				case NWN_STYPE_OBJECT:
					stack.elements[i].valint = scts[i].values[n[1]];
					break;
				case NWN_STYPE_FLOAT:
					stack.elements[i].valfloat = *((float32 *) &scts[i].values[n[1]]);
					break;
				case NWN_STYPE_STRING:
					if(gff.getExoString(n[1], scts[i], stack.elements[i].valstring))
						return ERR(gff);
					break;
				case NWN_STYPE_EFFECT:
					if(getEffect(gff, scts[i].values[n[1]], stack.elements[i]))
						return errcode;
					break;
				case NWN_STYPE_EVENT:
					if(getEvent(gff, scts[i].values[n[1]], stack.elements[i]))
						return errcode;
					break;
				case NWN_STYPE_LOCATION:
					if(getLocation(gff, scts[i].values[n[1]], stack.elements[i]))
						return errcode;
					break;
				case NWN_STYPE_TALENT:
					if(getTalent(gff, scts[i].values[n[1]], stack.elements[i]))
						return errcode;
					break;
				case NWN_STYPE_ITEMPROPERTY:
					if(getEffect(gff, scts[i].values[n[1]], stack.elements[i]))
						return errcode;
					break;
				default:
					break;
			}
		}
	}
	return errcode = 0;
}

int ScriptSituation::getEffect(GFFFile &gff, uint32 esn, StackElement &element)
{
	GFFFile::Struct esct;

	if(gff.getStruct(esn, esct)) return ERR(gff);
	if(element.valeffect.load(gff, esct)) return ERR(element.valeffect);
	return errcode = 0;
}

int ScriptSituation::getEvent(GFFFile &gff, uint32 esn, StackElement &element)
{
	GFFFile::Struct esct;

	if(gff.getStruct(esn, esct)) return ERR(gff);
	if(element.valevent.load(gff, esct)) return ERR(element.valevent);
	return errcode = 0;
}

int ScriptSituation::getLocation(GFFFile &gff, uint32 lsn, StackElement &element)
{
	std::string tlabels[] =
		{"Area", "PositionX", "PositionY", "PositionZ",
		 "OrientationX", "OrientationY", "OrientationZ"};
	std::vector<uint32> n;
	GFFFile::Struct lsct;

	if(gff.getStruct(lsn, lsct)) return ERR(gff);
	n.resize((sizeof(tlabels)/sizeof(std::string)));
	for(uint32 i=0;i<n.size();i++)
	{
		aderrstr = tlabels[i];
		if((n[i] = gff.getIdxByLabel(tlabels[i], lsct)) == ERROR)
			return ERR(gff);
	}
	aderrstr.clear();
	if(n[0] != NOTFOUND) element.vallocation.area = lsct.values[n[0]];
	if(n[1] != NOTFOUND)
		element.vallocation.x = *((float32 *) &lsct.values[n[1]]);
	if(n[2] != NOTFOUND)
		element.vallocation.y = *((float32 *) &lsct.values[n[2]]);
	if(n[3] != NOTFOUND)
		element.vallocation.z = *((float32 *) &lsct.values[n[3]]);
	if(n[4] != NOTFOUND)
		element.vallocation.dirx = *((float32 *) &lsct.values[n[4]]);
	if(n[5] != NOTFOUND)
		element.vallocation.diry = *((float32 *) &lsct.values[n[5]]);
	if(n[6] != NOTFOUND)
		element.vallocation.dirz = *((float32 *) &lsct.values[n[6]]);

	return errcode = 0;
}

int ScriptSituation::getTalent(GFFFile &gff, uint32 tsn, StackElement &element)
{
	std::string tlabels[] =
		{"MultiClass", "CasterLevel", "MetaType",
		 "ID", "Type", "ItemPropertyIndex", "Item"};
	std::vector<uint32> n;
	GFFFile::Struct tsct;

	if(gff.getStruct(tsn, tsct)) return ERR(gff);
	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				tsct, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) element.valtalent.multiclass = tsct.values[n[0]];
	if(n[1] != NOTFOUND) element.valtalent.casterlevel = tsct.values[n[1]];
	if(n[2] != NOTFOUND) element.valtalent.metatype = tsct.values[n[2]];
	if(n[3] != NOTFOUND) element.valtalent.id = tsct.values[n[3]];
	if(n[4] != NOTFOUND) element.valtalent.type = tsct.values[n[4]];
	if(n[5] != NOTFOUND) element.valtalent.itempropertyindex = tsct.values[n[5]];
	if(n[6] != NOTFOUND) element.valtalent.item = tsct.values[n[6]];
	return errcode = 0;
}

int ScriptSituation::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeVar(top, NWN_VAR_INT32, "InstructionPtr", ipointer))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "SecondaryPtr", secpointer))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "CodeSize", codesize)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "StackSize", stack.elements.size()))
		return ERR(gff);
	if(gff.writeData(top, "Code", code, false)) return ERR(gff);
	if(gff.writeExoString(top, "Name", name)) return ERR(gff);
	if(gff.writeStruct(sct, "Stack", top)) return ERR(gff);
	if(gff.writeSetId(sct, 0)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "BasePointer", stack.basepointer))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "StackPointer", stack.stackpointer))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "TotalSize", stack.totalsize))
		return ERR(gff);
	if(gff.writeList(list, sct, "Stack")) return ERR(gff);
	if(writeStackElements(gff, list)) return errcode;
	return errcode = 0;
}

int ScriptSituation::writeStackElements(GFFFile &gff, uint32 list)
{
	uint32 sct, vsct;

	for(uint32 i=0;i<stack.elements.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Type", (uint8) stack.elements[i].type))
				return ERR(gff);
		switch(stack.elements[i].type)
		{
			case NWN_STYPE_INT:
				if(gff.writeVar(sct, NWN_VAR_INT32, "Value", stack.elements[i].valint))
						return ERR(gff);
				break;
			case NWN_STYPE_FLOAT:
				if(gff.writeFloat(sct, "Value", stack.elements[i].valfloat))
						return ERR(gff);
				break;
			case NWN_STYPE_STRING:
				if(gff.writeExoString(sct, "Value", stack.elements[i].valstring))
						return ERR(gff);
				break;
			case NWN_STYPE_OBJECT:
				if(gff.writeVar(sct, NWN_VAR_UINT32, "Value",
							stack.elements[i].valobject))
						return ERR(gff);
				break;
			case NWN_STYPE_EFFECT:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 0)) return ERR(gff);
				if(stack.elements[i].valeffect.save(gff, vsct))
					return ERR(stack.elements[i].valeffect);
				break;
			case NWN_STYPE_EVENT:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 1)) return ERR(gff);
				if(stack.elements[i].valevent.save(gff, vsct))
					return ERR(stack.elements[i].valevent);
				break;
			case NWN_STYPE_LOCATION:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 2)) return ERR(gff);
				if(writeLocation(gff, vsct, stack.elements[i].vallocation))
					return errcode;
				break;
			case NWN_STYPE_TALENT:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 3)) return ERR(gff);
				if(writeTalent(gff, vsct, stack.elements[i].valtalent)) return errcode;
				break;
			case NWN_STYPE_ITEMPROPERTY:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 4)) return ERR(gff);
				if(stack.elements[i].valeffect.save(gff, vsct))
					return ERR(stack.elements[i].valeffect);
				break;
			default:
				break;
		}
	}
	return errcode = 0;
}

int ScriptSituation::writeLocation(GFFFile &gff, uint32 sct, Location &loc)
{
	if(gff.writeVar(sct, NWN_VAR_UINT32, "Area", loc.area)) return ERR(gff);
	if(gff.writeFloat(sct, "PositionX", loc.x)) return ERR(gff);
	if(gff.writeFloat(sct, "PositionY", loc.y)) return ERR(gff);
	if(gff.writeFloat(sct, "PositionZ", loc.z)) return ERR(gff);
	if(gff.writeFloat(sct, "OrientationX", loc.dirx)) return ERR(gff);
	if(gff.writeFloat(sct, "OrientationY", loc.diry)) return ERR(gff);
	if(gff.writeFloat(sct, "OrientationZ", loc.dirz)) return ERR(gff);
	return errcode = 0;
}

int ScriptSituation::writeTalent(GFFFile &gff, uint32 sct, ScriptTalent &talent)
{
	if(gff.writeVar(sct, NWN_VAR_UINT8, "MultiClass", talent.multiclass))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_UINT8, "CasterLevel", talent.casterlevel))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_UINT8, "MetaType", talent.metatype))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "ID", talent.id)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "Type", talent.type)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "ItemPropertyIndex",
				talent.itempropertyindex))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_UINT32, "Item", talent.item)) return ERR(gff);
	return errcode = 0;
}

// vim:tabstop=2
