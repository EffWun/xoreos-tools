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

Encounter::Encounter()
{
	objectstate = NWN_OBJECT_INSTANCE;
	spawnoption = ENCOUNTER_SPAWN_SINGLE;
	objectid = INVALID_OBJECT_ID;
	active = playeronly = reset = started = false;
	difficulty = exhausted = paletteid = 0;
	resettime = respawns = arealistmaxsize = currentspawns = 0;
	customscriptid = numberspawned = difficultyindex = faction = 0;
	heartbeatday = heartbeattime = 0;
	lastentered = lastleft = lastspawnday = lastspawntime = 0;
	x = y = z = areapoints = spawnpoolactive = 0.0;
	reccreatures = 1;
	maxcreatures = 8;
}

int Encounter::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Encounter::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Encounter::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Active", "PlayerOnly", "Reset", "Started", "MaxCreatures",
		 "RecCreatures", "Exhausted", "PaletteID", "ResetTime", "Respawns",
		 "AreaListMaxSize", "CurrentSpawns", "CustomScriptId", "NumberSpawned",
		 "Difficulty", "DifficultyIndex", "Faction", "SpawnOption", "HeartbeatDay",
		 "HeartbeatTime", "LastEntered", "LastLeft", "LastSpawnDay",
		 "LastSpawnTime", "ObjectId", "XPosition", "YPosition", "ZPosition",
		 "AreaPoints", "SpawnPoolActive", "Tag", "Comment", "OnEntered",
		 "OnExhausted", "OnExit", "OnHeartbeat", "OnUserDefined", "TemplateResRef",
		 "LocalizedName", "CreatureList", "Geometry", "SpawnPointList",
		 "ActionList", "VarTable"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) active = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) playeronly = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) reset = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) started = (bool) top.values[n[3]];
	if(n[4] != NOTFOUND) maxcreatures = top.values[n[4]];
	if(n[5] != NOTFOUND) reccreatures = top.values[n[5]];
	if(n[6] != NOTFOUND) exhausted = top.values[n[6]];
	if(n[7] != NOTFOUND) paletteid = top.values[n[7]];
	if(n[8] != NOTFOUND) resettime = top.values[n[8]];
	if(n[9] != NOTFOUND) respawns = top.values[n[9]];
	if(n[10] != NOTFOUND) arealistmaxsize = top.values[n[10]];
	if(n[11] != NOTFOUND) currentspawns = top.values[n[11]];
	if(n[12] != NOTFOUND) customscriptid = top.values[n[12]];
	if(n[13] != NOTFOUND) numberspawned = top.values[n[13]];
	if(n[14] != NOTFOUND) difficulty = top.values[n[14]];
	if(n[15] != NOTFOUND) difficultyindex = top.values[n[15]];
	if(n[16] != NOTFOUND) faction = top.values[n[16]];
	if(n[17] != NOTFOUND) spawnoption = top.values[n[17]];
	if(n[18] != NOTFOUND) heartbeatday = top.values[n[18]];
	if(n[19] != NOTFOUND) heartbeattime = top.values[n[19]];
	if(n[20] != NOTFOUND) lastentered = top.values[n[20]];
	if(n[21] != NOTFOUND) lastleft = top.values[n[21]];
	if(n[22] != NOTFOUND) lastspawnday = top.values[n[22]];
	if(n[23] != NOTFOUND) lastspawntime = top.values[n[23]];
	if(n[24] != NOTFOUND) objectid = top.values[n[24]];
	if(n[25] != NOTFOUND) x = *((float32*) &top.values[n[25]]);
	if(n[26] != NOTFOUND) y = *((float32*) &top.values[n[26]]);
	if(n[27] != NOTFOUND) z = *((float32*) &top.values[n[27]]);
	if(n[28] != NOTFOUND) areapoints = *((float32*) &top.values[n[28]]);
	if(n[29] != NOTFOUND) spawnpoolactive = *((float32*) &top.values[n[29]]);
	if(n[30] != NOTFOUND)
		if(gff.getExoString(n[30], top, tag)) return ERR(gff);
	if(n[31] != NOTFOUND)
		if(gff.getExoString(n[31], top, comment)) return ERR(gff);
	if(n[32] != NOTFOUND)
		if(gff.getResRef(n[32], top, onentered)) return ERR(gff);
	if(n[33] != NOTFOUND)
		if(gff.getResRef(n[33], top, onexhausted)) return ERR(gff);
	if(n[34] != NOTFOUND)
		if(gff.getResRef(n[34], top, onexit)) return ERR(gff);
	if(n[35] != NOTFOUND)
		if(gff.getResRef(n[35], top, onheartbeat)) return ERR(gff);
	if(n[36] != NOTFOUND)
		if(gff.getResRef(n[36], top, onuserdefined)) return ERR(gff);
	if(n[37] != NOTFOUND)
		if(gff.getResRef(n[37], top, templateresref)) return ERR(gff);
	if(n[38] != NOTFOUND)
		if(gff.getExoLocString(n[38], top, name)) return ERR(gff);
	if(n[39] != NOTFOUND) if(getCreatures(gff, n[39], top)) return errcode;
	if(n[40] != NOTFOUND) if(getGeometry(gff, n[40], top)) return errcode;
	if(n[41] != NOTFOUND) if(getSpawnPoints(gff, n[41], top)) return errcode;
	if(n[42] != NOTFOUND) if(getActions(gff, n[42], top)) return errcode;
	if(n[43] != NOTFOUND)
		if(variables.load(gff, n[43], top)) return ERR(variables);
	checkRanges();

	return errcode = 0;
}

