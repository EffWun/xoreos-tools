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

Item::Item()
{
	objectstate = NWN_OBJECT_INSTANCE;
	itemstate = NWN_ITEM_NORMAL;
	modeltype = NWN_MODEL_UNDEFINED;
	equipped = NWN_EQUIP_NONE;
	objectid = INVALID_OBJECT_ID;
	cursed = plot = stolen = pickpocketable = false;
	identified = dropable = true;
	reposx = reposy = -1;
	charges = paletteid = baseitem = cost = addcost = 0;
	cloth1color = cloth2color = 0;
	leather1color = leather2color = 0;
	metal1color = metal2color = 0;
	armorneck = armortorso = armorbelt = armorpelvis = 0;
	armorlshoul = armorlbicep = armorlfarm = armorlhand = 0;
	armorlthigh = armorlshin = armorlfoot = 0;
	armorrshoul = armorrbicep = armorrfarm = armorrhand = 0;
	armorrthigh = armorrshin = armorrfoot = 0;
	modelpart1 = modelpart2 = modelpart3 = 0;
	armorrobe = 0;
	stacksize = 1;
}

int Item::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	itemstate = NWN_ITEM_NORMAL;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Item::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	itemstate = NWN_ITEM_NORMAL;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Item::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Cursed", "Plot", "Stolen", "Charges", "PaletteID", "Cloth1Color",
		 "Cloth2Color", "Leather1Color", "Leather2Color", "Metal1Color",
		 "Metal2Color", "ModelPart1", "ModelPart2", "ModelPart3", "ArmorPart_Neck",
		 "ArmorPart_Torso", "ArmorPart_Belt", "ArmorPart_Pelvis",
		 "ArmorPart_LShoul", "ArmorPart_LBicep", "ArmorPart_LFArm",
		 "ArmorPart_LHand", "ArmorPart_LThigh", "ArmorPart_LShin",
		 "ArmorPart_LFoot", "ArmorPart_RShoul", "ArmorPart_RBicep",
		 "ArmorPart_RFArm", "ArmorPart_RHand", "ArmorPart_RThigh",
		 "ArmorPart_RShin", "ArmorPart_RFoot", "ArmorPart_Robe", "BaseItem",
		 "StackSize", "Repos_PosX", "Repos_Posy", "Cost", "AddCost", "ObjectId",
		 "Tag", "Comment", "TemplateResRef", "InventoryRes", "EquipRes",
		 "LocalizedName", "Description", "DescIdentified", "PropertiesList",
		 "VarTable", "XPosition", "YPosition", "ZPosition",
		 "XOrientation", "YOrientation", "Dropable", "Identified",
		 "Pickpocketable", "ItemList"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) cursed = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) plot = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) stolen = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) charges = top.values[n[3]];
	if(n[4] != NOTFOUND) paletteid = top.values[n[4]];
	if(n[5] != NOTFOUND) cloth1color = top.values[n[5]];
	if(n[6] != NOTFOUND) cloth2color = top.values[n[6]];
	if(n[7] != NOTFOUND) leather1color = top.values[n[7]];
	if(n[8] != NOTFOUND) leather2color = top.values[n[8]];
	if(n[9] != NOTFOUND) metal1color = top.values[n[9]];
	if(n[10] != NOTFOUND) metal2color = top.values[n[10]];
	if(n[11] != NOTFOUND) modelpart1 = top.values[n[11]];
	if(n[12] != NOTFOUND) modelpart2 = top.values[n[12]];
	if(n[13] != NOTFOUND) modelpart3 = top.values[n[13]];
	if(n[14] != NOTFOUND) armorneck = top.values[n[14]];
	if(n[15] != NOTFOUND) armortorso = top.values[n[15]];
	if(n[16] != NOTFOUND) armorbelt = top.values[n[16]];
	if(n[17] != NOTFOUND) armorpelvis = top.values[n[17]];
	if(n[18] != NOTFOUND) armorlshoul = top.values[n[18]];
	if(n[19] != NOTFOUND) armorlbicep = top.values[n[19]];
	if(n[20] != NOTFOUND) armorlfarm = top.values[n[20]];
	if(n[21] != NOTFOUND) armorlhand = top.values[n[21]];
	if(n[22] != NOTFOUND) armorlthigh = top.values[n[22]];
	if(n[23] != NOTFOUND) armorlshin = top.values[n[23]];
	if(n[24] != NOTFOUND) armorlfoot = top.values[n[24]];
	if(n[25] != NOTFOUND) armorrshoul = top.values[n[25]];
	if(n[26] != NOTFOUND) armorrbicep = top.values[n[26]];
	if(n[27] != NOTFOUND) armorrfarm = top.values[n[27]];
	if(n[28] != NOTFOUND) armorrhand = top.values[n[28]];
	if(n[29] != NOTFOUND) armorrthigh = top.values[n[29]];
	if(n[30] != NOTFOUND) armorrshin = top.values[n[30]];
	if(n[31] != NOTFOUND) armorrfoot = top.values[n[31]];
	if(n[32] != NOTFOUND) armorrobe = top.values[n[32]];
	if(n[33] != NOTFOUND) baseitem = top.values[n[33]];
	if(n[34] != NOTFOUND) stacksize = top.values[n[34]];
	if(n[35] != NOTFOUND) reposx = top.values[n[35]];
	if(n[36] != NOTFOUND) reposy = top.values[n[36]];
	if(n[37] != NOTFOUND) cost = top.values[n[37]];
	if(n[38] != NOTFOUND) addcost = top.values[n[38]];
	if(n[39] != NOTFOUND) objectid = top.values[n[39]];
	if(n[40] != NOTFOUND)
		if(gff.getExoString(n[40], top, tag)) return ERR(gff);
	if(n[41] != NOTFOUND)
		if(gff.getExoString(n[41], top, comment)) return ERR(gff);
	if(n[42] != NOTFOUND)
		if(gff.getResRef(n[42], top, templateresref)) return ERR(gff);
	if(n[43] != NOTFOUND)
		if(gff.getResRef(n[43], top, inventoryres)) return ERR(gff);
	if(n[44] != NOTFOUND)
		if(gff.getResRef(n[44], top, equipres)) return ERR(gff);
	if(n[45] != NOTFOUND)
		if(gff.getExoLocString(n[45], top, name)) return ERR(gff);
	if(n[46] != NOTFOUND)
		if(gff.getExoLocString(n[46], top, description)) return ERR(gff);
	if(n[47] != NOTFOUND)
		if(gff.getExoLocString(n[47], top, descidentified)) return ERR(gff);
	if(n[48] != NOTFOUND) if(getProperties(gff, n[48], top)) return errcode;
	if(n[49] != NOTFOUND)
		if(variables.load(gff, n[49], top)) return ERR(variables);
	if(n[50] != NOTFOUND) x = *((float32*) &top.values[n[50]]);
	if(n[51] != NOTFOUND) y = *((float32*) &top.values[n[51]]);
	if(n[52] != NOTFOUND) z = *((float32*) &top.values[n[52]]);
	if(n[53] != NOTFOUND) dirx = *((float32*) &top.values[n[53]]);
	if(n[54] != NOTFOUND) diry = *((float32*) &top.values[n[54]]);
	if(n[55] != NOTFOUND) dropable = (bool) top.values[n[55]];
	if(n[56] != NOTFOUND) identified = (bool) top.values[n[56]];
	if(n[57] != NOTFOUND) pickpocketable = (bool) top.values[n[57]];
	if(n[58] != NOTFOUND) if(getItems(gff, n[58], top)) return errcode;
	checkRanges();

	return errcode = 0;
}

