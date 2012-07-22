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

void VarTable::add(std::string name, int32 value)
{
	uint32 n;
	variables.resize((n=variables.size())+1);
	variables[n].set(name, value);
}

void VarTable::add(std::string name, float32 value)
{
	uint32 n;
	variables.resize((n=variables.size())+1);
	variables[n].set(name, value);
}

void VarTable::add(std::string name, std::string value)
{
	uint32 n;
	variables.resize((n=variables.size())+1);
	variables[n].set(name, value);
}

void VarTable::add(std::string name, uint32 value)
{
	uint32 n;
	variables.resize((n=variables.size())+1);
	variables[n].set(name, value);
}

void VarTable::add(std::string name, Location value)
{
	uint32 n;
	variables.resize((n=variables.size())+1);
	variables[n].set(name, value);
}

bool VarTable::set(std::string name, int32 value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].set(name, value);
			return true;
		}
	add(name, value);
	return false;
}

bool VarTable::set(std::string name, float32 value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].set(name, value);
			return true;
		}
	add(name, value);
	return false;
}

bool VarTable::set(std::string name, std::string value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].set(name, value);
			return true;
		}
	add(name, value);
	return false;
}


bool VarTable::set(std::string name, uint32 value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].set(name, value);
			return true;
		}
	add(name, value);
	return false;
}

bool VarTable::set(std::string name, Location value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].set(name, value);
			return true;
		}
	add(name, value);
	return false;
}

bool VarTable::get(std::string name, int32 &value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].get(value);
			return true;
		}
	value = 0;
	return false;
}

bool VarTable::get(std::string name, float32 &value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].get(value);
			return true;
		}
	value = 0;
	return false;
}

bool VarTable::get(std::string name, std::string &value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].get(value);
			return true;
		}
	value = "";
	return false;
}

bool VarTable::get(std::string name, uint32 &value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].get(value);
			return true;
		}
	value = 0;
	return false;
}

bool VarTable::get(std::string name, Location &value)
{
	for(uint32 i=0;i<variables.size();i++)
		if(variables[i].name == name)
		{
			variables[i].get(value);
			return true;
		}
	Location loc;
	memset(&loc, 0, sizeof(Location));
	value = loc;
	return false;
}

void VarTable::del(uint32 n, uint32 count)
{
	if(n >= variables.size()) return;
	if(count > (variables.size()-n)) count = variables.size()-n;
	variables.erase(variables.begin()+n, variables.begin()+n+count-1);
}

void VarTable::pop(void)
{
	variables.pop_back();
}

void VarTable::clear(void)
{
	variables.clear();
}

bool VarTable::empty(void)
{
	return variables.empty();
}

uint32 VarTable::size(void)
{
	return variables.size();
}

std::string VarTable::getName(uint32 n)
{
	if(n >= variables.size()) return "";
	return variables[n].name;
}

int VarTable::load(GFFFile &gff, uint32 vln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"Name", "Type", "Value"};
	int8 type;
	std::string name, valstr;
	Location valloc;
	GFFFile::Struct locsct;
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(vln, top, scts)) return ERR(gff);
	for(uint32 i=0;i<scts.size();i++)
	{
		type = VAR_TYPE_UNDEFINED;
		name.clear();
		n.resize((sizeof(tlabels)/sizeof(std::string)));
		for(uint32 j=0;j<n.size();j++)
		{
			aderrstr = tlabels[j];
			if((n[j] = gff.getIdxByLabel(tlabels[j], scts[i])) == ERROR)
				return ERR(gff);
		}
		aderrstr.clear();
		if(n[0] != NOTFOUND)
			if(gff.getExoString(n[0], scts[i], name)) return ERR(gff);
		if(n[1] != NOTFOUND) type = scts[i].values[n[1]] - 1;
		if((type == VAR_TYPE_UNDEFINED) || (n[2] == NOTFOUND)) continue;
		switch(type)
		{
			case VAR_TYPE_INT:
				set(name, (int32) scts[i].values[n[2]]);
				break;
			case VAR_TYPE_FLOAT:
				set(name, *((float32*) &scts[i].values[n[2]]));
				break;
			case VAR_TYPE_STRING:
				if(gff.getExoString(n[2], scts[i], valstr)) return ERR(gff);
				set(name, valstr);
				break;
			case VAR_TYPE_OBJECT:
				set(name, (uint32) scts[i].values[n[2]]);
				break;
			case VAR_TYPE_LOCATION:
				if(gff.getStruct(scts[i].values[n[2]], locsct)) return ERR(gff);
				if(getLocation(valloc, gff, locsct)) return errcode;
				set(name, valloc);
				break;
		}
	}
	return errcode = 0;
}

int VarTable::getLocation(Location &loc, GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Area", "PositionX", "PositionY", "PositionZ",
		 "OrientationX", "OrientationY", "OrientationZ"};
	std::vector<uint32> n;

	n.resize((sizeof(tlabels)/sizeof(std::string)));
	for(uint32 i=0;i<n.size();i++)
	{
		aderrstr = tlabels[i];
		if((n[i] = gff.getIdxByLabel(tlabels[i], top)) == ERROR)
			return ERR(gff);
	}
	aderrstr.clear();
	if(n[0] != NOTFOUND) loc.area = top.values[n[0]];
	if(n[1] != NOTFOUND) loc.x = *((float32 *) &top.values[n[1]]);
	if(n[2] != NOTFOUND) loc.y = *((float32 *) &top.values[n[2]]);
	if(n[3] != NOTFOUND) loc.z = *((float32 *) &top.values[n[3]]);
	if(n[4] != NOTFOUND) loc.dirx = *((float32 *) &top.values[n[4]]);
	if(n[5] != NOTFOUND) loc.diry = *((float32 *) &top.values[n[5]]);
	if(n[6] != NOTFOUND) loc.dirz = *((float32 *) &top.values[n[6]]);

	return errcode = 0;
}

int VarTable::save(GFFFile &gff, uint32 vln)
{
	uint32 sct, vsct;

	for(uint32 i=0;i<variables.size();i++)
	{
		if(gff.writeListStruct(sct, vln)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeExoString(sct, "Name", variables[i].name)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "Type", variables[i].type+1))
			return ERR(gff);
		switch(variables[i].type)
		{
			case VAR_TYPE_INT:
				if(gff.writeVar(sct, NWN_VAR_INT32, "Value", variables[i].value.tint))
					return ERR(gff);
				break;
			case VAR_TYPE_FLOAT:
				if(gff.writeFloat(sct, "Value", variables[i].value.tfloat))
					return ERR(gff);
				break;
			case VAR_TYPE_STRING:
				if(gff.writeExoString(sct, "Value", variables[i].value.tstr))
					return ERR(gff);
				break;
			case VAR_TYPE_OBJECT:
				if(gff.writeVar(sct, NWN_VAR_UINT32, "Value", variables[i].value.tobj))
					return ERR(gff);
				break;
			case VAR_TYPE_LOCATION:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 2)) return ERR(gff);
				if(writeLocation(variables[i].value.tloc, gff, vsct)) return errcode;
				break;
		}
	}
	return errcode = 0;
}

int VarTable::writeLocation(Location &loc, GFFFile &gff, uint32 sct)
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

// vim:tabstop=2
