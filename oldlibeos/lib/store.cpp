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

Store::Store()
{
	objectstate = NWN_OBJECT_INSTANCE;
	objectid = INVALID_OBJECT_ID;
	blackmarket = false;
	maxbuyprice = storegold = -1;
	id = 0;
	bm_markdown = markdown = markup = identifyprice = 100;
	x = y = z = dirx = diry = 0.0;
}

int Store::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Store::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Store::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"BlackMarket", "ID", "BM_MarkDown", "MarkDown", "MarkUp", "MaxBuyPrice",
		 "StoreGold", "IdentifyPrice", "XPosition", "YPosition", "ZPosition",
		 "XOrientation", "YOrientation", "Tag", "Comment", "OnOpenStore",
		 "OnStoreClosed", "ResRef", "TemplateResRef", "LocName", "StoreList",
		 "WillNotBuy", "WillOnlyBuy", "ObjectId", "VarTable"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) blackmarket = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) id = top.values[n[1]];
	if(n[2] != NOTFOUND) bm_markdown = top.values[n[2]];
	if(n[3] != NOTFOUND) markdown = top.values[n[3]];
	if(n[4] != NOTFOUND) markup = top.values[n[4]];
	if(n[5] != NOTFOUND) maxbuyprice = top.values[n[5]];
	if(n[6] != NOTFOUND) storegold = top.values[n[6]];
	if(n[7] != NOTFOUND) identifyprice = top.values[n[7]];
	if(n[8] != NOTFOUND) x = *((float32*) &top.values[n[8]]);
	if(n[9] != NOTFOUND) y = *((float32*) &top.values[n[9]]);
	if(n[10] != NOTFOUND) z = *((float32*) &top.values[n[10]]);
	if(n[11] != NOTFOUND) dirx = *((float32*) &top.values[n[11]]);
	if(n[12] != NOTFOUND) diry = *((float32*) &top.values[n[12]]);
	if(n[13] != NOTFOUND)
		if(gff.getExoString(n[13], top, tag)) return ERR(gff);
	if(n[14] != NOTFOUND)
		if(gff.getExoString(n[14], top, comment)) return ERR(gff);
	if(n[15] != NOTFOUND)
		if(gff.getResRef(n[15], top, onopenstore)) return ERR(gff);
	if(n[16] != NOTFOUND)
		if(gff.getResRef(n[16], top, onstoreclosed)) return ERR(gff);
	if(n[17] != NOTFOUND)
		if(gff.getResRef(n[17], top, templateresref)) return ERR(gff);
	if(n[18] != NOTFOUND)
		if(gff.getResRef(n[18], top, templateresref)) return ERR(gff);
	if(n[19] != NOTFOUND)
		if(gff.getExoLocString(n[19], top, name)) return ERR(gff);
	if(n[20] != NOTFOUND) if(getStoreLists(gff, n[20], top)) return errcode;
	if(n[21] != NOTFOUND)
		if(getBuyLimits(gff, n[21], top, willnotbuy)) return errcode;
	if(n[22] != NOTFOUND)
		if(getBuyLimits(gff, n[22], top, willonlybuy)) return errcode;
	if(n[23] != NOTFOUND) objectid = top.values[n[23]];
	if(n[24] != NOTFOUND)
		if(variables.load(gff, n[24], top)) return ERR(variables);
	checkRanges();

	return errcode = 0;
}

int Store::getStoreLists(GFFFile &gff, uint32 sln, GFFFile::Struct top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "ItemList";
	if(gff.getList(sln, top, scts)) return ERR(gff);
	for(uint32 i=0;i<scts.size();i++)
	{
		if(scts[i].id >= STORE_ITEMS_MAX) continue;
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND)
			if(getStoreList(gff, n, scts[i], items[scts[i].id])) return errcode;
	}
	aderrstr.clear();
	return errcode = 0;
}

int Store::getStoreList(GFFFile &gff, uint32 sln, GFFFile::Struct top,
		std::vector<StoreItem> &items)
{
	std::string tlabels[] =
		{"Infinite", "Repos_PosX", "Repos_Posy", "InventoryRes"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(sln, top, scts)) return ERR(gff);
	items.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		items[i].infinite = false;
		items[i].reposx = items[i].reposy = 0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) items[i].infinite = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) items[i].reposx = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) items[i].reposy = scts[i].values[n[2]];
		if(n[3] != NOTFOUND)
			if(gff.getResRef(n[3], scts[i], items[i].resref)) return ERR(gff);
	}
	return errcode = 0;
}

int Store::getBuyLimits(GFFFile &gff, uint32 brln, GFFFile::Struct top,
		std::vector<int32> &limits)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "BaseItem";
	if(gff.getList(brln, top, scts)) return ERR(gff);
	limits.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if((n == ERROR) || (n == NOTFOUND)) return errcode = 20;
		if(n != NOTFOUND) limits[i] = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

void Store::checkRanges(void)
{
	if(!willnotbuy.empty()) willonlybuy.clear();
	if(tag.length() > 32) tag.erase(32);
}

int Store::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTM;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Store::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTM;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Store::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "BlackMarket", blackmarket))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "BM_MarkDown", bm_markdown))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "MarkDown", markdown)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "MarkUp", markup)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "MaxBuyPrice", maxbuyprice))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "StoreGold", storegold)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "IdentifyPrice", identifyprice))
		return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeResRef(top, "OnOpenStore", onopenstore)) return ERR(gff);
	if(gff.writeResRef(top, "OnStoreClosed", onstoreclosed)) return ERR(gff);
	if(gff.writeResRef(top, "ResRef", templateresref)) return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocName", name)) return ERR(gff);
	if(gff.writeList(list, top, "StoreList")) return ERR(gff);
	for(uint32 i=0;i<STORE_ITEMS_MAX;i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(writeStoreList(gff, sct, items[i])) return errcode;
	}
	if(gff.writeList(list, top, "WillNotBuy")) return ERR(gff);
	if(writeBuyLimits(gff, list, willnotbuy)) return errcode;
	if(gff.writeList(list, top, "WillOnlyBuy")) return ERR(gff);
	if(writeBuyLimits(gff, list, willonlybuy)) return errcode;
	switch(objectstate)
	{
		case NWN_OBJECT_BLUEPRINT:
			if(gff.writeVar(top, NWN_VAR_INT8, "ID", id))
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
	return errcode = 0;
}

int Store::writeStoreList(GFFFile &gff, uint32 top, std::vector<StoreItem> &items)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "ItemList")) return ERR(gff);
	for(uint32 i=0;i<items.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(items[i].infinite)
			if(gff.writeVar(sct, NWN_VAR_INT8, "Infinite", items[i].infinite))
				return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "Repos_PosX", items[i].reposx))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "Repos_Posy", items[i].reposy))
			return ERR(gff);
		if(gff.writeResRef(sct, "InventoryRes", items[i].resref))
			return ERR(gff);
	}
	return errcode = 0;
}

int Store::writeBuyLimits(GFFFile &gff, uint32 list, std::vector<int32> &limits)
{
	uint32 sct;

	for(uint32 i=0;i<limits.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0x17E4D)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "BaseItem", limits[i]))
			return ERR(gff);
	}
	return errcode = 0;
}
// vim:tabstop=2
