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

#include <cstdlib>

ModuleIFO::ModuleIFO()
{
	savegame = nwm = false;
	xpscale = 10;
	dawnhour = 6;
	duskhour = 18;
	minperhour = 2;
	starthour = 13;
	startday = 1;
	startmonth = 6;
	startyear = 1372;
	entryx = entryy = entryz = entrydirx = entrydiry = 0.0;
	nextcharid0 = nextcharid1 = nextobjid0 = nextobjid1 = nexteffectid = 0;
	creatorid = 2;
	version = 3;
	mingamever = "1.22";
	id.alloc(32);
}

int ModuleIFO::load(std::string fname)
{
	GFFFile gff;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int ModuleIFO::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int ModuleIFO::load(MemSeg &memseg)
{
	return load(memseg.mem, memseg.size);
}

int ModuleIFO::load(GFFFile &gff, GFFFile::Struct &top)
{
	if(gff.type != NWN_FILE_IFO) return errcode = 11;
	if(!id.mem) return errcode = 4;
	std::string tlabels[] =
		{"Mod_IsSaveGame", "Expansion_Pack", "Mod_Tag", "Mod_Name",
		 "Mod_Description", "Mod_Version", "Mod_ID", "Mod_CustomTlk",
		 "Mod_MinPerHour", "Mod_DawnHour", "Mod_DuskHour", "Mod_StartHour",
		 "Mod_StartDay", "Mod_StartMonth", "Mod_StartYear", "Mod_XPScale",
		 "Mod_Entry_Area", "Mod_Entry_X", "Mod_Entry_Y", "Mod_Entry_Z",
		 "Mod_Entry_Dir_X", "Mod_Entry_Dir_Y", "Mod_OnAcquirItem",
		 "Mod_OnActvtItem", "Mod_OnClientEntr", "Mod_OnClientLeav",
		 "Mod_OnCutsnAbort", "Mod_OnHeartbeat", "Mod_OnModLoad", "Mod_OnModStart",
		 "Mod_OnPlrDeath", "Mod_OnPlrDying", "Mod_OnPlrEqItm", "Mod_OnPlrLvlUp",
		 "Mod_OnPlrRest", "Mod_OnPlrUnEqItm", "Mod_OnSpawnBtnDn",
		 "Mod_OnUnAqreItem", "Mod_OnUsrDefined", "Mod_Area_list",
		 "Mod_MinGameVer", "Mod_Creator_ID", "Mod_HakList", "Mod_Hak",
		 "Mod_StartMovie", "Mod_CacheNSSList", "EventQueue", "VarTable",
		 "Mod_MaxHenchmen", "Mod_StartMinute", "Mod_StartSecond",
		 "Mod_StartMiliSec", "Mod_Transition"};
	std::string tlabelss[] =
		{"Mod_Effect_NxtId", "Mod_NextCharId0", "Mod_NextCharId1", "Mod_NextObjId0",
		 "Mod_NextObjId1", "Mod_IsNWMFile", "Mod_Tokens", "Mod_PlayerList",
		 "Mod_TURDList"};
	std::vector<uint32> n, m;
	uint8 *data;
	uint32 size;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) savegame = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) expansions = top.values[n[1]];
	if(n[2] != NOTFOUND) if(gff.getExoString(n[2], top, tag)) return ERR(gff);
	if(n[3] != NOTFOUND) if(gff.getExoLocString(n[3], top, name)) return ERR(gff);
	if(n[4] != NOTFOUND) if(gff.getExoLocString(n[4], top, desc)) return ERR(gff);
	if(n[5] != NOTFOUND) version = top.values[n[5]];
	if(n[6] != NOTFOUND)
	{
		if(!(data = gff.getRaw(n[6], top, size))) return ERR(gff);
		if((size != 16) && (size != 32)) return errcode = 33;
		memset(id.mem, 0, 32);
		memcpy(id.mem, data, size);
		free(data);
	}
	if(n[7] != NOTFOUND)
		if(gff.getExoString(n[7], top, talktable)) return ERR(gff);
	if(n[8] != NOTFOUND) minperhour = top.values[n[8]];
	if(n[9] != NOTFOUND) dawnhour = top.values[n[9]];
	if(n[10] != NOTFOUND) duskhour = top.values[n[10]];
	if(n[11] != NOTFOUND) starthour = top.values[n[11]];
	if(n[12] != NOTFOUND) startday = top.values[n[12]];
	if(n[13] != NOTFOUND) startmonth = top.values[n[13]];
	if(n[14] != NOTFOUND) startyear = top.values[n[14]];
	if(n[15] != NOTFOUND) xpscale = top.values[n[15]];
	if(n[16] != NOTFOUND)
		if(gff.getResRef(n[16], top, entryarea)) return ERR(gff);
	if(n[17] != NOTFOUND) entryx = *((float32*) &top.values[n[17]]);
	if(n[18] != NOTFOUND) entryy = *((float32*) &top.values[n[18]]);
	if(n[19] != NOTFOUND) entryz = *((float32*) &top.values[n[19]]);
	if(n[20] != NOTFOUND) entrydirx = *((float32*) &top.values[n[20]]);
	if(n[21] != NOTFOUND) entrydiry = *((float32*) &top.values[n[21]]);
	if(n[22] != NOTFOUND)
		if(gff.getResRef(n[22], top, onacquireitem)) return ERR(gff);
	if(n[23] != NOTFOUND)
		if(gff.getResRef(n[23], top, onactivateitem)) return ERR(gff);
	if(n[24] != NOTFOUND)
		if(gff.getResRef(n[24], top, oncliententer)) return ERR(gff);
	if(n[25] != NOTFOUND)
		if(gff.getResRef(n[25], top, onclientleave)) return ERR(gff);
	if(n[26] != NOTFOUND)
		if(gff.getResRef(n[26], top, oncutsceneabort)) return ERR(gff);
	if(n[27] != NOTFOUND)
		if(gff.getResRef(n[27], top, onheartbeat)) return ERR(gff);
	if(n[28] != NOTFOUND)
		if(gff.getResRef(n[28], top, onmoduleload)) return ERR(gff);
	if(n[29] != NOTFOUND)
		if(gff.getResRef(n[29], top, onmodulestart)) return ERR(gff);
	if(n[30] != NOTFOUND)
		if(gff.getResRef(n[30], top, onplayerdeath)) return ERR(gff);
	if(n[31] != NOTFOUND)
		if(gff.getResRef(n[31], top, onplayerdying)) return ERR(gff);
	if(n[32] != NOTFOUND)
		if(gff.getResRef(n[32], top, onplayerequipitem)) return ERR(gff);
	if(n[33] != NOTFOUND)
		if(gff.getResRef(n[33], top, onplayerlevelup)) return ERR(gff);
	if(n[34] != NOTFOUND)
		if(gff.getResRef(n[34], top, onplayerrest)) return ERR(gff);
	if(n[35] != NOTFOUND)
		if(gff.getResRef(n[35], top, onplayerunequipitem)) return ERR(gff);
	if(n[36] != NOTFOUND)
		if(gff.getResRef(n[36], top, onplayerrespawn)) return ERR(gff);
	if(n[37] != NOTFOUND)
		if(gff.getResRef(n[37], top, onunacquireitem)) return ERR(gff);
	if(n[38] != NOTFOUND)
		if(gff.getResRef(n[38], top, onuserdefined)) return ERR(gff);
	if(n[39] != NOTFOUND)
		if(getAreas(gff, n[39], top)) return errcode;
	if(n[40] != NOTFOUND)
		if(gff.getExoString(n[40], top, mingamever)) return ERR(gff);
	if(n[41] != NOTFOUND) creatorid = top.values[n[41]];
	if(getHaks(gff, n[42], n[43], top)) return errcode;
	if(n[44] != NOTFOUND)
		if(gff.getResRef(n[44], top, startmovie)) return ERR(gff);
	if(n[45] != NOTFOUND) if(getCacheNss(gff, n[45], top)) return errcode;
	if(n[46] != NOTFOUND) if(getEvents(gff, n[46], top)) return errcode;
	if(n[47] != NOTFOUND)
		if(variables.load(gff, n[47], top)) return ERR(variables);
	if(n[48] != NOTFOUND) maxhenchmen = top.values[n[48]];
	if(n[49] != NOTFOUND) startminute = top.values[n[49]];
	if(n[50] != NOTFOUND) startsecond = top.values[n[50]];
	if(n[51] != NOTFOUND) startmilisecond = top.values[n[51]];
	if(n[52] != NOTFOUND) transition = top.values[n[52]];
	if(savegame)
	{
		if(gff.getIdxByLabel(tlabelss, sizeof(tlabelss)/sizeof(std::string),
					top, n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) nexteffectid = top.values[n[0]];
		if(n[1] != NOTFOUND) nextcharid0 = top.values[n[1]];
		if(n[2] != NOTFOUND) nextcharid1 = top.values[n[2]];
		if(n[3] != NOTFOUND) nextobjid0 = top.values[n[3]];
		if(n[4] != NOTFOUND) nextobjid1 = top.values[n[4]];
		if(n[5] != NOTFOUND) nwm = (bool) top.values[n[5]];
		if(nwm)
		{
			aderrstr = "Mod_NWMResName";
			uint32 n = gff.getIdxByLabel(aderrstr, top);
			if((n == ERROR) || (n == NOTFOUND)) return errcode = 20;
			if(gff.getExoString(n, top, nwmresname)) return ERR(gff);
			aderrstr.clear();
		}
		if(n[6] != NOTFOUND) if(getTokens(gff, n[6], top)) return errcode;
		if(n[7] != NOTFOUND) if(getPlayers(gff, n[7], top)) return errcode;
		if(n[8] != NOTFOUND) if(getTurds(gff, n[8], top)) return errcode;
	}
	checkRanges();
	return errcode = 0;
}

