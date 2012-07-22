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

Area::Area()
{
	objectstate = NWN_OBJECT_INSTANCE;
	isnight = norest = weatherstarted = false;
	daynight = sunshadow = moonshadow = true;
	clightning = crain = csnow = 0;
	loadscreen = lightscheme = skybox = windpower = pvp = 0;
	sunambient = sundiffuse = sunfog = sunfogcolor = 0;
	moonambient = moondiffuse = moonfog = moonfogcolor = 0;
	width = height = modlisten = modspot = 0;
	currentweather = ambientday = ambientnight = envaudio = 0;
	musicbattle = musicday = musicnight = musicdelay = 0;
	version = 1;
	fogclipdist = 45.0;
	shadowopacity = 100;
	ambientdayvol = ambientnightvol = 127;
}

int Area::load(std::string arefile, std::string gitfile, std::string gicfile)
{
	std::string files[] = {arefile, gitfile, gicfile};
	int (Area::*funcs[])(GFFFile &, GFFFile::Struct &) =
		{&Area::loadAre, &Area::loadGit, &Area::loadGic};

	for(uint32 i=0;i<3;i++)
		if(!files[i].empty())
		{
			GFFFile gff;
			if(gff.open(files[i])) return ERR(gff);
			if(gff.init()) return ERR(gff);
			if((this->*funcs[i])(gff, gff.topstruct)) return errcode;
		}
	checkRanges();
	return errcode = 0;
}

int Area::load(uint8 *are, uint32 aresize, uint8 *git, uint32 gitsize,
		uint8 *gic, uint32 gicsize)
{
	uint8 *files[] = {are, git, gic};
	uint32 sizes[] = {aresize, gitsize, gicsize};
	int (Area::*funcs[])(GFFFile &, GFFFile::Struct &) =
		{&Area::loadAre, &Area::loadGit, &Area::loadGic};

	for(uint32 i=0;i<3;i++)
		if(files[i] && sizes[i])
		{
			GFFFile gff;
			if(gff.open(files[i], sizes[i])) return ERR(gff);
			if(gff.init()) return ERR(gff);
			if((this->*funcs[i])(gff, gff.topstruct)) return errcode;
		}
	checkRanges();
	return errcode = 0;
}

int Area::load(MemSeg &are, MemSeg &git, MemSeg &gic)
{
	return load(are.mem, are.size, git.mem, git.size, gic.mem, gic.size);
}

int Area::load(GFFFile *are, GFFFile::Struct *aretop,
               GFFFile *git, GFFFile::Struct *gittop,
               GFFFile *gic, GFFFile::Struct *gictop)
{
	GFFFile *files[] = {are, git, gic};
	GFFFile::Struct *tops[] = {aretop, gittop, gictop};
	int (Area::*funcs[])(GFFFile &, GFFFile::Struct &) =
		{&Area::loadAre, &Area::loadGit, &Area::loadGic};

	for(uint32 i=0;i<3;i++)
		if(files[i] && tops[i])
			if((this->*funcs[i])(*files[i], *tops[i])) return errcode;
	checkRanges();
	return errcode = 0;
}

