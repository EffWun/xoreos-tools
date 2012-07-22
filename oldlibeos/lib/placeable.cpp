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

Placeable::Placeable()
{
	isstatic = useable = hasinventory = diewhenempty = lightstate = false;
	animationstate = bodybag = trapcreator = trapfaction = 0;
}

int Placeable::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Placeable::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Placeable::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Static", "Useable", "HasInventory", "DieWhenEmpty", "LightState",
		 "Animation", "BodyBag", "TrapCreator", "TrapFaction",
		 "Portal", "OnInvDisturbed", "OnUsed", "ItemList"};
	std::vector<uint32> n;

	if(Situated::load(gff, top)) return errcode;
	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) isstatic = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) useable = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) hasinventory = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) diewhenempty = (bool) top.values[n[3]];
	if(n[4] != NOTFOUND) lightstate = (bool) top.values[n[4]];
	if(n[5] != NOTFOUND) animationstate = top.values[n[5]];
	if(n[6] != NOTFOUND) bodybag = top.values[n[6]];
	if(n[7] != NOTFOUND) trapcreator = top.values[n[7]];
	if(n[8] != NOTFOUND) trapfaction = top.values[n[8]];
	if(n[9] != NOTFOUND)
		if(gff.getExoString(n[9], top, portal)) return ERR(gff);
	if(n[10] != NOTFOUND)
		if(gff.getResRef(n[10], top, oninventorydisturbed)) return ERR(gff);
	if(n[11] != NOTFOUND)
		if(gff.getResRef(n[11], top, onused)) return ERR(gff);
	if(n[12] != NOTFOUND) if(getItems(gff, n[12], top)) return errcode;
	checkRanges();

	return errcode = 0;
}

int Placeable::getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(iln, top, scts)) return ERR(gff);
	items.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		items[i].objectstate = objectstate;
		items[i].itemstate = NWN_ITEM_INVENTORY;
		if(items[i].load(gff, scts[i])) return ERR(items[i]);
	}
	return errcode = 0;
}

void Placeable::checkRanges(void)
{
	Situated::checkRanges();
	if((animationstate > 5) && (animationstate < 72)) animationstate = 5;
	else if(animationstate > 76) animationstate = 76;
}

int Placeable::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTP;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Placeable::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTP;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Placeable::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(Situated::save(gff, top)) return errcode;
	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "Static", isstatic)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Useable", useable)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "HasInventory", hasinventory))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "BodyBag", bodybag)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Type", 0)) return ERR(gff);
	if(gff.writeResRef(top, "OnInvDisturbed", oninventorydisturbed))
		return ERR(gff);
	if(gff.writeResRef(top, "OnUsed", onused)) return ERR(gff);
	if(!items.empty())
	{
		if(gff.writeList(list, top, "ItemList")) return ERR(gff);
		for(uint32 i=0;i<items.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, i)) return ERR(gff);
			items[i].objectstate = objectstate;
			items[i].itemstate = NWN_ITEM_INVENTORY;
			if(items[i].save(gff, sct)) return ERR(items[i]);
		}
	}
	if(objectstate != NWN_OBJECT_SAVE) return errcode = 0;
	if(gff.writeVar(top, NWN_VAR_INT32, "Animation", animationstate))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "DieWhenEmpty", diewhenempty))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "GroundPile", 0)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "LightState", lightstate))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "TrapCreator", trapcreator))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "TrapFaction", trapfaction))
		return ERR(gff);
	if(gff.writeExoString(top, "Portal", portal)) return ERR(gff);
	return errcode = 0;
}

// vim:tabstop=2