int ModuleIFO::getHaks(GFFFile &gff, uint32 hln, uint32 hn,
		GFFFile::Struct &top)
{
	if(hln != NOTFOUND)
	{
		std::vector<GFFFile::Struct> scts;
		if(gff.getList(hln, top, scts)) return ERR(gff);
		haks.resize(scts.size());
		aderrstr = "Mod_Hak";
		for(uint32 i=0;i<scts.size();i++)
			if(gff.getExoString(aderrstr, scts[i], haks[i])) return ERR(gff);
		aderrstr.clear();
	}
	else if(hn != NOTFOUND)
	{
		haks.resize(1);
		if(gff.getExoString(hn, top, haks[0])) return ERR(gff);
	}

	return errcode = 0;
}

int ModuleIFO::getAreas(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(aln, top, scts)) return ERR(gff);
	areas.resize(scts.size());
	if(savegame) areaids.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		aderrstr = "Area_Name";
		if(gff.getResRef(aderrstr, scts[i], areas[i])) return ERR(gff);
		if(savegame)
		{
			aderrstr = "ObjectId";
			uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
			if((n == ERROR) || (n == NOTFOUND)) return errcode = 20;
			areaids[i] = scts[i].values[n];
		}
		aderrstr.clear();
	}

	return errcode = 0;
}