int Encounter::getGeometry(GFFFile &gff, uint32 gln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"PointX", "PointY", "PointZ"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(gln, top, scts)) return ERR(gff);
	points.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&points[i], 0, sizeof(Point));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) points[i].x = *((float32*) &scts[i].values[n[0]]);
		if(n[1] != NOTFOUND) points[i].y = *((float32*) &scts[i].values[n[1]]);
		if(n[2] != NOTFOUND) points[i].z = *((float32*) &scts[i].values[n[2]]);
	}
	return errcode = 0;
}

int Encounter::getSpawnPoints(GFFFile &gff, uint32 spln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"X", "Y", "Z", "Orientation"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(spln, top, scts)) return ERR(gff);
	spawnpoints.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&spawnpoints[i], 0, sizeof(SpawnPoint));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) spawnpoints[i].x = *((float32*) &scts[i].values[n[0]]);
		if(n[1] != NOTFOUND) spawnpoints[i].y = *((float32*) &scts[i].values[n[1]]);
		if(n[2] != NOTFOUND) spawnpoints[i].z = *((float32*) &scts[i].values[n[2]]);
		if(n[3] != NOTFOUND)
			spawnpoints[i].orientation = *((float32*) &scts[i].values[n[3]]);
	}
	return errcode = 0;
}

int Encounter::getCreatures(GFFFile &gff, uint32 cln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"SingleSpawn", "Appearance", "CR", "ResRef"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(cln, top, scts)) return ERR(gff);
	creatures.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		creatures[i].singlespawn = false;
		creatures[i].appearance = 0;
		creatures[i].cr = 0.0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND)
			creatures[i].singlespawn = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) creatures[i].appearance = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) creatures[i].cr = *((float32*) &scts[i].values[n[2]]);
		if(n[3] != NOTFOUND)
			if(gff.getResRef(n[3], scts[i], creatures[i].resref)) return ERR(gff);
	}
	return errcode = 0;
}

int Encounter::getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(aln, top, scts)) return ERR(gff);
	actions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(actions[i].load(gff, scts[i])) return ERR(actions[i]);
	return errcode = 0;
}

void Encounter::checkRanges(void)
{
	if(maxcreatures > 8) maxcreatures = 8;
	if(maxcreatures < 1) maxcreatures = 1;
	if(reccreatures > 8) reccreatures = 8;
	if(reccreatures < 1) reccreatures = 1;
	if(maxcreatures < reccreatures) maxcreatures = reccreatures;
	if(reccreatures > maxcreatures) reccreatures = maxcreatures;
	if(resettime > 32000) resettime = 32000;
	if(resettime < -1) resettime = -1;
	if(spawnoption > 1) spawnoption = 1;
	if(tag.length() > 32) tag.erase(32);
}

