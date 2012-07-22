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

Situated::Situated()
{
	objectstate = NWN_OBJECT_INSTANCE;
	objectid = INVALID_OBJECT_ID;
	removekey = locked = lockable = plot = trapped = false;
	interruptable = trapdetectable = trapdisarmable = traponeshot = true;
	animationstate = hardness = openlockdc = closelockdc = traptype = 0;
	trapdetectdc = disarmdc = fortitude = will = reflex = paletteid = 0;
	portraitid = appearance = faction = animationday = animationtime = 0;
	x = y = z = bearing = 0.0;
	hp = currenthp = 1;
}

int Situated::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"AutoRemoveKey", "Interruptable", "Lockable", "Locked", "Plot", "TrapFlag",
		 "TrapDetectable", "TrapDisarmable", "TrapOneShot", "AnimationState",
		 "Hardness", "OpenLockDC", "CloseLockDC", "TrapType", "TrapDetectDC",
		 "DisarmDC", "Fort", "Will", "Ref", "PaletteID", "HP", "CurrentHP",
		 "PortraitId", "Appearance", "Faction", "AnimationDay", "AnimationTime",
		 "ObjectId", "X", "Y", "Z", "Bearing", "TemplateResRef", "Conversation",
		 "Tag", "Comment", "OnClosed", "OnDamaged", "OnDeath", "OnDisarm",
		 "OnHeartbeat", "OnLock", "OnMeleeAttacked", "OnOpen", "OnSpellCastAt",
		 "OnTrapTriggered", "OnUnlock", "OnUserDefined", "LocName", "Description",
		 "VarTable", "ActionList", "EffectList", "KeyRequired", "KeyName"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) removekey = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) interruptable = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) lockable = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) locked = (bool) top.values[n[3]];
	if(n[4] != NOTFOUND) plot = (bool) top.values[n[4]];
	if(n[5] != NOTFOUND) trapped = (bool) top.values[n[5]];
	if(n[6] != NOTFOUND) trapdetectable = (bool) top.values[n[6]];
	if(n[7] != NOTFOUND) trapdisarmable = (bool) top.values[n[7]];
	if(n[8] != NOTFOUND) traponeshot = (bool) top.values[n[8]];
	if(n[9] != NOTFOUND) animationstate = top.values[n[9]];
	if(n[10] != NOTFOUND) hardness = top.values[n[10]];
	if(n[11] != NOTFOUND) openlockdc = top.values[n[11]];
	if(n[12] != NOTFOUND) closelockdc = top.values[n[12]];
	if(n[13] != NOTFOUND) traptype = top.values[n[13]];
	if(n[14] != NOTFOUND) trapdetectdc = top.values[n[14]];
	if(n[15] != NOTFOUND) disarmdc = top.values[n[15]];
	if(n[16] != NOTFOUND) fortitude = top.values[n[16]];
	if(n[17] != NOTFOUND) will = top.values[n[17]];
	if(n[18] != NOTFOUND) reflex = top.values[n[18]];
	if(n[19] != NOTFOUND) paletteid = top.values[n[19]];
	if(n[20] != NOTFOUND) hp = top.values[n[20]];
	if(n[21] != NOTFOUND) currenthp = top.values[n[21]];
	if(n[22] != NOTFOUND) portraitid = top.values[n[22]];
	if(n[23] != NOTFOUND) appearance = top.values[n[23]];
	if(n[24] != NOTFOUND) faction = top.values[n[24]];
	if(n[25] != NOTFOUND) animationday = top.values[n[25]];
	if(n[26] != NOTFOUND) animationtime = top.values[n[26]];
	if(n[27] != NOTFOUND) objectid = top.values[n[27]];
	if(n[28] != NOTFOUND) x = *((float32 *) &top.values[n[28]]);
	if(n[29] != NOTFOUND) y = *((float32 *) &top.values[n[29]]);
	if(n[30] != NOTFOUND) z = *((float32 *) &top.values[n[30]]);
	if(n[31] != NOTFOUND) bearing = *((float32 *) &top.values[n[31]]);
	if(n[32] != NOTFOUND)
		if(gff.getResRef(n[32], top, templateresref)) return ERR(gff);
	if(n[33] != NOTFOUND)
		if(gff.getResRef(n[33], top, conversation)) return ERR(gff);
	if(n[34] != NOTFOUND)
		if(gff.getExoString(n[34], top, tag)) return ERR(gff);
	if(n[35] != NOTFOUND)
		if(gff.getExoString(n[35], top, comment)) return ERR(gff);
	if(n[36] != NOTFOUND)
		if(gff.getResRef(n[36], top, onclosed)) return ERR(gff);
	if(n[37] != NOTFOUND)
		if(gff.getResRef(n[37], top, ondamaged)) return ERR(gff);
	if(n[38] != NOTFOUND)
		if(gff.getResRef(n[38], top, ondeath)) return ERR(gff);
	if(n[39] != NOTFOUND)
		if(gff.getResRef(n[39], top, ondisarm)) return ERR(gff);
	if(n[40] != NOTFOUND)
		if(gff.getResRef(n[40], top, onheartbeat)) return ERR(gff);
	if(n[41] != NOTFOUND)
		if(gff.getResRef(n[41], top, onlock)) return ERR(gff);
	if(n[42] != NOTFOUND)
		if(gff.getResRef(n[42], top, onmeleeattacked)) return ERR(gff);
	if(n[43] != NOTFOUND)
		if(gff.getResRef(n[43], top, onopen)) return ERR(gff);
	if(n[44] != NOTFOUND)
		if(gff.getResRef(n[44], top, onspellcastat)) return ERR(gff);
	if(n[45] != NOTFOUND)
		if(gff.getResRef(n[45], top, ontraptriggered)) return ERR(gff);
	if(n[46] != NOTFOUND)
		if(gff.getResRef(n[46], top, onunlock)) return ERR(gff);
	if(n[47] != NOTFOUND)
		if(gff.getResRef(n[47], top, onuserdefined)) return ERR(gff);
	if(n[48] != NOTFOUND)
		if(gff.getExoLocString(n[48], top, name)) return ERR(gff);
	if(n[49] != NOTFOUND)
		if(gff.getExoLocString(n[49], top, description)) return ERR(gff);
	if(n[50] != NOTFOUND)
		if(variables.load(gff, n[50], top)) return ERR(variables);
	if(n[51] != NOTFOUND) if(getActions(gff, n[51], top)) return errcode;
	if(n[52] != NOTFOUND) if(getEffects(gff, n[52], top)) return errcode;
	if(n[53] != NOTFOUND) keyrequired = (bool) top.values[n[53]];
	if(n[54] != NOTFOUND)
		if(gff.getExoString(n[54], top, keyname)) return ERR(gff);

	return errcode = 0;
}

