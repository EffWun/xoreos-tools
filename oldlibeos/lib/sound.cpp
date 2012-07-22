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

Sound::Sound()
{
	objectstate = NWN_OBJECT_INSTANCE;
	objectid = INVALID_OBJECT_ID;
	active = continuous = random = randomposition = false;
	looping = positional = generatedtype = false;
	priority = volumevrtn = paletteid = hours = interval = intervalvrtn = 0;
	x = y = z = pitchvariation = randomrangex = randomrangey = 0.0;
	elevation = mindistance = 1.0;
	maxdistance = 2.0;
	times = 3;
	volume = 127;
}

int Sound::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Sound::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Sound::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Active", "Continuous", "Random", "RandomPosition", "Looping",
		 "Positional", "GeneratedType", "Priority", "Times", "Volume",
		 "VolumeVrtn", "PaletteID", "Hours", "Interval", "IntervalVrtn",
		 "ObjectId", "Elevation", "MaxDistance", "MinDistance", "XPosition",
		 "YPosition", "ZPosition", "PitchVariation", "RandomRangeX",
		 "RandomRangeY", "Tag", "Comment", "TemplateResRef", "LocName",
		 "Sounds", "ActionList", "VarTable"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) active = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) continuous = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) random = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) randomposition = (bool) top.values[n[3]];
	if(n[4] != NOTFOUND) looping = (bool) top.values[n[4]];
	if(n[5] != NOTFOUND) positional = (bool) top.values[n[5]];
	if(n[6] != NOTFOUND) generatedtype = (bool) top.values[n[6]];
	if(n[7] != NOTFOUND) priority = top.values[n[7]];
	if(n[8] != NOTFOUND) times = top.values[n[8]];
	if(n[9] != NOTFOUND) volume = top.values[n[9]];
	if(n[10] != NOTFOUND) volumevrtn = top.values[n[10]];
	if(n[11] != NOTFOUND) paletteid = top.values[n[11]];
	if(n[12] != NOTFOUND) hours = top.values[n[12]];
	if(n[13] != NOTFOUND) interval = top.values[n[13]];
	if(n[14] != NOTFOUND) intervalvrtn = top.values[n[14]];
	if(n[15] != NOTFOUND) objectid = top.values[n[15]];
	if(n[16] != NOTFOUND) elevation = *((float32 *) &top.values[n[16]]);
	if(n[17] != NOTFOUND) maxdistance = *((float32 *) &top.values[n[17]]);
	if(n[18] != NOTFOUND) mindistance = *((float32 *) &top.values[n[18]]);
	if(n[19] != NOTFOUND) x = *((float32 *) &top.values[n[19]]);
	if(n[20] != NOTFOUND) y = *((float32 *) &top.values[n[20]]);
	if(n[21] != NOTFOUND) z = *((float32 *) &top.values[n[21]]);
	if(n[22] != NOTFOUND) pitchvariation = *((float32 *) &top.values[n[22]]);
	if(n[23] != NOTFOUND) randomrangex = *((float32 *) &top.values[n[23]]);
	if(n[24] != NOTFOUND) randomrangey = *((float32 *) &top.values[n[24]]);
	if(n[25] != NOTFOUND)
		if(gff.getExoString(n[25], top, tag)) return ERR(gff);
	if(n[26] != NOTFOUND)
		if(gff.getExoString(n[26], top, comment)) return ERR(gff);
	if(n[27] != NOTFOUND)
		if(gff.getResRef(n[27], top, templateresref)) return ERR(gff);
	if(n[28] != NOTFOUND)
		if(gff.getExoLocString(n[28], top, name)) return ERR(gff);
	if(n[29] != NOTFOUND) if(getSounds(gff, n[29], top)) return errcode;
	if(n[30] != NOTFOUND) if(getActions(gff, n[29], top)) return errcode;
	if(n[31] != NOTFOUND)
		if(variables.load(gff, n[32], top)) return ERR(variables);
	checkRanges();

	return errcode = 0;
}

int Sound::getSounds(GFFFile &gff, uint32 sln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Sound";
	if(gff.getList(sln, top, scts)) return ERR(gff);
	sounds.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(gff.getResRef(aderrstr, scts[i], sounds[i])) return ERR(gff);
	aderrstr.clear();
	return errcode = 0;
}

int Sound::getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(aln, top, scts)) return ERR(gff);
	actions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(actions[i].load(gff, scts[i])) return ERR(actions[i]);
	return errcode = 0;
}

void Sound::checkRanges(void)
{
	if(continuous)
	{
		random = randomposition = false;
		interval = intervalvrtn = 0;
		pitchvariation = randomrangex = randomrangey = 0.0;
	}
	if(volume > 127) volume = 127;
	if(volumevrtn > 127) volumevrtn = 127;
	if(times > 3) times = 3;
	if(maxdistance < mindistance) maxdistance = mindistance;
	if(tag.length() > 32) tag.erase(32);
}

int Sound::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTS;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Sound::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTS;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Sound::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "Active", active)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Continuous", continuous))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Random", random)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "RandomPosition", randomposition))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Looping", looping)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Positional", positional))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Priority", priority)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Times", times)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Volume", volume)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "VolumeVrtn", volumevrtn))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Hours", hours)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Interval", interval)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "IntervalVrtn", intervalvrtn))
		return ERR(gff);
	if(gff.writeFloat(top, "PitchVariation", pitchvariation)) return ERR(gff);
	if(gff.writeFloat(top, "Elevation", elevation)) return ERR(gff);
	if(gff.writeFloat(top, "MinDistance", mindistance)) return ERR(gff);
	if(gff.writeFloat(top, "MaxDistance", maxdistance)) return ERR(gff);
	if(gff.writeFloat(top, "RandomRangeX", randomrangex)) return ERR(gff);
	if(gff.writeFloat(top, "RandomRangeY", randomrangey)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocName", name)) return ERR(gff);
	if(gff.writeList(list, top, "Sounds")) return ERR(gff);
	for(uint32 i=0;i<sounds.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeResRef(sct, "Sound", sounds[i])) return ERR(gff);
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
			if(writeActions(gff, top)) return errcode;
		case NWN_OBJECT_INSTANCE:
			if(gff.writeVar(top, NWN_VAR_UINT8, "GeneratedType", generatedtype))
				return ERR(gff);
			if(gff.writeFloat(top, "XPosition", x)) return ERR(gff);
			if(gff.writeFloat(top, "YPosition", y)) return ERR(gff);
			if(gff.writeFloat(top, "ZPosition", z)) return ERR(gff);
			break;
	}
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Sound::writeActions(GFFFile &gff, uint32 top)
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