int ModuleIFO::getCacheNss(GFFFile &gff, uint32 cln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(cln, top, scts)) return ERR(gff);
	tokens.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		aderrstr = "ResRef";
		if(gff.getResRef(aderrstr, scts[i], areas[i])) return ERR(gff);
		aderrstr.clear();
	}
	return errcode = 0;
}

int ModuleIFO::getTokens(GFFFile &gff, uint32 tln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(tln, top, scts)) return ERR(gff);
	tokens.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		aderrstr = "Mod_TokensNumber";
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if((n == ERROR) || (n == NOTFOUND)) return errcode = 20;
		tokens[i].number = scts[i].values[n];
		aderrstr = "Mod_TokensValue";
		if(gff.getExoString(aderrstr, scts[i], tokens[i].value))
			return ERR(gff);
		aderrstr.clear();
	}
	return errcode = 0;
}

int ModuleIFO::getEvents(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(eln, top, scts)) return ERR(gff);
	events.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(events[i].load(gff, scts[i])) return ERR(events[i]);
	return errcode = 0;
}

int ModuleIFO::getPlayers(GFFFile &gff, uint32 pln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(pln, top, scts)) return ERR(gff);
	players.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		if(players[i].load(gff, scts[i])) return ERR(events[i]);
		players[i].objectstate = NWN_OBJECT_SAVE;
		players[i].creaturestate = NWN_CREATURE_BIC;
	}
	return errcode = 0;
}