int Situated::getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(eln, top, scts)) return ERR(gff);
	effects.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(effects[i].load(gff, scts[i])) return ERR(effects[i]);
	return errcode = 0;
}

int Situated::getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(aln, top, scts)) return ERR(gff);
	actions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(actions[i].load(gff, scts[i])) return ERR(actions[i]);
	return errcode = 0;
}

void Situated::checkRanges(void)
{
	if(tag.length() > 32) tag.erase(32);
}

int Situated::save(GFFFile &gff, uint32 top)
{
	uint32 list;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "AutoRemoveKey", removekey))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Interruptable", interruptable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Lockable", lockable)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Locked", locked)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Plot", plot)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapFlag", trapped)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapDetectable", trapdetectable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapDisarmable", trapdisarmable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapOneShot", traponeshot))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "KeyRequired", keyrequired))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "AnimationState", animationstate))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Hardness", hardness)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "OpenLockDC", openlockdc))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "CloseLockDC", closelockdc))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapType", traptype)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "TrapDetectDC", trapdetectdc))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "DisarmDC", disarmdc)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Fort", fortitude)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Will", will)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Ref", reflex)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "HP", hp)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "CurrentHP", currenthp)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "PortraitId", portraitid))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Appearance", appearance))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Faction", faction)) return ERR(gff);
	if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeResRef(top, "Conversation", conversation)) return ERR(gff);
	if(gff.writeResRef(top, "OnClosed", onclosed)) return ERR(gff);
	if(gff.writeResRef(top, "OnDamaged", ondamaged)) return ERR(gff);
	if(gff.writeResRef(top, "OnDeath", ondeath)) return ERR(gff);
	if(gff.writeResRef(top, "OnDisarm", ondisarm)) return ERR(gff);
	if(gff.writeResRef(top, "OnHeartbeat", onheartbeat)) return ERR(gff);
	if(gff.writeResRef(top, "OnLock", onlock)) return ERR(gff);
	if(gff.writeResRef(top, "OnMeleeAttacked", onmeleeattacked)) return ERR(gff);
	if(gff.writeResRef(top, "OnOpen", onopen)) return ERR(gff);
	if(gff.writeResRef(top, "OnSpellCastAt", onspellcastat)) return ERR(gff);
	if(gff.writeResRef(top, "OnTrapTriggered", ontraptriggered)) return ERR(gff);
	if(gff.writeResRef(top, "OnUnlock", onunlock)) return ERR(gff);
	if(gff.writeResRef(top, "OnUserDefined", onuserdefined)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(gff.writeExoString(top, "KeyName", keyname)) return ERR(gff);
	if(gff.writeExoLocString(top, "LocName", name)) return ERR(gff);
	if(gff.writeExoLocString(top, "Description", description)) return ERR(gff);
	switch(objectstate)
	{
		case NWN_OBJECT_BLUEPRINT:
			if(gff.writeVar(top, NWN_VAR_INT8, "PaletteID", paletteid))
				return ERR(gff);
			if(gff.writeExoString(top, "Comment", comment)) return ERR(gff);
			break;
		case NWN_OBJECT_SAVE:
			if(gff.writeVar(top, NWN_VAR_UINT32, "AnimationDay", animationday))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "AnimationTime", animationtime))
				return ERR(gff);
			if(gff.writeVar(top, NWN_VAR_UINT32, "ObjectId", objectid))
				return ERR(gff);
			if(writeActions(gff, top)) return errcode;
			if(writeEffects(gff, top)) return errcode;
		case NWN_OBJECT_INSTANCE:
			if(gff.writeFloat(top, "X", x)) return ERR(gff);
			if(gff.writeFloat(top, "Y", y)) return ERR(gff);
			if(gff.writeFloat(top, "Z", z)) return ERR(gff);
			if(gff.writeFloat(top, "Bearing", bearing)) return ERR(gff);
			break;
	}
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Situated::writeActions(GFFFile &gff, uint32 top)
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

int Situated::writeEffects(GFFFile &gff, uint32 top)
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