int Area::loadAre(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Flags", "DayNightCycle", "IsNight", "SunShadows", "MoonShadows", "NoRest",
		 "SkyBox", "LightingScheme", "PlayerVsPlayer", "SunFogAmount",
		 "MoonFogAmount", "ShadowOpacity", "LoadScreenID", "ModListenCheck",
		 "ModSpotCheck", "Version", "Width", "Height", "WindPower",
		 "ChanceLightning", "ChanceRain", "ChanceSnow", "SunAmbientColor",
		 "SunDiffuseColor", "SunFogColor", "MoonAmbientColor", "MoonDiffuseColor",
		 "MoonFogColor", "FogClipDist", "ResRef", "Tileset", "OnEnter", "OnExit",
		 "OnHeartbeat", "OnUserDefined", "Tag", "Comments", "Name", "Tile_List"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) flags = (uint16) top.values[n[0]];
	if(n[1] != NOTFOUND) daynight = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) isnight = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) sunshadow = (bool) top.values[n[3]];
	if(n[4] != NOTFOUND) moonshadow = (bool) top.values[n[4]];
	if(n[5] != NOTFOUND) norest = (bool) top.values[n[5]];
	if(n[6] != NOTFOUND) skybox = top.values[n[6]];
	if(n[7] != NOTFOUND) lightscheme = top.values[n[7]];
	if(n[8] != NOTFOUND) pvp = top.values[n[8]];
	if(n[9] != NOTFOUND) sunfog = top.values[n[9]];
	if(n[10] != NOTFOUND) moonfog = top.values[n[10]];
	if(n[11] != NOTFOUND) shadowopacity = top.values[n[11]];
	if(n[12] != NOTFOUND) loadscreen = top.values[n[12]];
	if(n[13] != NOTFOUND) modlisten = top.values[n[13]];
	if(n[14] != NOTFOUND) modspot = top.values[n[14]];
	if(n[15] != NOTFOUND) version = top.values[n[15]];
	if(n[16] != NOTFOUND) width = top.values[n[16]];
	if(n[17] != NOTFOUND) height = top.values[n[17]];
	if(n[18] != NOTFOUND) windpower = top.values[n[18]];
	if(n[19] != NOTFOUND) clightning = top.values[n[19]];
	if(n[20] != NOTFOUND) crain = top.values[n[20]];
	if(n[21] != NOTFOUND) csnow = top.values[n[21]];
	if(n[22] != NOTFOUND) sunambient = top.values[n[22]];
	if(n[23] != NOTFOUND) sundiffuse = top.values[n[23]];
	if(n[24] != NOTFOUND) sunfogcolor = top.values[n[24]];
	if(n[25] != NOTFOUND) moonambient = top.values[n[25]];
	if(n[26] != NOTFOUND) moondiffuse = top.values[n[26]];
	if(n[27] != NOTFOUND) moonfogcolor = top.values[n[27]];
	if(n[28] != NOTFOUND) fogclipdist = *((float32*) &top.values[n[28]]);
	if(n[29] != NOTFOUND) if(gff.getResRef(n[29], top, resref)) return ERR(gff);
	if(n[30] != NOTFOUND) if(gff.getResRef(n[30], top, tileset)) return ERR(gff);
	if(n[31] != NOTFOUND) if(gff.getResRef(n[31], top, onenter)) return ERR(gff);
	if(n[32] != NOTFOUND) if(gff.getResRef(n[32], top, onexit)) return ERR(gff);
	if(n[33] != NOTFOUND)
		if(gff.getResRef(n[33], top, onheartbeat)) return ERR(gff);
	if(n[34] != NOTFOUND)
		if(gff.getResRef(n[34], top, onuserdefined)) return ERR(gff);
	if(n[35] != NOTFOUND) if(gff.getExoString(n[35], top, tag)) return ERR(gff);
	if(n[36] != NOTFOUND)
		if(gff.getExoString(n[36], top, comments)) return ERR(gff);
	if(n[37] != NOTFOUND)
		if(gff.getExoLocString(n[37], top, name)) return ERR(gff);
	if(n[38] != NOTFOUND) if(getTiles(gff, n[38], top)) return errcode;
	return errcode = 0;
}

int Area::loadGit(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"AreaProperties", "Creature List", "Door List", "Encounter List", "List",
		 "Placeable List", "SoundList", "StoreList", "TriggerList", "WaypointList"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) if(getProperties(gff, top.values[n[0]])) return errcode;
	if(n[1] != NOTFOUND) if(getCreatures(gff, n[1], top)) return errcode;
	if(n[2] != NOTFOUND) if(getDoors(gff, n[2], top)) return errcode;
	if(n[3] != NOTFOUND) if(getEncounters(gff, n[3], top)) return errcode;
	if(n[4] != NOTFOUND) if(getItems(gff, n[4], top)) return errcode;
	if(n[5] != NOTFOUND) if(getPlaceables(gff, n[5], top)) return errcode;
	if(n[6] != NOTFOUND) if(getSounds(gff, n[6], top)) return errcode;
	if(n[7] != NOTFOUND) if(getStores(gff, n[7], top)) return errcode;
	if(n[8] != NOTFOUND) if(getTriggers(gff, n[8], top)) return errcode;
	if(n[9] != NOTFOUND) if(getWaypoints(gff, n[9], top)) return errcode;
	return errcode = 0;
}

int Area::loadGic(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Creature List", "Door List", "Encounter List", "List", "Placeable List",
		 "SoundList", "StoreList", "TriggerList", "WaypointList"};
	std::vector<std::string> *comments[] =
		{&comcreatures, &comdoors, &comencounters, &comitems, &complaceables,
		 &comsounds, &comstores, &comtriggers, &comwaypoints};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	for(uint32 i=0;i<n.size();i++)
		if(n[i] != NOTFOUND)
			if(getComments(gff, n[i], *comments[i], top)) return errcode;

	return errcode = 0;
}