int Encounter::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTE;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Encounter::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTE;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Encounter::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "Active", active)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "PlayerOnly", playeronly))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Reset", reset)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "MaxCreatures", maxcreatures))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "RecCreatures", reccreatures))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ResetTime", resettime))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Respawns", respawns)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Difficulty", difficulty))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "DifficultyIndex", difficultyindex))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "SpawnOption", spawnoption))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Faction", faction)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeResRef(top, "OnEntered", onentered)) return ERR(gff);
	if(gff.writeResRef(top, "OnExhausted", onexhausted)) return ERR(gff);
	if(gff.writeResRef(top, "OnExit", onexit)) return ERR(gff);
	if(gff.writeResRef(top, "OnHeartbeat", onheartbeat)) return ERR(gff);
	if(gff.writeResRef(top, "OnUserDefined", onuserdefined)) return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocalizedName", name)) return ERR(gff);
	if(gff.writeList(list, top, "CreatureList")) return ERR(gff);
	for(uint32 i=0;i<creatures.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "SingleSpawn",
					creatures[i].singlespawn))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "Appearance", creatures[i].appearance))
			return ERR(gff);
		if(gff.writeFloat(sct, "CR", creatures[i].cr)) return ERR(gff);
		if(gff.writeResRef(sct, "ResRef", creatures[i].resref)) return ERR(gff);
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
			if(gff.writeVar(top, NWN_VAR_INT8, "Exhausted", exhausted))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_INT8, "Started", started))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_INT32, "AreaListMaxSize", arealistmaxsize))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_INT32, "CurrentSpawns", currentspawns))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_INT32, "CustomScriptId", customscriptid))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_INT32, "NumberSpawned", numberspawned))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "HeartbeatDay", heartbeatday))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "HeartbeatTime", heartbeattime))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "LastEntered", lastentered))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "LastLeft", lastleft))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "LastSpawnDay", lastspawnday))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "LastSpawnTime", lastspawntime))
				return ERR(gff);
			if(gff.writeFloat(top, "AreaPoints", areapoints)) return ERR(gff);
			if(gff.writeFloat(top, "SpawnPoolActive", spawnpoolactive))
				return ERR(gff);
			if(writeActions(gff, top)) return errcode;
		case NWN_OBJECT_INSTANCE:
			if(gff.writeFloat(top, "XPosition", x)) return ERR(gff);
			if(gff.writeFloat(top, "YPosition", y)) return ERR(gff);
			if(gff.writeFloat(top, "ZPosition", z)) return ERR(gff);
			if(gff.writeList(list, top, "Geometry")) return ERR(gff);
			for(uint32 i=0;i<points.size();i++)
			{
				if(gff.writeListStruct(sct, list)) return ERR(gff);
				if(gff.writeSetId(sct, 1)) return ERR(gff);
				if(gff.writeFloat(sct, "X", points[i].x)) return ERR(gff);
				if(gff.writeFloat(sct, "Y", points[i].y)) return ERR(gff);
				if(gff.writeFloat(sct, "Z", points[i].z)) return ERR(gff);
			}
			if(spawnpoints.empty()) break;
			if(gff.writeList(list, top, "SpawnPointList")) return ERR(gff);
			for(uint32 i=0;i<spawnpoints.size();i++)
			{
				if(gff.writeListStruct(sct, list)) return ERR(gff);
				if(gff.writeSetId(sct, 0)) return ERR(gff);
				if(gff.writeFloat(sct, "X", spawnpoints[i].x)) return ERR(gff);
				if(gff.writeFloat(sct, "Y", spawnpoints[i].y)) return ERR(gff);
				if(gff.writeFloat(sct, "Z", spawnpoints[i].z)) return ERR(gff);
				if(gff.writeFloat(sct, "Orientation", spawnpoints[i].orientation))
					return ERR(gff);
			}
			break;
	}
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Encounter::writeActions(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "ActionList")) return ERR(gff);
	for(uint32 i=0;i<actions.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(actions[i].save(gff, sct)) return ERR(actions[i]);
	}
	return errcode = 0;
}

// vim:tabstop=2
