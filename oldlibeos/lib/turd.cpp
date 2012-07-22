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

TURD::TURD()
{
	mapnumareas = calendarday = timeofday = playerid = areaid = 0;
	x = y = z = dirx = diry = dirz = 0.0;
}

int TURD::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"ModMapNumAreas", "TURD_CalendarDay", "TURD_TimeOfDay", "TURD_PlayerID",
		 "TURD_AreaId", "TURD_PositionX", "TURD_PositionY", "TURD_PositionZ",
		 "TURD_OrientatX", "TURD_OrientatY", "TURD_OrientatZ", "TURD_CommntyName",
		 "TURD_FirstName", "TURD_LastName", "Mod_MapAreasData", "EffectList",
		 "TURD_PersonalRep", "TURD_RepList", "Mod_MapDataList", "VarTable"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) mapnumareas = top.values[n[0]];
	if(n[1] != NOTFOUND) calendarday = top.values[n[1]];
	if(n[2] != NOTFOUND) timeofday = top.values[n[2]];
	if(n[3] != NOTFOUND) playerid = top.values[n[3]];
	if(n[4] != NOTFOUND) areaid = top.values[n[4]];
	if(n[5] != NOTFOUND) x = *((float32*) &top.values[n[5]]);
	if(n[6] != NOTFOUND) y = *((float32*) &top.values[n[6]]);
	if(n[7] != NOTFOUND) z = *((float32*) &top.values[n[7]]);
	if(n[8] != NOTFOUND) dirx = *((float32*) &top.values[n[8]]);
	if(n[9] != NOTFOUND) diry = *((float32*) &top.values[n[9]]);
	if(n[10] != NOTFOUND) dirz = *((float32*) &top.values[n[10]]);
	if(n[11] != NOTFOUND)
		if(gff.getExoString(n[11], top, commntyname)) return ERR(gff);
	if(n[12] != NOTFOUND)
		if(gff.getExoLocString(n[12], top, firstname)) return ERR(gff);
	if(n[13] != NOTFOUND)
		if(gff.getExoLocString(n[13], top, lastname)) return ERR(gff);
	if(n[14] != NOTFOUND)
		if(gff.getRaw(n[14], top, mapareasdata)) return ERR(gff);
	if(n[15] != NOTFOUND) if(getEffects(gff, n[15], top)) return errcode;
	if(n[16] != NOTFOUND) if(getPreputations(gff, n[16], top)) return errcode;
	if(n[17] != NOTFOUND) if(getReputations(gff, n[17], top)) return errcode;
	if(n[18] != NOTFOUND) if(getMapDatas(gff, n[18], top)) return errcode;
	if(n[19] != NOTFOUND)
		if(variables.load(gff, n[19], top)) return ERR(variables);

	return errcode = 0;
}

int TURD::getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(eln, top, scts)) return ERR(gff);
	effects.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(effects[i].load(gff, scts[i])) return ERR(effects[i]);
	return errcode = 0;
}

int TURD::getReputations(GFFFile &gff, uint32 rln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "TURD_RepAmount";
	if(gff.getList(rln, top, scts)) return ERR(gff);
	reputations.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) reputations[i] = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

int TURD::getPreputations(GFFFile &gff, uint32 prln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"TURD_PR_Decays", "TURD_PR_Amount", "TURD_PR_Duration",
		 "TURD_PR_Day", "TURD_PR_Time", "TURD_PR_ObjId"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(prln, top, scts)) return ERR(gff);
	preputations.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&preputations[i], 0, sizeof(PersonalReputation));
		preputations[i].objectid = INVALID_OBJECT_ID;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) preputations[i].decays = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) preputations[i].amount = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) preputations[i].duration = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) preputations[i].day = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) preputations[i].time = scts[i].values[n[4]];
		if(n[5] != NOTFOUND) preputations[i].objectid = scts[i].values[n[5]];
	}
	return errcode = 0;
}

int TURD::getMapDatas(GFFFile &gff, uint32 mdln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Mod_MapData";
	if(gff.getList(mdln, top, scts)) return ERR(gff);
	mapdatas.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(gff.getRaw(aderrstr, scts[i], mapdatas[i])) return ERR(gff);
	aderrstr.clear();
	return errcode = 0;
}

int TURD::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeVar(top, NWN_VAR_INT32, "ModMapNumAreas", mapnumareas))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "TURD_CalendarDay", calendarday))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "TURD_TimeOfDay", timeofday))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "TURD_PlayerID", playerid))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "TURD_AreaId", areaid)) return ERR(gff);
	if(gff.writeFloat(top, "TURD_PositionX", x)) return ERR(gff);
	if(gff.writeFloat(top, "TURD_PositionY", y)) return ERR(gff);
	if(gff.writeFloat(top, "TURD_PositionZ", z)) return ERR(gff);
	if(gff.writeFloat(top, "TURD_OrientatX", dirx)) return ERR(gff);
	if(gff.writeFloat(top, "TURD_OrientatY", diry)) return ERR(gff);
	if(gff.writeFloat(top, "TURD_OrientatZ", dirz)) return ERR(gff);
	if(gff.writeExoString(top, "TURD_CommntyName", commntyname)) return ERR(gff);
	if(gff.writeExoLocString(top, "TURD_FirstName", firstname)) return ERR(gff);
	if(gff.writeExoLocString(top, "TURD_LastName", lastname)) return ERR(gff);
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	if(gff.writeData(top, "Mod_MapAreasData", mapareasdata, false))
		return ERR(gff);
	if(gff.writeList(list, top, "TURD_PersonalRep")) return ERR(gff);
	for(uint32 i=0;i<preputations.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 47787)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "TURD_PR_Decays",
					preputations[i].decays))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "TURD_PR_Amount",
					preputations[i].amount))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "TURD_PR_Duration",
					preputations[i].duration))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "TURD_PR_Day", preputations[i].day))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "TURD_PR_Time", preputations[i].time))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "TURD_PR_ObjId",
					preputations[i].objectid))
			return ERR(gff);
	}
	if(gff.writeList(list, top, "TURD_RepList")) return ERR(gff);
	for(uint32 i=0;i<reputations.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 43962)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "TURD_RepAmount", reputations[i]))
			return ERR(gff);
	}
	if(gff.writeList(list, top, "Mod_MapDataList")) return ERR(gff);
	for(uint32 i=0;i<mapdatas.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeData(sct, "Mod_MapData", mapdatas[i], false)) return ERR(gff);
	}
	if(writeEffects(gff, top)) return errcode;
	return errcode = 0;
}

int TURD::writeEffects(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "EffectList")) return ERR(gff);
	for(uint32 i=0;i<effects.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 2)) return ERR(gff);
		if(effects[i].save(gff, sct)) return ERR(effects[i]);
	}
	return errcode = 0;
}

// vim:tabstop=2