int Area::getComments(GFFFile &gff, uint32 cln, std::vector<std::string> &comments,
		GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(cln, top, scts)) return ERR(gff);
	comments.resize(scts.size());
	aderrstr = "Comment";
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND)
			if(gff.getExoString(n, scts[i], comments[i])) return ERR(gff);
	}
	aderrstr.clear();

	return errcode = 0;
}

int Area::getTiles(GFFFile &gff, uint32 tln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Tile_AnimLoop1", "Tile_AnimLoop2", "Tile_AnimLoop3", "Tile_MainLight1",
		 "Tile_MainLight2", "Tile_SrcLight1", "Tile_SrcLight2", "Tile_Orientation",
		 "Tile_Height", "Tile_ID"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(tln, top, scts)) return ERR(gff);
	tiles.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&tiles[i], 0, sizeof(AreaTile));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) tiles[i].animloop1 = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) tiles[i].animloop2 = (bool) scts[i].values[n[1]];
		if(n[2] != NOTFOUND) tiles[i].animloop3 = (bool) scts[i].values[n[2]];
		if(n[3] != NOTFOUND) tiles[i].mainlight1 = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) tiles[i].mainlight2 = scts[i].values[n[4]];
		if(n[5] != NOTFOUND) tiles[i].srclight1 = scts[i].values[n[5]];
		if(n[6] != NOTFOUND) tiles[i].srclight2 = scts[i].values[n[6]];
		if(n[7] != NOTFOUND) tiles[i].orientation = scts[i].values[n[7]];
		if(n[8] != NOTFOUND) tiles[i].height = scts[i].values[n[8]];
		if(n[9] != NOTFOUND) tiles[i].id = scts[i].values[n[9]];
	}
	return errcode = 0;
}

int Area::getProperties(GFFFile &gff, uint32 psn)
{
	std::string tlabels[] =
		{"WeatherStarted", "AmbientSndDayVol", "AmbientSndNitVol",
		 "CurrentWeather", "AmbientSndDay", "AmbientSndNight", "EnvAudio",
		 "MusicBattle", "MusicDay", "MusicNight", "MusicDelay", "AreaEffectList",
		 "VarTable"};
	std::vector<uint32> n;
	GFFFile::Struct psct;

	if(gff.getStruct(psn, psct)) return ERR(gff);
	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				psct, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) weatherstarted = (bool) psct.values[n[0]];
	if(n[1] != NOTFOUND) ambientdayvol = psct.values[n[1]];
	if(n[2] != NOTFOUND) ambientnightvol = psct.values[n[2]];
	if(n[3] != NOTFOUND) currentweather = psct.values[n[3]];
	if(n[4] != NOTFOUND) ambientday = psct.values[n[4]];
	if(n[5] != NOTFOUND) ambientnight = psct.values[n[5]];
	if(n[6] != NOTFOUND) envaudio = psct.values[n[6]];
	if(n[7] != NOTFOUND) musicbattle = psct.values[n[7]];
	if(n[8] != NOTFOUND) musicday = psct.values[n[8]];
	if(n[9] != NOTFOUND) musicnight = psct.values[n[9]];
	if(n[10] != NOTFOUND) musicdelay = psct.values[n[10]];
	if(n[11] != NOTFOUND) if(getEffects(gff, n[11], psct)) return errcode;
	if(n[12] != NOTFOUND)
		if(variables.load(gff, n[12], psct)) return ERR(variables);
	return errcode = 0;
}

int Area::getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(eln, top, scts)) return ERR(gff);
	effects.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(effects[i].load(gff, scts[i])) return ERR(effects[i]);
	return errcode = 0;
}

void Area::checkRanges(void)
{
	if(clightning > 100) clightning = 100;
	if(crain > 100) crain = 100;
	if(csnow > 100) csnow = 100;
	if(shadowopacity > 100) shadowopacity = 100;
	if(sunfog > 15) sunfog = 15;
	if(moonfog > 15) moonfog = 15;
	if(windpower > 2) windpower = 2;
	if(ambientdayvol > 127) ambientdayvol = 127;
	if(ambientnightvol > 127) ambientnightvol = 127;
	if(currentweather > 2) currentweather = 2;
	if(tag.length() > 32) tag.erase(32);
}