int Item::getProperties(GFFFile &gff, uint32 pln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"CostTable", "Param1", "Param1Value", "CostValue",
		 "PropertyName", "Subtype", "Useable", "UsesPerDay"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(pln, top, scts)) return ERR(gff);
	properties.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&properties[i], 0, sizeof(Property));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) properties[i].costtable = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) properties[i].param = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) properties[i].paramvalue = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) properties[i].costvalue = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) properties[i].propertyname = scts[i].values[n[4]];
		if(n[5] != NOTFOUND) properties[i].subtype = scts[i].values[n[5]];
		if(n[6] != NOTFOUND) properties[i].useable = (bool) scts[i].values[n[6]];
		if(n[7] != NOTFOUND) properties[i].usesperday = scts[i].values[n[7]];
	}
	return errcode = 0;
}

int Item::getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top)
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

void Item::checkRanges(void)
{
	if(tag.length() > 32) tag.erase(32);
}

int Item::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTI;
	objectstate = NWN_OBJECT_BLUEPRINT;
	itemstate = NWN_ITEM_NORMAL;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Item::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTI;
	objectstate = NWN_OBJECT_BLUEPRINT;
	itemstate = NWN_ITEM_NORMAL;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Item::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "Cursed", cursed)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Dropable", dropable)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Identified", identified)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Pickpocketable", pickpocketable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Plot", plot)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Stolen", stolen)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Charges", charges)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "BaseItem", baseitem)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "StackSize", stacksize)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Cost", cost)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "AddCost", addcost)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocalizedName", name)) return ERR(gff);
	if(gff.writeExoLocString(top, "Description", description)) return ERR(gff);
	if(gff.writeExoLocString(top, "DescIdentified", descidentified))
		return ERR(gff);
	if(gff.writeList(list, top, "PropertiesList")) return ERR(gff);
	for(uint32 i=0;i<properties.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Useable", properties[i].useable))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "ChanceAppear", 100)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "CostTable", properties[i].costtable))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Param1", properties[i].param))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Param1Value", properties[i].paramvalue))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "UsesPerDay", properties[i].usesperday))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "CostValue", properties[i].costvalue))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "PropertyName",
					properties[i].propertyname))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "Subtype", properties[i].subtype))
			return ERR(gff);
	}
	if((itemstate == NWN_ITEM_INVENTORY)
			|| (itemstate == NWN_ITEM_BICINVENTORY)
			|| (itemstate == NWN_ITEM_EINVENTORY)
			|| (itemstate == NWN_ITEM_BICEINVENTORY))
	{
		if(gff.writeVar(top, NWN_VAR_UINT16, "Repos_PosX", reposx)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT16, "Repos_Posy", reposy)) return ERR(gff);
		if(objectstate == NWN_OBJECT_BLUEPRINT)
			if(gff.writeResRef(top, "InventoryRes", inventoryres)) return ERR(gff);
	}
	if((itemstate == NWN_ITEM_EINVENTORY) || (itemstate == NWN_ITEM_BICEINVENTORY))
		if(gff.writeResRef(top, "EquipRes", equipres)) return ERR(gff);
