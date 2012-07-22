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

Action::Action()
{
	groupactionid = actionid = 0;
}

int Action::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] = {"GroupActionId", "ActionId", "Paramaters"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) groupactionid = top.values[n[0]];
	if(n[1] != NOTFOUND) actionid = top.values[n[1]];
	return errcode = 0;
}

void Action::groundParameter(Parameter &parameter)
{
	parameter.type = NWN_ATYPE_UNDEFINED;
	parameter.valint = parameter.valobject = 0;
	parameter.valfloat = 0.0;
}

int Action::getParameters(GFFFile &gff, uint32 pln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"Type", "Value"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(pln, top, scts)) return ERR(gff);
	parameters.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		groundParameter(parameters[i]);
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND)
			parameters[i].type = (NWNActionParameterType) scts[i].values[n[0]];
		if(n[1] != NOTFOUND)
		{
			switch(parameters[i].type)
			{
				case NWN_ATYPE_INT:
				case NWN_ATYPE_OBJECT:
					parameters[i].valint = scts[i].values[n[1]];
					break;
				case NWN_ATYPE_FLOAT:
					parameters[i].valfloat = *((float32 *) &scts[i].values[n[1]]);
					break;
				case NWN_ATYPE_STRING:
					if(gff.getExoString(n[1], scts[i], parameters[i].valstring))
						return ERR(gff);
					break;
				case NWN_ATYPE_SCRIPTSITUATION:
					if(getScriptSituation(gff, scts[i].values[n[1]], parameters[i]))
						return errcode;
					break;
				default:
					break;
			}
		}
	}
	return errcode = 0;
}

int Action::getScriptSituation(GFFFile &gff, uint32 psn, Parameter &element)
{
	GFFFile::Struct psct;

	if(gff.getStruct(psn, psct)) return ERR(gff);
	if(element.valscriptsituation.load(gff, psct))
		return ERR(element.valscriptsituation);
	return errcode = 0;
}

int Action::save(GFFFile &gff, uint32 top)
{
	uint32 list;

	if(gff.writeVar(top, NWN_VAR_UINT16, "GroupActionId", groupactionid))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "NumParams", parameters.size()))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "ActionId", actionid)) return ERR(gff);
	if(parameters.empty()) return errcode = 0;
	if(gff.writeList(list, top, "Paramaters")) return ERR(gff);
	if(writeParameters(gff, list)) return ERR(gff);
	return errcode = 0;
}

int Action::writeParameters(GFFFile &gff, uint32 list)
{
	uint32 sct, vsct;

	for(uint32 i=0;i<parameters.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 1)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "Type", (uint32) parameters[i].type))
			return ERR(gff);
		switch(parameters[i].type)
		{
			case NWN_ATYPE_INT:
				if(gff.writeVar(sct, NWN_VAR_INT32, "Value", parameters[i].valint))
					return ERR(gff);
				break;
			case NWN_ATYPE_FLOAT:
				if(gff.writeFloat(sct, "Value", parameters[i].valfloat))
					return ERR(gff);
				break;
			case NWN_ATYPE_OBJECT:
				if(gff.writeVar(sct, NWN_VAR_UINT32, "Value", parameters[i].valobject))
					return ERR(gff);
				break;
			case NWN_ATYPE_STRING:
				if(gff.writeExoString(sct, "Value", parameters[i].valstring))
					return ERR(gff);
				break;
			case NWN_ATYPE_SCRIPTSITUATION:
				if(gff.writeStruct(vsct, "Value", sct)) return ERR(gff);
				if(gff.writeSetId(vsct, 2)) return ERR(gff);
				if(parameters[i].valscriptsituation.save(gff, vsct))
					return ERR(parameters[i].valscriptsituation);
				break;
			default:
				break;
		}
	}
	return errcode = 0;
}

// vim:tabstop=2