int Area::getCreatures(GFFFile &gff, uint32 cln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(cln, top, scts)) return ERR(gff);
	creatures.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		creatures[i].objectstate = objectstate;
		if(creatures[i].load(gff, scts[i])) return ERR(creatures[i]);
	}
	return errcode = 0;
}

int Area::getDoors(GFFFile &gff, uint32 dln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(dln, top, scts)) return ERR(gff);
	doors.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		doors[i].objectstate = objectstate;
		if(doors[i].load(gff, scts[i])) return ERR(doors[i]);
	}
	return errcode = 0;
}

int Area::getEncounters(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(eln, top, scts)) return ERR(gff);
	encounters.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		encounters[i].objectstate = objectstate;
		if(encounters[i].load(gff, scts[i])) return ERR(encounters[i]);
	}
	return errcode = 0;
}

int Area::getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(iln, top, scts)) return ERR(gff);
	items.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		items[i].objectstate = objectstate;
		items[i].itemstate = NWN_ITEM_NORMAL;
		if(items[i].load(gff, scts[i])) return ERR(items[i]);
	}
	return errcode = 0;
}

int Area::getPlaceables(GFFFile &gff, uint32 pln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(pln, top, scts)) return ERR(gff);
	placeables.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		placeables[i].objectstate = objectstate;
		if(placeables[i].load(gff, scts[i])) return ERR(placeables[i]);
	}
	return errcode = 0;
}

int Area::getSounds(GFFFile &gff, uint32 sln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(sln, top, scts)) return ERR(gff);
	sounds.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		sounds[i].objectstate = objectstate;
		if(sounds[i].load(gff, scts[i])) return ERR(sounds[i]);
	}
	return errcode = 0;
}

int Area::getStores(GFFFile &gff, uint32 sln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(sln, top, scts)) return ERR(gff);
	stores.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		stores[i].objectstate = objectstate;
		if(stores[i].load(gff, scts[i])) return ERR(stores[i]);
	}
	return errcode = 0;
}

int Area::getTriggers(GFFFile &gff, uint32 tln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(tln, top, scts)) return ERR(gff);
	triggers.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		triggers[i].objectstate = objectstate;
		if(triggers[i].load(gff, scts[i])) return ERR(triggers[i]);
	}
	return errcode = 0;
}

int Area::getWaypoints(GFFFile &gff, uint32 wln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(wln, top, scts)) return ERR(gff);
	waypoints.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		waypoints[i].objectstate = objectstate;
		if(waypoints[i].load(gff, scts[i])) return ERR(waypoints[i]);
	}
	return errcode = 0;
}

int Area::save(std::string arefile, std::string gitfile, std::string gicfile)
{
	uint32 top;

	checkRanges();
	if(!arefile.empty())
	{
		GFFFile gff;
		if(gff.beginWrite(arefile)) return ERR(gff);
		gff.type = NWN_FILE_ARE;
		if(gff.writeStruct(top, "", 0)) return ERR(gff);
		if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
		if(saveAre(gff, top)) return errcode;
		if(gff.endwrite()) return ERR(gff);
	}
	if(!gitfile.empty())
	{
		GFFFile gff;
		if(gff.beginWrite(gitfile)) return ERR(gff);
		gff.type = NWN_FILE_GIT;
		if(gff.writeStruct(top, "", 0)) return ERR(gff);
		if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
		if(saveGit(gff, top)) return errcode;
		if(gff.endwrite()) return ERR(gff);
	}
	if(!gicfile.empty() && (objectstate != NWN_OBJECT_SAVE))
	{
		GFFFile gff;
		if(gff.beginWrite(gicfile)) return ERR(gff);
		gff.type = NWN_FILE_GIC;
		if(gff.writeStruct(top, "", 0)) return ERR(gff);
		if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
		if(saveGic(gff, top)) return errcode;
		if(gff.endwrite()) return ERR(gff);
	}
	return errcode = 0;
}