int ModuleIFO::getTurds(GFFFile &gff, uint32 tln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(tln, top, scts)) return ERR(gff);
	turds.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(turds[i].load(gff, scts[i])) return ERR(turds[i]);
	return errcode = 0;
}

void ModuleIFO::checkRanges(void)
{
	if(dawnhour > 23) dawnhour = 0;
	if(duskhour > 23) duskhour = 0;
	if(starthour > 23) starthour = 0;
	if((startday < 1) || (startday > 31)) startday = 1;
	if((startmonth < 1) || (startmonth > 24)) startmonth = 2;
	if(!minperhour) minperhour = 1;
	creatorid = 2;
	version = 3;
}


int ModuleIFO::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_IFO;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int ModuleIFO::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_IFO;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int ModuleIFO::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_IsSaveGame", savegame))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "Expansion_Pack", expansions))
		return ERR(gff);
	if(gff.writeExoString(top, "Mod_MinGameVer", mingamever)) return ERR(gff);
	if(gff.writeExoString(top, "Mod_Tag", tag)) return ERR(gff);
	if(gff.writeExoString(top, "Mod_CustomTlk", talktable)) return ERR(gff);
	if(gff.writeExoLocString(top, "Mod_Name", name)) return ERR(gff);
	if(gff.writeExoLocString(top, "Mod_Description", desc)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_DawnHour", dawnhour))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_DuskHour", duskhour))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_StartHour", starthour))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_StartDay", startday))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_StartMonth", startmonth))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_StartYear", startyear))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_MinPerHour", minperhour))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "Mod_StartMinute", startminute))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "Mod_StartSecond", startsecond))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "Mod_StartMiliSec", startmilisecond))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_Transition", transition))
		return ERR(gff);
	if(gff.writeFloat(top, "Mod_Entry_X", entryx)) return ERR(gff);
	if(gff.writeFloat(top, "Mod_Entry_Y", entryy)) return ERR(gff);
	if(gff.writeFloat(top, "Mod_Entry_Z", entryz)) return ERR(gff);
	if(gff.writeFloat(top, "Mod_Entry_Dir_X", entrydirx)) return ERR(gff);
	if(gff.writeFloat(top, "Mod_Entry_Dir_Y", entrydiry)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_Version", 3)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Mod_XPScale", xpscale))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Mod_Creator_ID", 2)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Mod_MaxHenchmen", maxhenchmen))
		return ERR(gff);
	if(gff.writeList(list, top, "Mod_HakList")) return ERR(gff);
	for(uint32 i=0;i<haks.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 8)) return ERR(gff);
		if(gff.writeExoString(sct, "Mod_Hak", haks[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, "Mod_Area_list")) return ERR(gff);
	if(savegame && (areaids.size() < areas.size())) areaids.resize(areas.size());
	for(uint32 i=0;i<areas.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 6)) return ERR(gff);
		if(gff.writeResRef(sct, "Area_Name", areas[i])) return ERR(gff);
		if(savegame)
			if(gff.writeVar(sct, NWN_VAR_UINT32, "ObjectId", areaids[i]))
				return ERR(gff);
	}
	if(gff.writeList(list, top, "Mod_CacheNSSList")) return ERR(gff);
	for(uint32 i=0;i<cachenss.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 9)) return ERR(gff);
		if(gff.writeResRef(sct, "ResRef", cachenss[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, "Mod_CutSceneList")) return ERR(gff);
	if(gff.writeList(list, top, "Mod_Expan_List")) return ERR(gff);
	if(gff.writeList(list, top, "Mod_GVar_List")) return ERR(gff);
	if(gff.writeResRef(top, "Mod_StartMovie", startmovie)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_Entry_Area", entryarea)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnAcquirItem", onacquireitem))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnActvtItem", onactivateitem))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnClientEntr", oncliententer))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnClientLeav", onclientleave))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnCutsnAbort", oncutsceneabort))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnHeartbeat", onheartbeat)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnModLoad", onmoduleload)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnModStart", onmodulestart)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnPlrDeath", onplayerdeath)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnPlrDying", onplayerdying)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnPlrEqItm", onplayerequipitem))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnPlrLvlUp", onplayerlevelup))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnPlrRest", onplayerrest)) return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnPlrUnEqItm", onplayerunequipitem))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnSpawnBtnDn", onplayerrespawn))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnUnAqreItem", onunacquireitem))
		return ERR(gff);
	if(gff.writeResRef(top, "Mod_OnUsrDefined", onuserdefined))
		return ERR(gff);
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	if(!tokens.empty())
	{
		if(gff.writeList(list, top, "Mod_Tokens")) return ERR(gff);
		for(uint32 i=0;i<tokens.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 7)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "Mod_TokensNumber",
						tokens[i].number))
				return ERR(gff);
			if(gff.writeExoString(sct, "Mod_TokensValue", tokens[i].value))
				return ERR(gff);
		}
	}
	if(savegame)
	{
		if(gff.writeData(top, "Mod_ID", id.mem, 32, false)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Mod_IsNWMFile", nwm)) return ERR(gff);
		if(nwm)
			if(gff.writeExoString(top, "Mod_NWMResName", nwmresname)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_NextCharId0", nextcharid0))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_NextCharId1", nextcharid1))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_NextObjId0", nextobjid0))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "Mod_NextObjId1", nextobjid1))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT64, "Mod_Effect_NxtId", nexteffectid))
			return ERR(gff);
		if(writeEvents(gff, top)) return errcode;
		if(writePlayers(gff, top)) return errcode;
		if(writeTurds(gff, top)) return errcode;
	}
	else if(gff.writeData(top, "Mod_ID", id.mem, 16, false)) return ERR(gff);

	return errcode = 0;
}

int ModuleIFO::writeEvents(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "EventQueue")) return ERR(gff);
	for(uint32 i=0;i<events.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 43981)) return ERR(gff);
		if(events[i].save(gff, sct)) return ERR(events[i]);
	}
	return errcode = 0;
}

int ModuleIFO::writePlayers(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "Mod_PlayerList")) return ERR(gff);
	for(uint32 i=0;i<players.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 48813)) return ERR(gff);
		players[i].objectstate = NWN_OBJECT_SAVE;
		players[i].creaturestate = NWN_CREATURE_BIC;
		if(players[i].save(gff, sct)) return ERR(players[i]);
	}
	return errcode = 0;
}

int ModuleIFO::writeTurds(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "Mod_TURDList")) return ERR(gff);
	for(uint32 i=0;i<turds.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 13634816)) return ERR(gff);
		if(turds[i].save(gff, sct)) return ERR(turds[i]);
	}
	return errcode = 0;
}

// vim:tabstop=2