/*	if(itemstate == NWN_ITEM_STORE)
		if(gff.writeVar(top, NWN_VAR_INT8, "Infinite", infinite)) return ERR(gff);*/
	if((modeltype == NWN_MODEL_LAYERED)
			|| (modeltype == NWN_MODEL_ARMOR)
			|| (modeltype == NWN_MODEL_UNDEFINED))
	{
		if(gff.writeVar(top, NWN_VAR_INT8, "Cloth1Color", cloth1color))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Cloth2Color", cloth2color))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Leather1Color", leather1color))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Leather2Color", leather2color))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Metal1Color", metal1color))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Metal2Color", metal2color))
			return ERR(gff);
	}
	if(modeltype == NWN_MODEL_SIMPLE)
		if(gff.writeVar(top, NWN_VAR_INT8, "ModelPart1", modelpart1))
			return ERR(gff);
	if((modeltype == NWN_MODEL_COMPOSITE) || (modeltype == NWN_MODEL_UNDEFINED))
	{
		if(gff.writeVar(top, NWN_VAR_INT8, "ModelPart1", modelpart1))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ModelPart2", modelpart2))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ModelPart3", modelpart3))
			return ERR(gff);
	}
	if((modeltype == NWN_MODEL_ARMOR) || (modeltype == NWN_MODEL_UNDEFINED))
	{
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_Neck", armorneck))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_Torso", armortorso))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_Belt", armorbelt))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_Pelvis", armorpelvis))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LShoul", armorlshoul))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LBicep", armorlbicep))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LFArm", armorlfarm))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LHand", armorlhand))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LThigh", armorlthigh))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LShin", armorlshin))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_LFoot", armorlfoot))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RShoul", armorrshoul))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RBicep", armorrbicep))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RFArm", armorrfarm))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RHand", armorrhand))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RThigh", armorrthigh))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RShin", armorrshin))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RFoot", armorrfoot))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_Robe", armorrobe))
			return ERR(gff);
	}
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
	if(!items.empty()) if(writeItems(gff, top)) return errcode;
	return errcode = 0;
}

int Item::writeItems(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "ItemList")) return ERR(gff);
	for(uint32 i=0;i<items.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		items[i].objectstate = objectstate;
		items[i].itemstate = NWN_ITEM_INVENTORY;
		if(items[i].save(gff, sct)) return ERR(gff);
	}
	return errcode = 0;
}

// vim:tabstop=2