int Area::save(uint8 **are, uint32 *aresize, uint8 **git, uint32 *gitsize,
		uint8 **gic, uint32 *gicsize)
{
	uint32 top;

	checkRanges();
	if(are && aresize)
	{
		GFFFile gff;
		if(gff.beginWrite()) return ERR(gff);
		gff.type = NWN_FILE_ARE;
		if(gff.writeStruct(top, "", 0)) return ERR(gff);
		if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
		if(saveAre(gff, top)) return errcode;
		if(gff.endwrite(are, aresize)) return ERR(gff);
	}
	if(git && gitsize)
	{
		GFFFile gff;
		if(gff.beginWrite()) return ERR(gff);
		gff.type = NWN_FILE_GIT;
		if(gff.writeStruct(top, "", 0)) return ERR(gff);
		if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
		if(saveGit(gff, top)) return errcode;
		if(gff.endwrite(git, gitsize)) return ERR(gff);
	}
	if(gic && gicsize && (objectstate != NWN_OBJECT_SAVE))
	{
		GFFFile gff;
		if(gff.beginWrite()) return ERR(gff);
		gff.type = NWN_FILE_GIC;
		if(gff.writeStruct(top, "", 0)) return ERR(gff);
		if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
		if(saveGic(gff, top)) return errcode;
		if(gff.endwrite(gic, gicsize)) return ERR(gff);
	}
	return errcode = 0;
	return errcode = 0;
}

int Area::save(GFFFile &are, uint32 aretop, GFFFile &git, uint32 gittop,
		GFFFile &gic, uint32 gictop)
{
	checkRanges();
	if(saveAre(are, aretop)) return errcode;
	if(saveGit(git, gittop)) return errcode;
	if(saveGic(gic, gictop)) return errcode;
	return errcode = 0;
}

int Area::saveAre(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeVar(top, NWN_VAR_INT8, "DayNightCycle", daynight))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "IsNight", isnight)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "SunShadows", sunshadow)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "NoRest", norest)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "MoonShadows", moonshadow))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "SkyBox", skybox)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "LightingScheme", lightscheme))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "MoonFogAmount", moonfog)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "PlayerVsPlayer", pvp)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "ShadowOpacity", shadowopacity))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "SunFogAmount", sunfog)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "LoadScreenID", loadscreen))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "MoonAmbientColor", moonambient))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "MoonDiffuseColor", moondiffuse))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Flags", (uint16) flags))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "MoonFogColor", moonfogcolor))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "SunAmbientColor", sunambient))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "SunDiffuseColor", sundiffuse))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "SunFogColor", sunfog)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Version", version)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ChanceLightning", clightning))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ChanceRain", crain)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ChanceSnow", csnow)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Creator_ID", 0xFFFFFFFF))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Height", height)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ID", 0xFFFFFFFF)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ModListenCheck", modlisten))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "ModSpotCheck", modspot)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Width", width))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "WindPower", windpower))
		return ERR(gff);
	if(gff.writeFloat(top, "FogClipDist", fogclipdist)) return ERR(gff);
	if(gff.writeResRef(top, "ResRef", resref)) return ERR(gff);
	if(gff.writeResRef(top, "Tileset", tileset)) return ERR(gff);
	if(gff.writeResRef(top, "OnEnter", onenter)) return ERR(gff);
	if(gff.writeResRef(top, "OnExit", onexit)) return ERR(gff);
	if(gff.writeResRef(top, "OnHeartbeat", onheartbeat)) return ERR(gff);
	if(gff.writeResRef(top, "OnUserDefined", onuserdefined)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeExoString(top, "Comments", comments)) return ERR(gff);
	if(gff.writeExoLocString(top, "Name", name)) return ERR(gff);
	if(gff.writeList(list, top, "Tile_List")) return ERR(gff);
	for(uint32 i=0;i<tiles.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 1)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_AnimLoop1", tiles[i].animloop1))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_AnimLoop2", tiles[i].animloop2))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_AnimLoop3", tiles[i].animloop3))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_MainLight1", tiles[i].mainlight1))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_MainLight2", tiles[i].mainlight2))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_SrcLight1", tiles[i].srclight1))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Tile_SrcLight2", tiles[i].srclight2))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "Tile_ID", tiles[i].id))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "Tile_Orientation",
					tiles[i].orientation))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "Tile_Height", tiles[i].height))
			return ERR(gff);
	}

	return errcode = 0;
}

int Area::saveGit(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeStruct(sct, "AreaProperties", top)) return ERR(gff);
	if(gff.writeSetId(sct, 100)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "AmbientSndDay", ambientday))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "AmbientSndDayVol", ambientdayvol))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "AmbientSndNight", ambientnight))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "AmbientSndNitVol", ambientnightvol))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "EnvAudio", envaudio)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "MusicBattle", musicbattle))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "MusicDay", musicday)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "MusicNight", musicnight))
		return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT32, "MusicDelay", musicdelay))
		return ERR(gff);
	if(objectstate == NWN_OBJECT_SAVE)
	{
		if(gff.writeVar(sct, NWN_VAR_INT8, "CurrentWeather", currentweather))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "WeatherStarted", weatherstarted))
			return ERR(gff);
		if(writeEffects(gff, top)) return errcode;
	}
	if(gff.writeList(list, top, "Creature List")) return ERR(gff);
	for(uint32 i=0;i<creatures.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 4)) return ERR(gff);
		creatures[i].objectstate = objectstate;
		if(creatures[i].save(gff, sct)) return ERR(creatures[i]);
	}
	if(gff.writeList(list, top, "Door List")) return ERR(gff);
	for(uint32 i=0;i<doors.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 8)) return ERR(gff);
		doors[i].objectstate = objectstate;
		if(doors[i].save(gff, sct)) return ERR(doors[i]);
	}
	if(gff.writeList(list, top, "Encounter List")) return ERR(gff);
	for(uint32 i=0;i<encounters.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 7)) return ERR(gff);
		encounters[i].objectstate = objectstate;
		if(encounters[i].save(gff, sct)) return ERR(encounters[i]);
	}
	if(gff.writeList(list, top, "List")) return ERR(gff);
	for(uint32 i=0;i<items.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		items[i].objectstate = objectstate;
		if(items[i].save(gff, sct)) return ERR(items[i]);
	}
	if(gff.writeList(list, top, "Placeable List")) return ERR(gff);
	for(uint32 i=0;i<placeables.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 9)) return ERR(gff);
		placeables[i].objectstate = objectstate;
		if(placeables[i].save(gff, sct)) return ERR(placeables[i]);
	}
	if(gff.writeList(list, top, "SoundList")) return ERR(gff);
	for(uint32 i=0;i<sounds.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 6)) return ERR(gff);
		sounds[i].objectstate = objectstate;
		if(sounds[i].save(gff, sct)) return ERR(sounds[i]);
	}
	if(gff.writeList(list, top, "StoreList")) return ERR(gff);
	for(uint32 i=0;i<stores.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 11)) return ERR(gff);
		stores[i].objectstate = objectstate;
		if(stores[i].save(gff, sct)) return ERR(stores[i]);
	}
	if(gff.writeList(list, top, "TriggerList")) return ERR(gff);
	for(uint32 i=0;i<triggers.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 1)) return ERR(gff);
		triggers[i].objectstate = objectstate;
		if(triggers[i].save(gff, sct)) return ERR(triggers[i]);
	}
	if(gff.writeList(list, top, "WaypointList")) return ERR(gff);
	for(uint32 i=0;i<waypoints.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 5)) return ERR(gff);
		waypoints[i].objectstate = objectstate;
		if(waypoints[i].save(gff, sct)) return ERR(waypoints[i]);
	}


	if(!variables.empty())
	{
		if(gff.writeList(list, sct, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Area::saveGic(GFFFile &gff, uint32 top)
{
	std::string tlabels[] =
		{"Creature List", "Door List", "Encounter List", "List", "Placeable List",
		 "SoundList", "StoreList", "TriggerList", "WaypointList"};
	std::vector<std::string> *comments[] =
		{&comcreatures, &comdoors, &comencounters, &comitems, &complaceables,
		 &comsounds, &comstores, &comtriggers, &comwaypoints};
	uint32 ids[] = {4, 8, 7, 0, 9, 6, 11, 1, 5};
	uint32 list;

	for(uint32 i=0;i<9;i++)
	{
		if(gff.writeList(list, top, tlabels[i])) return ERR(gff);
		if(writeComments(gff, list, ids[i], *comments[i])) return ERR(gff);
	}

	return errcode = 0;
}

int Area::writeComments(GFFFile &gff, uint32 cln, uint32 id,
		std::vector<std::string> &comments)
{
	uint32 sct;

	for(uint32 i=0;i<comments.size();i++)
	{
		if(gff.writeListStruct(sct, cln)) return ERR(gff);
		if(gff.writeSetId(sct, id)) return ERR(gff);
		if(gff.writeExoString(sct, "Comment", comments[i])) return ERR(gff);
	}
	return errcode = 0;
}

int Area::writeEffects(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "AreaEffectList")) return ERR(gff);
	for(uint32 i=0;i<effects.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 13)) return ERR(gff);
		if(effects[i].save(gff, sct)) return ERR(effects[i]);
	}
	return errcode = 0;
}

// vim:tabstop=2
