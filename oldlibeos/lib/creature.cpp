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

Creature::Creature()
{
	objectstate = NWN_OBJECT_INSTANCE;
	creaturestate = NWN_CREATURE_NORMAL;
	creatureappearance = NWN_CREATURE_SUBDIV;
	objectid = INVALID_OBJECT_ID;
	commandable = dm = pc = disarmable = plot = immortal = lootable = false;
	nopermdeath = deadselectable = raiseable = stealthmode = detectmode = false;
	mod_isprimaryplr = pm_ispolymorphed = false;
	destroyable = interruptable = true;
	fortitudesavethrow = reflexsavethrow = willsavethrow = 0;
	gender = race = goodevil = lawfulchaotic = phenotype = 0;
	baseattackbonus = ac = encounterobject = movementrate = 0;
	tail = wings = bodybag = startingpackage = perceptionrange = 0;
	paletteid = ambientanimstate = overridebab = listening = mclasslevupin = 0;
	fortitudebonus = reflexbonus = willbonus = creaturesize = pregamecurrent = 0;
	appearance = skillpoints = soundset = factionid = portraitid = 0;
	cradjust = walkrate = familiartype = mod_mapnumareas = 0;
	experience = gold = animationday = animationtime = decaytime = 0;
	bodybagid = areaid = masterid = sitobject = 0;
	challengerating = x = y = z = dirx = diry = 0.0;
	numattacks = spellresistance = arcanespellfail = armorcheckpen = 0;
	unarmeddamdice = unarmeddamdie = onhandcritrng = onhandcritmult = 0;
	offhandweaponeq = offhandcritrng = offhandcritmult = creaturedice = 0;
	creaturedie = reaturedice = reaturedie = eaturedice = eaturedie = 0;
	damagedice = damagedie = 0;
	onhandattackmod = onhanddamagemod = offhandattackmod = offhanddamagemod = 0;
	leftequip = rightequip = 0;
	bodyhead = bodyneck = bodytorso = bodybelt = bodypelvis = 0;
	bodylshoul = bodylbicep = bodylfarm = bodylhand = 0;
	bodyrshoul = bodyrbicep = bodyrfarm = bodyrhand = 0;
	bodylthigh = bodylshin = bodylfoot = 0;
	bodyrthigh = bodyrshin = bodyrfoot = 0;
	colorskin = colorhair = colortattoo1 = colortattoo2 = 0;
	maxhitpoints = hitpoints = currenthitpoints = age = 1;
	strength = dexterity = constitution = intelligence = wisdom = charisma = 10;
	naturalac = 10;
}

int Creature::load(std::string fname)
{
	GFFFile gff;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(gff.type == NWN_FILE_BIC)
	{
		creaturestate = NWN_CREATURE_BIC;
		objectstate = NWN_OBJECT_INSTANCE;
	}
	else if(gff.type == NWN_FILE_UTC)
	{
		creaturestate = NWN_CREATURE_NORMAL;
		objectstate = NWN_OBJECT_BLUEPRINT;
	}
	else return errcode = 11;
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Creature::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(gff.type == NWN_FILE_BIC)
	{
		creaturestate = NWN_CREATURE_BIC;
		objectstate = NWN_OBJECT_INSTANCE;
	}
	else if(gff.type == NWN_FILE_UTC)
	{
		creaturestate = NWN_CREATURE_NORMAL;
		objectstate = NWN_OBJECT_BLUEPRINT;
	}
	else return errcode = 11;
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Creature::load(GFFFile &gff, GFFFile::Struct &top)
{
	// . . .
	std::string tlabels[] =
		{"IsCommandable", "IsDM", "IsPC", "IsDestroyable", "Plot", "IsImmortal",
		 "Interruptable", "Disarmable", "Lootable", "NoPermDeath",
		 "DeadSelectable", "IsRaiseable", "StealthMode", "DetectMode", "Phenotype",
		 "Mod_IsPrimaryPlr", "PM_IsPolymorphed", "FortSaveThrow", "RefSaveThrow",
		 "WillSaveThrow", "Gender", "Race", "GoodEvil", "LawfulChaotic", "Str",
		 "Dex", "Con", "Int", "Wis", "Cha", "NaturalAC", "BaseAttackBonus", "Tail",
		 "Wings", "BodyBag", "StartingPackage", "PerceptionRange", "PaletteID",
		 "AmbientAnimState", "OverrideBAB", "Listening", "MClassLevUpIn",
		 "MaxHitPoints", "HitPoints", "CurrentHitPoints", "fortbonus", "refbonus",
		 "willbonus", "CreatureSize", "PregameCurrent", "Appearance_Type",
		 "SkillPoints", "SoundSetFile", "FactionID", "PortraitId", "Age",
		 "CRAdjust", "WalkRate", "FamiliarType", "Mod_MapNumAreas", "Experience",
		 "Gold", "AnimationDay", "AnimationTime", "DecayTime", "BodyBagId",
		 "ObjectId", "AreaId", "MasterID", "SitObject", "ChallengeRating",
		 "XPosition", "YPosition", "ZPosition", "XOrientation", "YOrientation",
		 "Subrace", "Deity", "FamiliarName", "Tag", "Comment", "Mod_CommntyName",
		 "Conversation", "TemplateResRef", "EquipRes", "ScriptAttacked",
		 "ScriptDamaged", "ScriptDeath", "ScriptDialogue", "ScriptDisturbed",
		 "ScriptEndRound", "ScriptHeartbeat", "ScriptOnBlocked", "ScriptOnNotice",
		 "ScriptRested", "ScriptSpawn", "ScriptSpellAt", "ScriptUserDefine",
		 "Description", "FirstName", "LastName", "Mod_FirstName", "Mod_LastName",
		 "ClassList", "ItemList", "Equip_ItemList", "SkillList", "FeatList",
		 "SpecAbilityList", "ActionList", "EffectList", "ExpressionList",
		 "PerceptionList", "PersonalRepList", "VarTable", "QBList",
		 "Mod_MapDataList", "ReputationList", "CombatInfo", "CombatRoundData",
		 "Mod_MapAreasData", "ArmorClass", "MovementRate", "EncounterObject",
		 "Portrait", "Appearance_Head", "BodyPart_Neck", "BodyPart_Torso",
		 "BodyPart_Belt", "BodyPart_Pelvis", "BodyPart_LShoul", "BodyPart_LBicep",
		 "BodyPart_LFArm", "BodyPart_LHand", "BodyPart_RShoul", "BodyPart_RBicep",
		 "BodyPart_RFArm", "BodyPart_RHand", "BodyPart_LThigh", "BodyPart_LShin",
		 "BodyPart_LFoot", "BodyPart_RThigh", "BodyPart_RShin", "ArmorPart_RFoot",
		 "Color_Skin", "Color_Hair", "Color_Tattoo1", "Color_Tattoo2",
		 "LvlStatList"};

	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) commandable = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) dm = (bool) top.values[n[1]];
	if(n[2] != NOTFOUND) pc = (bool) top.values[n[2]];
	if(n[3] != NOTFOUND) destroyable = (bool) top.values[n[3]];
	if(n[4] != NOTFOUND) plot = (bool) top.values[n[4]];
	if(n[5] != NOTFOUND) immortal = (bool) top.values[n[5]];
	if(n[6] != NOTFOUND) interruptable = (bool) top.values[n[6]];
	if(n[7] != NOTFOUND) disarmable = (bool) top.values[n[7]];
	if(n[8] != NOTFOUND) lootable = (bool) top.values[n[8]];
	if(n[9] != NOTFOUND) nopermdeath = (bool) top.values[n[9]];
	if(n[10] != NOTFOUND) deadselectable = (bool) top.values[n[10]];
	if(n[11] != NOTFOUND) raiseable = (bool) top.values[n[11]];
	if(n[12] != NOTFOUND) stealthmode = (bool) top.values[n[12]];
	if(n[13] != NOTFOUND) detectmode = (bool) top.values[n[13]];
	if(n[14] != NOTFOUND) phenotype = top.values[n[14]];
	if(n[15] != NOTFOUND) mod_isprimaryplr = (bool) top.values[n[15]];
	if(n[16] != NOTFOUND) pm_ispolymorphed = (bool) top.values[n[16]];
	if(n[17] != NOTFOUND) fortitudesavethrow = top.values[n[17]];
	if(n[18] != NOTFOUND) reflexsavethrow = top.values[n[18]];
	if(n[19] != NOTFOUND) willsavethrow = top.values[n[19]];
	if(n[20] != NOTFOUND) gender = top.values[n[20]];
	if(n[21] != NOTFOUND) race = top.values[n[21]];
	if(n[22] != NOTFOUND) goodevil = top.values[n[22]];
	if(n[23] != NOTFOUND) lawfulchaotic = top.values[n[23]];
	if(n[24] != NOTFOUND) strength = top.values[n[24]];
	if(n[25] != NOTFOUND) dexterity = top.values[n[25]];
	if(n[26] != NOTFOUND) constitution = top.values[n[26]];
	if(n[27] != NOTFOUND) intelligence = top.values[n[27]];
	if(n[28] != NOTFOUND) wisdom = top.values[n[28]];
	if(n[29] != NOTFOUND) charisma = top.values[n[29]];
	if(n[30] != NOTFOUND) naturalac = top.values[n[30]];
	if(n[31] != NOTFOUND) baseattackbonus = top.values[n[31]];
	if(n[32] != NOTFOUND) tail = top.values[n[32]];
	if(n[33] != NOTFOUND) wings = top.values[n[33]];
	if(n[34] != NOTFOUND) bodybag = top.values[n[34]];
	if(n[35] != NOTFOUND) startingpackage = top.values[n[35]];
	if(n[36] != NOTFOUND) perceptionrange = top.values[n[36]];
	if(n[37] != NOTFOUND) paletteid = top.values[n[37]];
	if(n[38] != NOTFOUND) ambientanimstate = top.values[n[38]];
	if(n[39] != NOTFOUND) overridebab = top.values[n[39]];
	if(n[40] != NOTFOUND) listening = top.values[n[40]];
	if(n[41] != NOTFOUND) mclasslevupin = top.values[n[41]];
	if(n[42] != NOTFOUND) maxhitpoints = top.values[n[42]];
	if(n[43] != NOTFOUND) hitpoints = top.values[n[43]];
	if(n[44] != NOTFOUND) currenthitpoints = top.values[n[44]];
	if(n[45] != NOTFOUND) fortitudebonus = top.values[n[45]];
	if(n[46] != NOTFOUND) reflexbonus = top.values[n[46]];
	if(n[47] != NOTFOUND) willbonus = top.values[n[47]];
	if(n[48] != NOTFOUND) creaturesize = top.values[n[48]];
	if(n[49] != NOTFOUND) pregamecurrent = top.values[n[49]];
	if(n[50] != NOTFOUND) appearance = top.values[n[50]];
	if(n[51] != NOTFOUND) skillpoints = top.values[n[51]];
	if(n[52] != NOTFOUND) soundset = top.values[n[52]];
	if(n[53] != NOTFOUND) factionid = top.values[n[53]];
	if(n[54] != NOTFOUND) portraitid = top.values[n[54]];
	if(n[55] != NOTFOUND) age = top.values[n[55]];
	if(n[56] != NOTFOUND) cradjust = top.values[n[56]];
	if(n[57] != NOTFOUND) walkrate = top.values[n[57]];
	if(n[58] != NOTFOUND) familiartype = top.values[n[58]];
	if(n[59] != NOTFOUND) mod_mapnumareas = top.values[n[59]];
	if(n[60] != NOTFOUND) experience = top.values[n[60]];
	if(n[61] != NOTFOUND) gold = top.values[n[61]];
	if(n[62] != NOTFOUND) animationday = top.values[n[62]];
	if(n[63] != NOTFOUND) animationtime = top.values[n[63]];
	if(n[64] != NOTFOUND) decaytime = top.values[n[64]];
	if(n[65] != NOTFOUND) bodybagid = top.values[n[65]];
	if(n[66] != NOTFOUND) objectid = top.values[n[66]];
	if(n[67] != NOTFOUND) areaid = top.values[n[67]];
	if(n[68] != NOTFOUND) masterid = top.values[n[68]];
	if(n[69] != NOTFOUND) sitobject = top.values[n[69]];
	if(n[70] != NOTFOUND) challengerating = *((float32*) &top.values[n[70]]);
	if(n[71] != NOTFOUND) x = *((float32*) &top.values[n[71]]);
	if(n[72] != NOTFOUND) y = *((float32*) &top.values[n[72]]);
	if(n[73] != NOTFOUND) z = *((float32*) &top.values[n[73]]);
	if(n[74] != NOTFOUND) dirx = *((float32*) &top.values[n[74]]);
	if(n[75] != NOTFOUND) diry = *((float32*) &top.values[n[75]]);
	if(n[76] != NOTFOUND)
		if(gff.getExoString(n[76], top, subrace)) return ERR(gff);
	if(n[77] != NOTFOUND)
		if(gff.getExoString(n[77], top, deity)) return ERR(gff);
	if(n[78] != NOTFOUND)
		if(gff.getExoString(n[78], top, familiarname)) return ERR(gff);
	if(n[79] != NOTFOUND)
		if(gff.getExoString(n[79], top, tag)) return ERR(gff);
	if(n[80] != NOTFOUND)
		if(gff.getExoString(n[80], top, comment)) return ERR(gff);
	if(n[81] != NOTFOUND)
		if(gff.getExoString(n[81], top, mod_commntyname)) return ERR(gff);
	if(n[82] != NOTFOUND)
		if(gff.getResRef(n[82], top, conversation)) return ERR(gff);
	if(n[83] != NOTFOUND)
		if(gff.getResRef(n[83], top, templateresref)) return ERR(gff);
	if(n[84] != NOTFOUND)
		if(gff.getResRef(n[84], top, equipres)) return ERR(gff);
	if(n[85] != NOTFOUND)
		if(gff.getResRef(n[85], top, scriptattacked)) return ERR(gff);
	if(n[86] != NOTFOUND)
		if(gff.getResRef(n[86], top, scriptdamaged)) return ERR(gff);
	if(n[87] != NOTFOUND)
		if(gff.getResRef(n[87], top, scriptdeath)) return ERR(gff);
	if(n[88] != NOTFOUND)
		if(gff.getResRef(n[88], top, scriptdialogue)) return ERR(gff);
	if(n[89] != NOTFOUND)
		if(gff.getResRef(n[89], top, scriptdisturbed)) return ERR(gff);
	if(n[90] != NOTFOUND)
		if(gff.getResRef(n[90], top, scriptendround)) return ERR(gff);
	if(n[91] != NOTFOUND)
		if(gff.getResRef(n[91], top, scriptheartbeat)) return ERR(gff);
	if(n[92] != NOTFOUND)
		if(gff.getResRef(n[92], top, scriptonblocked)) return ERR(gff);
	if(n[93] != NOTFOUND)
		if(gff.getResRef(n[93], top, scriptonnotice)) return ERR(gff);
	if(n[94] != NOTFOUND)
		if(gff.getResRef(n[94], top, scriptrested)) return ERR(gff);
	if(n[95] != NOTFOUND)
		if(gff.getResRef(n[95], top, scriptspawn)) return ERR(gff);
	if(n[96] != NOTFOUND)
		if(gff.getResRef(n[96], top, scriptspellat)) return ERR(gff);
	if(n[97] != NOTFOUND)
		if(gff.getResRef(n[97], top, scriptuserdefine)) return ERR(gff);
	if(n[98] != NOTFOUND)
		if(gff.getExoLocString(n[98], top, description)) return ERR(gff);
	if(n[99] != NOTFOUND)
		if(gff.getExoLocString(n[99], top, firstname)) return ERR(gff);
	if(n[100] != NOTFOUND)
		if(gff.getExoLocString(n[100], top, lastname)) return ERR(gff);
	if(n[101] != NOTFOUND)
		if(gff.getExoLocString(n[101], top, mod_firstname)) return ERR(gff);
	if(n[102] != NOTFOUND)
		if(gff.getExoLocString(n[102], top, mod_lastname)) return ERR(gff);
	if(n[103] != NOTFOUND) if(getClasses(gff, n[103], top)) return errcode;
	if(n[104] != NOTFOUND) if(getItems(gff, n[104], top, false)) return errcode;
	if(n[105] != NOTFOUND) if(getItems(gff, n[105], top, true)) return errcode;
	if(n[106] != NOTFOUND) if(getSkills(gff, n[106], top)) return errcode;
	if(n[107] != NOTFOUND) if(getFeats(gff, n[107], top)) return errcode;
	if(n[108] != NOTFOUND) if(getSpecAbilities(gff, n[108], top)) return errcode;
	if(n[109] != NOTFOUND) if(getActions(gff, n[109], top)) return errcode;
	if(n[110] != NOTFOUND) if(getEffects(gff, n[110], top)) return errcode;
	if(n[111] != NOTFOUND) if(getExpressions(gff, n[111], top)) return errcode;
	if(n[112] != NOTFOUND) if(getPerceptions(gff, n[112], top)) return errcode;
	if(n[113] != NOTFOUND) if(getPreputations(gff, n[113], top)) return errcode;
	if(n[114] != NOTFOUND)
		if(variables.load(gff, n[114], top)) return ERR(variables);
	if(n[115] != NOTFOUND) if(getQuickBar(gff, n[115], top)) return errcode;
	if(n[116] != NOTFOUND) if(getMapDatas(gff, n[116], top)) return errcode;
	if(n[117] != NOTFOUND) if(getReputations(gff, n[117], top)) return errcode;
	if(n[118] != NOTFOUND)
		if(getCombatInfo(gff, top.values[n[118]])) return errcode;
	// TODO: CombatRoundData (what's in there anyway?) [high]
//	if(n[121] != NOTFOUND) // CombatRoundData
	if(n[120] != NOTFOUND)
		if(gff.getRaw(n[120], top, mapareasdata)) return ERR(gff);
	if(n[121] != NOTFOUND) ac = top.values[n[121]];
	if(n[122] != NOTFOUND) movementrate = top.values[n[122]];
	if(n[123] != NOTFOUND) encounterobject = top.values[n[123]];
	if(n[124] != NOTFOUND)
		if(gff.getResRef(n[124], top, portrait)) return ERR(gff);
	if(n[125] != NOTFOUND) bodyhead = top.values[n[125]];
	if(n[126] != NOTFOUND) bodyneck = top.values[n[126]];
	if(n[127] != NOTFOUND) bodytorso = top.values[n[127]];
	if(n[128] != NOTFOUND) bodybelt = top.values[n[128]];
	if(n[129] != NOTFOUND) bodypelvis = top.values[n[129]];
	if(n[130] != NOTFOUND) bodylshoul = top.values[n[130]];
	if(n[131] != NOTFOUND) bodylbicep = top.values[n[131]];
	if(n[132] != NOTFOUND) bodylfarm = top.values[n[132]];
	if(n[133] != NOTFOUND) bodylhand = top.values[n[133]];
	if(n[134] != NOTFOUND) bodyrshoul = top.values[n[134]];
	if(n[135] != NOTFOUND) bodyrbicep = top.values[n[135]];
	if(n[136] != NOTFOUND) bodyrfarm = top.values[n[136]];
	if(n[137] != NOTFOUND) bodyrhand = top.values[n[137]];
	if(n[138] != NOTFOUND) bodylthigh = top.values[n[138]];
	if(n[139] != NOTFOUND) bodylshin = top.values[n[139]];
	if(n[140] != NOTFOUND) bodylfoot = top.values[n[140]];
	if(n[141] != NOTFOUND) bodyrthigh = top.values[n[141]];
	if(n[142] != NOTFOUND) bodyrshin = top.values[n[142]];
	if(n[143] != NOTFOUND) bodyrfoot = top.values[n[143]];
	if(n[144] != NOTFOUND) colorskin = top.values[n[144]];
	if(n[145] != NOTFOUND) colorhair = top.values[n[145]];
	if(n[146] != NOTFOUND) colortattoo1 = top.values[n[146]];
	if(n[147] != NOTFOUND) colortattoo2 = top.values[n[147]];
	if(n[148] != NOTFOUND) if(getLvlStats(gff, n[148], top));
	checkRanges();
	return errcode = 0;
}

int Creature::getCombatInfo(GFFFile &gff, uint32 csn)
{
	std::string tlabels[] =
		{"NumAttacks", "SpellResistance", "ArcaneSpellFail", "ArmorCheckPen",
		 "UnarmedDamDice", "UnarmedDamDie", "OnHandCritRng", "OnHandCritMult",
		 "OffHandWeaponEq", "OffHandCritRng", "OffHandCritMult", "CreatureDice",
		 "CreatureDie", "reatureDice", "reatureDie", "eatureDice", "eatureDie",
		 "DamageDice", "DamageDie", "OnHandAttackMod", "OnHandDamageMod",
		 "OffHandAttackMod", "OffHandDamageMod", "LeftEquip", "RightEquip",
		 "LeftString", "RightString", "AttackList", "DamageList"};
	std::vector<uint32> n;
	GFFFile::Struct csct;

	if(gff.getStruct(csn, csct)) return ERR(gff);
	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				csct, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) numattacks = csct.values[n[0]];
	if(n[1] != NOTFOUND) spellresistance = csct.values[n[1]];
	if(n[2] != NOTFOUND) arcanespellfail = csct.values[n[2]];
	if(n[3] != NOTFOUND) armorcheckpen = csct.values[n[3]];
	if(n[4] != NOTFOUND) unarmeddamdice = csct.values[n[4]];
	if(n[5] != NOTFOUND) unarmeddamdie = csct.values[n[5]];
	if(n[6] != NOTFOUND) onhandcritrng = csct.values[n[6]];
	if(n[7] != NOTFOUND) onhandcritmult = csct.values[n[7]];
	if(n[8] != NOTFOUND) offhandweaponeq = csct.values[n[8]];
	if(n[9] != NOTFOUND) offhandcritrng = csct.values[n[9]];
	if(n[10] != NOTFOUND) offhandcritmult = csct.values[n[10]];
	if(n[11] != NOTFOUND) creaturedice = csct.values[n[11]];
	if(n[12] != NOTFOUND) creaturedie = csct.values[n[12]];
	if(n[13] != NOTFOUND) reaturedice = csct.values[n[13]];
	if(n[14] != NOTFOUND) reaturedie = csct.values[n[14]];
	if(n[15] != NOTFOUND) eaturedice = csct.values[n[15]];
	if(n[16] != NOTFOUND) eaturedie = csct.values[n[16]];
	if(n[17] != NOTFOUND) damagedice = csct.values[n[17]];
	if(n[18] != NOTFOUND) damagedie = csct.values[n[18]];
	if(n[19] != NOTFOUND) onhandattackmod = csct.values[n[19]];
	if(n[20] != NOTFOUND) onhanddamagemod = csct.values[n[20]];
	if(n[21] != NOTFOUND) offhandattackmod = csct.values[n[21]];
	if(n[22] != NOTFOUND) offhanddamagemod = csct.values[n[22]];
	if(n[23] != NOTFOUND) leftequip = csct.values[n[23]];
	if(n[24] != NOTFOUND) rightequip = csct.values[n[24]];
	if(n[25] != NOTFOUND)
		if(gff.getExoString(n[25], csct, leftstring)) return ERR(gff);
	if(n[26] != NOTFOUND)
		if(gff.getExoString(n[26], csct, rightstring)) return ERR(gff);
	if(n[27] != NOTFOUND)
		if(getAttackDamage(gff, n[27], csct, attack)) return errcode;
	if(n[28] != NOTFOUND)
		if(getAttackDamage(gff, n[28], csct, damage)) return errcode;
	return errcode = 0;
}

int Creature::getClasses(GFFFile &gff, uint32 cln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Domain1", "Domain2", "School", "ClassLevel", "Class", "SpellsPerDayList",
		 "KnownList0", "KnownList1", "KnownList2", "KnownList3", "KnownList4",
		 "KnownList5", "KnownList6", "KnownList7", "KnownList8", "KnownList9",
		 "MemorizedList0", "MemorizedList1", "MemorizedList2", "MemorizedList3",
		 "MemorizedList4", "MemorizedList5", "MemorizedList6", "MemorizedList7",
		 "MemorizedList8", "MemorizedList9"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(cln, top, scts)) return ERR(gff);
	classes.resize(MIN(scts.size(), 3));
	for(uint32 i=0;i<MIN(scts.size(), 3);i++)
	{
		classes[i].domain1 = classes[i].domain2 = classes[i].school = 0;
		classes[i].level = classes[i].cclass = 0;
		classes[i].kspells = classes[i].mspells = true;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) classes[i].domain1 = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) classes[i].domain2 = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) classes[i].school = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) classes[i].level = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) classes[i].cclass = scts[i].values[n[4]];
		if(n[5] != NOTFOUND)
			if(getSpellsLeft(gff, n[5], scts[i], classes[i])) return errcode;
		for(uint32 j=0;j<10;j++)
			if(n[j+6] != NOTFOUND)
				if(getSpellLevel(gff, n[j+6], scts[i], classes[i].knownspells[j]))
					return errcode;
		for(uint32 j=0;j<10;j++)
			if(n[j+16] != NOTFOUND)
				if(getSpellLevel(gff, n[j+16], scts[i], classes[i].memorizedspells[j]))
					return errcode;
	}
	return errcode = 0;
}

int Creature::getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top,
		bool equip)
{
	std::vector<GFFFile::Struct> scts;
	std::vector<class Item> *it=NULL;
	if(equip) it = &equippeditems;
	else it = &items;

	if(gff.getList(iln, top, scts)) return ERR(gff);
	it->resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		(*it)[i].objectstate = objectstate;
		if(creaturestate == NWN_CREATURE_BIC)
		{
			if(equip) (*it)[i].itemstate = NWN_ITEM_BICEINVENTORY;
			else (*it)[i].itemstate = NWN_ITEM_BICINVENTORY;
		}
		else
		{
			if(equip) (*it)[i].itemstate = NWN_ITEM_EINVENTORY;
			else (*it)[i].itemstate = NWN_ITEM_INVENTORY;
		}
		if(equip) (*it)[i].equipped = (NWNEquipped) scts[i].id;
		if((*it)[i].load(gff, scts[i])) return ERR((*it)[i]);
	}
	return errcode = 0;
}

int Creature::getSpellsLeft(GFFFile &gff, uint32 sln, GFFFile::Struct &top,
		Class &cls)
{
	std::vector<GFFFile::Struct> scts;

	memset(cls.spellsleft, 0, 10*sizeof(uint8));
	aderrstr = "NumSpellsLeft";
	if(gff.getList(sln, top, scts)) return ERR(gff);
	for(uint32 i=0;i<MIN(scts.size(), 10);i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) cls.spellsleft[i] = scts[i].values[n];
		else cls.spellsleft[i] = 0;
	}
	aderrstr.clear();
	return errcode = 0;
}

int Creature::getSpellLevel(GFFFile &gff, uint32 sln, GFFFile::Struct &top,
		std::vector<Spell> &spells)
{
	std::string tlabels[] =
		{"Ready", "SpellMetaMagic", "Spell", "SpellFlags"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(sln, top, scts)) return ERR(gff);
	spells.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		spells[i].ready = false;
		spells[i].metamagic = spells[i].spell = 0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) spells[i].ready = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) spells[i].metamagic = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) spells[i].spell = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) spells[i].flags = (uint16) scts[i].values[n[3]];
	}
	return errcode = 0;
}

int Creature::getSkills(GFFFile &gff, uint32 sln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Rank";
	if(gff.getList(sln, top, scts)) return ERR(gff);
	skills.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if(n == ERROR) return errcode = 20;
		if(n != NOTFOUND) skills[i] = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

int Creature::getFeats(GFFFile &gff, uint32 fln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	uint32 n;

	if(gff.getList(fln, top, scts)) return ERR(gff);
	feats.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		feats[i].uses = feats[i].feat = 0;
		aderrstr = "Uses";
		if((n = gff.getIdxByLabel(aderrstr, scts[i])) == ERROR)
			return errcode = 20;
		if(n != NOTFOUND) feats[i].uses = scts[i].values[n];
		aderrstr = "Feat";
		if((n = gff.getIdxByLabel(aderrstr, scts[i])) == ERROR)
			return errcode = 20;
		if(n != NOTFOUND) feats[i].feat = scts[i].values[n];
	}
	aderrstr.clear();
	return errcode = 0;
}

int Creature::getSpecAbilities(GFFFile &gff, uint32 saln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"SpellCasterLevel", "Spell", "SpellFlags"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(saln, top, scts)) return ERR(gff);
	specialabilities.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		specialabilities[i].spelllevel = specialabilities[i].spell = 0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) specialabilities[i].spelllevel = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) specialabilities[i].spell = scts[i].values[n[1]];
		if(n[2] != NOTFOUND)
			specialabilities[i].flags = (uint16) scts[i].values[n[2]];
	}
	return errcode = 0;
}

int Creature::getExpressions(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"ExpressionId", "ExpressionString"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(eln, top, scts)) return ERR(gff);
	expressions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		expressions[i].id = 0;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) expressions[i].id = scts[i].values[n[0]];
		if(n[1] != NOTFOUND)
			if(gff.getExoString(n[1], scts[i], expressions[i].expression))
				return ERR(gff);
	}
	return errcode = 0;
}

int Creature::getPerceptions(GFFFile &gff, uint32 pln, GFFFile::Struct &top)
{
	std::string tlabels[] = {"ObjectId", "PerceptionData"};

	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(pln, top, scts)) return ERR(gff);
	perceptions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&perceptions[i], 0, sizeof(Perception));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) perceptions[i].objectid = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) perceptions[i].data = scts[i].values[n[1]];
	}
	return errcode = 0;
}

int Creature::getPreputations(GFFFile &gff, uint32 prln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"Decays", "Amount", "Duration", "ObjectId", "Day", "Time"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(prln, top, scts)) return ERR(gff);
	preputations.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&preputations[i], 0, sizeof(PersonalReputation));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) preputations[i].decays = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) preputations[i].amount = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) preputations[i].duration = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) preputations[i].objectid = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) preputations[i].day = scts[i].values[n[4]];
		if(n[5] != NOTFOUND) preputations[i].time = scts[i].values[n[5]];
	}
	return errcode = 0;
}

int Creature::getReputations(GFFFile &gff, uint32 rln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	aderrstr = "Amount";
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

int Creature::getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(eln, top, scts)) return ERR(gff);
	effects.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(effects[i].load(gff, scts[i])) return ERR(effects[i]);
	return errcode = 0;
}

int Creature::getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;
	if(gff.getList(aln, top, scts)) return ERR(gff);
	actions.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
		if(actions[i].load(gff, scts[i])) return ERR(actions[i]);
	return errcode = 0;
}

int Creature::getMapDatas(GFFFile &gff, uint32 mdln, GFFFile::Struct &top)
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

int Creature::getAttackDamage(GFFFile &gff, uint32 adln, GFFFile::Struct &top,
		std::vector<AttackDamage> &list)
{
	std::string tlabels[] =
		{"Modifier", "ModifierType", "WeaponWield",
		 "VersusLawChaos", "VersusGoodEvil", "VersusRace"};

	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(adln, top, scts)) return ERR(gff);
	list.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		memset(&list[i], 0, sizeof(AttackDamage));
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) list[i].modifier = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) list[i].modifiertype = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) list[i].weaponwield = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) list[i].versuslawchaos = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) list[i].versusgoodevil = scts[i].values[n[4]];
		if(n[5] != NOTFOUND) list[i].versusrace = scts[i].values[n[5]];
	}
	return errcode = 0;
}

int Creature::getQuickBar(GFFFile &gff, uint32 qln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"QBObjectType", "QBCastPropIndex", "QBCastSubPropIdx", "QBContReposX",
		 "QBContReposY", "QBItemReposX", "QBItemReposY", "QBItemInvSlot",
		 "QBDomainLevel", "QBMetaType", "QBMultiClass", "QBINTParam1",
		 "QBCommandLabel", "QBCommandLine", "QBSecItemInvSlot", "QBSecItemReposX",
		 "QBSecItemReposY"};

	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(qln, top, scts)) return ERR(gff);
	for(uint32 i=0;i<MIN(scts.size(), 36);i++)
	{
		quickbar[i].type = NWN_QB_EMPTY;
		quickbar[i].castpropidx = quickbar[i].castsubpropidx = 0;
		quickbar[i].contreposx = quickbar[i].contreposy = 0;
		quickbar[i].itemreposx = quickbar[i].itemreposy = 0;
		quickbar[i].secitemreposx = quickbar[i].secitemreposy = 255;
		quickbar[i].domainlevel = quickbar[i].metatype = 0;
		quickbar[i].multiclass = quickbar[i].intparam1 = 0;
		quickbar[i].iteminvslot = 0;
		quickbar[i].seciteminvslot = 0xFFFFFFFF;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND)
			quickbar[i].type = (NWNQBObjectType) scts[i].values[n[0]];
		if(n[1] != NOTFOUND)
			quickbar[i].castpropidx = scts[i].values[n[1]];
		if(n[2] != NOTFOUND)
			quickbar[i].castsubpropidx = scts[i].values[n[2]];
		if(n[3] != NOTFOUND)
			quickbar[i].contreposx = scts[i].values[n[3]];
		if(n[4] != NOTFOUND)
			quickbar[i].contreposy = scts[i].values[n[4]];
		if(n[5] != NOTFOUND)
			quickbar[i].itemreposx = scts[i].values[n[5]];
		if(n[6] != NOTFOUND)
			quickbar[i].itemreposy = scts[i].values[n[6]];
		if(n[7] != NOTFOUND)
			quickbar[i].iteminvslot = scts[i].values[n[7]];
		if(n[8] != NOTFOUND)
			quickbar[i].domainlevel = scts[i].values[n[8]];
		if(n[9] != NOTFOUND)
			quickbar[i].metatype = scts[i].values[n[9]];
		if(n[10] != NOTFOUND)
			quickbar[i].multiclass = scts[i].values[n[10]];
		if(n[11] != NOTFOUND)
			quickbar[i].intparam1 = scts[i].values[n[11]];
		if(n[12] != NOTFOUND)
			if(gff.getExoString(n[12], scts[i], quickbar[i].commandlabel))
				return ERR(gff);
		if(n[13] != NOTFOUND)
			if(gff.getExoString(n[13], scts[i], quickbar[i].commandline))
				return ERR(gff);
		if(n[14] != NOTFOUND)
			quickbar[i].seciteminvslot = scts[i].values[n[14]];
		if(n[15] != NOTFOUND)
			quickbar[i].secitemreposx = scts[i].values[n[15]];
		if(n[16] != NOTFOUND)
			quickbar[i].secitemreposy = scts[i].values[n[16]];
	}
	return errcode = 0;
}

int Creature::getLvlStats(GFFFile &gff, uint32 lsln, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"EpicLevel", "LvlStatAbility", "LvlStatHitDie", "LvlStatClass",
		 "SkillPoints", "SkillList", "FeatList", "KnownList0", "KnownList1",
		 "KnownList2", "KnownList3", "KnownList4", "KnownList5", "KnownList6",
		 "KnownList7", "KnownList8", "KnownList9"};

	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts, sctss;

	if(gff.getList(lsln, top, scts)) return ERR(gff);
	lvlstats.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		lvlstats[i].epic = false;
		lvlstats[i].skillpoints = lvlstats[i].cclass = 0;
		lvlstats[i].hitdie = 1;
		lvlstats[i].ability = 255;
		if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
					scts[i], n, aderrstr))
			return ERR(gff);
		if(n[0] != NOTFOUND) lvlstats[i].epic = (bool) scts[i].values[n[0]];
		if(n[1] != NOTFOUND) lvlstats[i].ability = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) lvlstats[i].hitdie = scts[i].values[n[2]];
		if(n[3] != NOTFOUND) lvlstats[i].cclass = scts[i].values[n[3]];
		if(n[4] != NOTFOUND) lvlstats[i].skillpoints = scts[i].values[n[4]];
		if(n[5])
		{
			if(gff.getList(n[5], scts[i], sctss)) return ERR(gff);
			lvlstats[i].skills.resize(sctss.size());
			aderrstr = "Rank";
			for(uint32 j=0;j<sctss.size();j++)
			{
				uint32 m;
				if((m = gff.getIdxByLabel(aderrstr, sctss[j])) == ERROR)
					return ERR(gff);
				if(m != NOTFOUND) lvlstats[i].skills[j] = sctss[j].values[m];
			}
			aderrstr.clear();
			sctss.clear();
		}
		if(n[6])
		{
			if(gff.getList(n[6], scts[i], sctss)) return ERR(gff);
			lvlstats[i].feats.resize(sctss.size());
			aderrstr = "Feat";
			for(uint32 j=0;j<sctss.size();j++)
			{
				uint32 m;
				if((m = gff.getIdxByLabel(aderrstr, sctss[j])) == ERROR)
					return ERR(gff);
				if(m != NOTFOUND) lvlstats[i].feats[j] = sctss[j].values[m];
			}
			aderrstr.clear();
			sctss.clear();
		}
		for(uint32 j=0;j<10;j++)
			if(n[j+7] != NOTFOUND)
			{
				if(gff.getList(n[j+7], scts[i], sctss)) return ERR(gff);
				lvlstats[i].knownspells[j].resize(sctss.size());
				aderrstr = "Spell";
				for(uint32 k=0;k<sctss.size();k++)
				{
					uint32 m;
					if((m = gff.getIdxByLabel(aderrstr, sctss[k])) == ERROR)
						return ERR(gff);
					if(m != NOTFOUND) lvlstats[i].knownspells[j][k] = sctss[k].values[m];
				}
				aderrstr.clear();
				sctss.clear();
			}
	}
	return errcode = 0;
}

void Creature::checkRanges(void)
{
	if(goodevil > 100) goodevil = 100;
	if(lawfulchaotic > 100) lawfulchaotic = 100;
	if(classes.size() > 3) classes.resize(3);
	for(uint32 i=0;i<classes.size();i++)
	{
		if(!classes[i].kspells)
			for(uint32 j=0;j<10;j++) classes[i].knownspells[j].clear();
		if(!classes[i].mspells)
			for(uint32 j=0;j<10;j++) classes[i].memorizedspells[j].clear();
	}
	if(classes.empty())
	{
		classes.resize(1);
		classes[0].kspells = classes[0].mspells = false;
		classes[0].cclass = classes[0].domain1 = classes[0].domain2 = 0;
		classes[0].school = 0;
		classes[0].level = 1;
		memset(classes[0].spellsleft, 0, 10*sizeof(uint8));
	}
	if(creaturestate == NWN_CREATURE_BIC)
		pc = commandable = raiseable = interruptable = disarmable = true;
	if(tag.length() > 32) tag.erase(32);
}

int Creature::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	if(creaturestate == NWN_CREATURE_BIC)
	{
		objectstate = NWN_OBJECT_INSTANCE;
		gff.type = NWN_FILE_BIC;
	}
	else
	{
		objectstate = NWN_OBJECT_BLUEPRINT;
		gff.type = NWN_FILE_UTC;
	}
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Creature::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	if(creaturestate == NWN_CREATURE_BIC)
	{
		objectstate = NWN_OBJECT_INSTANCE;
		gff.type = NWN_FILE_BIC;
	}
	else
	{
		objectstate = NWN_OBJECT_BLUEPRINT;
		gff.type = NWN_FILE_UTC;
	}
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Creature::save(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "IsPC", pc)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Plot", plot)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "IsImmortal", immortal)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Interruptable", interruptable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Disarmable", disarmable))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Lootable", lootable)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "NoPermDeath", nopermdeath))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Gender", gender)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Race", race)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "GoodEvil", goodevil)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "LawfulChaotic", lawfulchaotic))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Str", strength)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Dex", dexterity)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Con", constitution)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Int", intelligence)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Wis", wisdom)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Cha", charisma)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "NaturalAC", naturalac)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Tail", tail)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "Wings", wings)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "BodyBag", bodybag)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "StartingPackage", startingpackage))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "MovementRate", movementrate))
		return ERR(gff);
	if(creatureappearance == NWN_CREATURE_SUBDIV)
	{
		if(gff.writeVar(top, NWN_VAR_INT8, "Appearance_Head", bodyhead))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_Neck", bodyneck))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_Torso", bodytorso))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_Belt", bodybelt))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_Pelvis", bodypelvis))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LShoul", bodylshoul))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LBicep", bodylbicep))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LFArm", bodylfarm))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LHand", bodylhand))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_RShoul", bodyrshoul))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_RBicep", bodyrbicep))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_RFArm", bodyrfarm))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_RHand", bodyrhand))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LThigh", bodylthigh))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LShin", bodylshin))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_LFoot", bodylfoot))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_RThigh", bodyrthigh))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BodyPart_RShin", bodyrshin))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "ArmorPart_RFoot", bodyrfoot))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Color_Skin", colorskin))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Color_Hair", colorhair))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Color_Tattoo1", colortattoo1))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Color_Tattoo2", colortattoo2))
			return ERR(gff);
	}
	if(gff.writeVar(top, NWN_VAR_INT8, "PerceptionRange", perceptionrange))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "MaxHitPoints", maxhitpoints))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "HitPoints", hitpoints)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "CurrentHitPoints", currenthitpoints))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "ArmorClass", ac)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "fortbonus", fortitudebonus))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "refbonus", reflexbonus))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT16, "willbonus", willbonus)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "Appearance_Type", appearance))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "SoundSetFile", soundset))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "FactionID", factionid))
		return ERR(gff);
	if(portrait.empty())
		if(gff.writeVar(top, NWN_VAR_UINT16, "PortraitId", portraitid))
			return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "CRAdjust", cradjust)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "WalkRate", walkrate)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT32, "Phenotype", phenotype)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "DecayTime", decaytime))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "EncounterObject", encounterobject))
		return ERR(gff);
	if(creaturestate == NWN_CREATURE_NORMAL)
		if(gff.writeFloat(top, "ChallengeRating", challengerating)) return ERR(gff);
	if(gff.writeExoString(top, "Subrace", subrace)) return ERR(gff);
	if(gff.writeExoString(top, "Deity", deity)) return ERR(gff);
	if(gff.writeExoString(top, "Tag", tag)) return ERR(gff);
	if(creaturestate == NWN_CREATURE_NORMAL)
		if(gff.writeResRef(top, "Conversation", conversation)) return ERR(gff);
	if(!portrait.empty())
		if(gff.writeResRef(top, "Portrait", portrait)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptAttacked", scriptattacked)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptDamaged", scriptdamaged)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptDeath", scriptdeath)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptDialogue", scriptdialogue)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptDisturbed", scriptdisturbed)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptEndRound", scriptendround)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptHeartbeat", scriptheartbeat)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptOnBlocked", scriptonblocked)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptOnNotice", scriptonnotice)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptRested", scriptrested)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptSpawn", scriptspawn)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptSpellAt", scriptspellat)) return ERR(gff);
	if(gff.writeResRef(top, "ScriptUserDefine", scriptuserdefine))
		return ERR(gff);
	if(creaturestate == NWN_CREATURE_NORMAL)
		if(gff.writeResRef(top, "TemplateResRef", templateresref)) return ERR(gff);
	if(gff.writeExoLocString(top, "Description", description)) return ERR(gff);
	if(gff.writeExoLocString(top, "FirstName", firstname)) return ERR(gff);
	if(gff.writeExoLocString(top, "LastName", lastname)) return ERR(gff);

	if(objectstate == NWN_OBJECT_BLUEPRINT)
	{
		if(creaturestate == NWN_CREATURE_NORMAL)
			if(gff.writeExoString(top, "Comment", comment)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "PaletteID", paletteid))
			return ERR(gff);
	}
	else
	{
		if(gff.writeFloat(top, "XPosition", x)) return ERR(gff);
		if(gff.writeFloat(top, "YPosition", y)) return ERR(gff);
		if(gff.writeFloat(top, "ZPosition", z)) return ERR(gff);
		if(gff.writeFloat(top, "XOrientation", dirx)) return ERR(gff);
		if(gff.writeFloat(top, "YOrientation", diry)) return ERR(gff);
	}
	if((objectstate == NWN_OBJECT_SAVE)
			|| (creaturestate == NWN_CREATURE_BIC))
	{
		if(gff.writeVar(top, NWN_VAR_INT8, "IsCommandable", commandable))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "IsDM", dm)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "IsDestroyable", destroyable))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "DeadSelectable", deadselectable))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "IsRaiseable", raiseable))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "StealthMode", stealthmode))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "DetectMode", detectmode))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT8, "FortSaveThrow", fortitudesavethrow))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT8, "RefSaveThrow", reflexsavethrow))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT8, "WillSaveThrow", willsavethrow))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "AmbientAnimState", ambientanimstate))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "BaseAttackBonus", baseattackbonus))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "MClassLevUpIn", mclasslevupin))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "OverrideBAB", overridebab))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "PM_IsPolymorphed", pm_ispolymorphed))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT8, "Listening", listening))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT16, "PregameCurrent", pregamecurrent))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT16, "SkillPoints", skillpoints))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT32, "Age", age)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT32, "CreatureSize", creaturesize))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT32, "FamiliarType", familiartype))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "AnimationDay", animationday))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "AnimationTime", animationtime))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "AreaId", areaid)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "BodyBagId", bodybagid))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "Experience", experience))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "Gold", gold)) return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "MasterID", masterid))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "ObjectId", objectid))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_UINT32, "SitObject", sitobject))
			return ERR(gff);
		if(gff.writeExoString(top, "FamiliarName", familiarname))
			return ERR(gff);
		if(gff.writeList(list, top, "PersonalRepList")) return ERR(gff);
		for(uint32 i=0;i<preputations.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 0xABED)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT8, "Decays", preputations[i].decays))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT32, "Amount", preputations[i].amount))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT32, "Duration", preputations[i].duration))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "ObjectId", preputations[i].objectid))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "Day", preputations[i].day))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "Time", preputations[i].time))
				return ERR(gff);
		}
		if(gff.writeList(list, top, "PerceptionList")) return ERR(gff);
		for(uint32 i=0;i<perceptions.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 0)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT8, "PerceptionData", perceptions[i].data))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "ObjectId", perceptions[i].objectid))
				return ERR(gff);
		}
		if(gff.writeList(list, top, "ExpressionList")) return ERR(gff);
		for(uint32 i=0;i<expressions.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 5)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "ExpressionId", expressions[i].id))
				return ERR(gff);
			if(gff.writeExoString(sct, "ExpressionString", expressions[i].expression))
				return ERR(gff);
		}
		if(gff.writeStruct(sct, "CombatInfo", top)) return ERR(gff);
		if(gff.writeSetId(sct, 51882)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "NumAttacks", numattacks))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "OnHandAttackMod", onhandattackmod))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "OnHandDamageMod", onhanddamagemod))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "OffHandAttackMod", offhandattackmod))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "OffHandDamageMod", offhanddamagemod))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "SpellResistance", spellresistance))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "ArcaneSpellFail", arcanespellfail))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "ArmorCheckPen", armorcheckpen))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "UnarmedDamDice", unarmeddamdice))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "UnarmedDamDie", unarmeddamdie))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "OnHandCritRng", onhandcritrng))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "OnHandCritMult", onhandcritmult))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "OffHandWeaponEq", offhandweaponeq))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "OffHandCritRng", offhandcritrng))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "OffHandCritMult", offhandcritmult))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "LeftEquip", leftequip))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "RightEquip", rightequip))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "CreatureDice", creaturedice))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "CreatureDie", creaturedie))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "reatureDice", reaturedice))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "reatureDie", reaturedie))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "eatureDice", eaturedice))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "eatureDie", eaturedie))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "DamageDice", damagedice))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "DamageDie", damagedie))
			return ERR(gff);
		if(gff.writeExoString(sct, "LeftString", leftstring)) return ERR(gff);
		if(gff.writeExoString(sct, "RightString", rightstring)) return ERR(gff);
		if(gff.writeList(list, sct, "AttackList")) return ERR(gff);
		if(writeAttackDamage(gff, list, attack, 43758)) return errcode;
		if(gff.writeList(list, sct, "DamageList")) return ERR(gff);
		if(writeAttackDamage(gff, list, damage, 56814)) return errcode;
// Struct CombatRoundData
	}
	if(objectstate == NWN_OBJECT_SAVE)
	{
		if(writeActions(gff, top)) return errcode;
		if(writeEffects(gff, top)) return errcode;
	}
	if(creaturestate == NWN_CREATURE_BIC)
		if(writeQuickBar(gff, top)) return errcode;
	if((objectstate == NWN_OBJECT_SAVE) && (creaturestate == NWN_CREATURE_BIC))
	{
		if(gff.writeVar(top, NWN_VAR_INT8, "Mod_IsPrimaryPlr", mod_isprimaryplr))
			return ERR(gff);
		if(gff.writeVar(top, NWN_VAR_INT32, "Mod_MapNumAreas", mod_mapnumareas))
			return ERR(gff);
		if(gff.writeExoString(top, "Mod_CommntyName", mod_commntyname))
			return ERR(gff);
		if(gff.writeExoLocString(top, "Mod_FirstName", mod_firstname))
			return ERR(gff);
		if(gff.writeExoLocString(top, "Mod_LastName", mod_lastname))
			return ERR(gff);
		if(gff.writeData(top, "Mod_MapAreasData", mapareasdata, false))
			return ERR(gff);
		if(gff.writeList(list, top, "Mod_MapDataList")) return ERR(gff);
		for(uint32 i=0;i<mapdatas.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 0)) return ERR(gff);
			if(gff.writeData(sct, "Mod_MapData", mapdatas[i], false)) return ERR(gff);
		}
		if(gff.writeList(list, top, "ReputationList")) return ERR(gff);
		for(uint32 i=0;i<reputations.size();i++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 47837)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT32, "Amount", reputations[i]))
				return ERR(gff);
		}
	}

	if(gff.writeList(list, top, "ClassList")) return ERR(gff);
	for(uint32 i=0;i<MIN(classes.size(), 3);i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 2)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT32, "Class", classes[i].cclass))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT16, "ClassLevel", classes[i].level))
			return ERR(gff);
		if((objectstate == NWN_OBJECT_SAVE)
			|| (objectstate == NWN_OBJECT_INSTANCE)
			|| (creaturestate == NWN_CREATURE_BIC))
		{
			if(gff.writeVar(sct, NWN_VAR_UINT8, "Domain1", classes[i].domain1))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT8, "Domain2", classes[i].domain2))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT8, "School", classes[i].school))
				return ERR(gff);
			if(writeSpellsLeft(gff, sct, classes[i].spellsleft)) return errcode;
			if(classes[i].kspells)
				if(writeKSpells(gff, sct, classes[i].knownspells)) return errcode;
			if(classes[i].mspells)
				if(writeMSpells(gff, sct, classes[i].memorizedspells)) return errcode;
		}
	}
	if(gff.writeList(list, top, "FeatList")) return ERR(gff);
	for(uint32 i=0;i<feats.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 1)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Uses", feats[i].uses)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "Feat", feats[i].feat))
			return ERR(gff);
	}
	if(gff.writeList(list, top, "SkillList")) return ERR(gff);
	for(uint32 i=0;i<skills.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "Rank", skills[i])) return ERR(gff);
	}
	if(gff.writeList(list, top, "SpecAbilityList")) return ERR(gff);
	for(uint32 i=0;i<specialabilities.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 4)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "Spell", specialabilities[i].spell))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "SpellCasterLevel",
					specialabilities[i].spelllevel))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "SpellFlags",
					(uint16) specialabilities[i].flags))
			return ERR(gff);
	}
	if(!lvlstats.empty()) if(writeLvlStats(gff, top)) return ERR(gff);
	if(gff.writeList(list, top, "ItemList")) return ERR(gff);
	for(uint32 i=0;i<items.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(creaturestate == NWN_CREATURE_BIC)
			items[i].objectstate = NWN_OBJECT_SAVE;
		else
			items[i].objectstate = objectstate;
		items[i].itemstate = NWN_ITEM_INVENTORY;
		if(items[i].save(gff, sct)) return ERR(items[i]);
	}
	if(gff.writeList(list, top, "Equip_ItemList")) return ERR(gff);
	for(uint32 i=0;i<equippeditems.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, (uint32) equippeditems[i].equipped)) return ERR(gff);
		equippeditems[i].objectstate = objectstate;
		equippeditems[i].itemstate = NWN_ITEM_EINVENTORY;
		if(equippeditems[i].save(gff, sct)) return ERR(equippeditems[i]);
	}
	if(!variables.empty())
	{
		if(gff.writeList(list, top, "VarTable")) return ERR(gff);
		if(variables.save(gff, list)) return ERR(variables);
	}
	return errcode = 0;
}

int Creature::writeSpellsLeft(GFFFile &gff, uint32 top, uint8 sl[10])
{
	uint32 list, sct;

	if(gff.writeList(list, top, "SpellsPerDayList")) return ERR(gff);
	for(uint32 i=0;i<10;i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 17767)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "NumSpellsLeft", sl[i]))
			return ERR(gff);
	}
	return errcode = 0;
}

int Creature::writeKSpells(GFFFile &gff, uint32 top, std::vector<Spell> spells[10])
{
	uint32 list, sct;
	std::string str[] =
		{"KnownList0", "KnownList1", "KnownList2", "KnownList3", "KnownList4",
		 "KnownList5", "KnownList6", "KnownList7", "KnownList8", "KnownList9"};

	for(uint32 i=0;i<10;i++)
	{
		if(gff.writeList(list, top, str[i])) return ERR(gff);
		for(uint32 j=0;j<spells[i].size();j++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 3)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT16, "Spell", spells[i][j].spell))
				return ERR(gff);
			if(((objectstate == NWN_OBJECT_BLUEPRINT)
					|| (objectstate == NWN_OBJECT_INSTANCE))
					&& (creaturestate == NWN_CREATURE_NORMAL))
			{
				if(gff.writeVar(sct, NWN_VAR_INT8, "SpellFlags",
							(uint16) spells[i][j].flags))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "SpellMetaMagic",
							spells[i][j].metamagic))
					return ERR(gff);
			}
		}
	}
	return errcode = 0;
}

int Creature::writeMSpells(GFFFile &gff, uint32 top, std::vector<Spell> spells[10])
{
	uint32 list, sct;
	std::string str[] =
		{"MemorizedList0", "MemorizedList1", "MemorizedList2", "MemorizedList3",
		 "MemorizedList4", "MemorizedList5", "MemorizedList6", "MemorizedList7",
		 "MemorizedList8", "MemorizedList9"};

	for(uint32 i=0;i<10;i++)
	{
		if(gff.writeList(list, top, str[i])) return ERR(gff);
		for(uint32 j=0;j<spells[i].size();j++)
		{
			if(gff.writeListStruct(sct, list)) return ERR(gff);
			if(gff.writeSetId(sct, 3)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT16, "Spell", spells[i][j].spell))
				return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT8, "SpellMetaMagic",
						spells[i][j].metamagic))
				return ERR(gff);
			if(((objectstate == NWN_OBJECT_BLUEPRINT)
					|| (objectstate == NWN_OBJECT_INSTANCE))
					&& (creaturestate == NWN_CREATURE_NORMAL))
			{
				if(gff.writeVar(sct, NWN_VAR_INT8, "SpellFlags",
							(uint16) spells[i][j].flags))
					return ERR(gff);
			}
			else
				if(gff.writeVar(sct, NWN_VAR_INT8, "Ready", spells[i][j].ready))
					return ERR(gff);
		}
	}
	return errcode = 0;
}

int Creature::writeActions(GFFFile &gff, uint32 top)
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

int Creature::writeEffects(GFFFile &gff, uint32 top)
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

int Creature::writeQuickBar(GFFFile &gff, uint32 top)
{
	uint32 list, sct;

	if(gff.writeList(list, top, "QBList")) return ERR(gff);
	for(uint32 i=0;i<36;i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "QBObjectType",
					(uint8) quickbar[i].type))
			return ERR(gff);
		switch(quickbar[i].type)
		{
			case NWN_QB_ITEM:
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBCastPropIndex",
							quickbar[i].castpropidx))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBCastSubPropIdx",
							quickbar[i].castsubpropidx))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBContReposX",
							quickbar[i].contreposx))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBContReposY",
							quickbar[i].contreposy))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBItemReposX",
							quickbar[i].itemreposx))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBItemReposY",
							quickbar[i].itemreposy))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_UINT32, "QBItemInvSlot",
							quickbar[i].iteminvslot))
					return ERR(gff);
				if(quickbar[i].secitemreposx != 255)
					if(gff.writeVar(sct, NWN_VAR_INT8, "QBSecItemReposX",
								quickbar[i].secitemreposx))
						return ERR(gff);
				if(quickbar[i].secitemreposy != 255)
					if(gff.writeVar(sct, NWN_VAR_INT8, "QBSecItemReposY",
								quickbar[i].secitemreposy))
						return ERR(gff);
				if(quickbar[i].seciteminvslot != 0xFFFFFFFF)
					if(gff.writeVar(sct, NWN_VAR_UINT32, "QBSecItemInvSlot",
								quickbar[i].seciteminvslot))
						return ERR(gff);
				break;
			case NWN_QB_MACRO:
				if(gff.writeExoString(sct, "QBCommandLabel", quickbar[i].commandlabel))
					return ERR(gff);
				if(gff.writeExoString(sct, "QBCommandLine", quickbar[i].commandline))
					return ERR(gff);
				break;
			case NWN_QB_SPELL:
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBDomainLevel",
							quickbar[i].domainlevel))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBMetaType",
							quickbar[i].metatype))
					return ERR(gff);
				if(gff.writeVar(sct, NWN_VAR_INT8, "QBMultiClass",
							quickbar[i].multiclass))
					return ERR(gff);
			case NWN_QB_SKILL:
			case NWN_QB_FEAT:
			case NWN_QB_MODE_TOGGLE:
			case NWN_QB_EMOTE:
			case NWN_QB_QUICKCHAT:
			case NWN_QB_SPELLLIKE_ABILITY:
				if(gff.writeVar(sct, NWN_VAR_INT32, "QBINTParam1",
							quickbar[i].intparam1))
					return ERR(gff);
				break;
			default:
				break;
		}
	}
	return errcode = 0;
}

int Creature::writeAttackDamage(GFFFile &gff, uint32 list,
		std::vector<AttackDamage> &ad, uint32 id)
{
	uint32 sct;

	for(uint32 i=0;i<ad.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, id)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT8, "Modifier", ad[i].modifier))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "ModifierType", ad[i].modifiertype))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "WeaponWield", ad[i].weaponwield))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "VersusLawChaos", ad[i].versuslawchaos))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "VersusGoodEvil", ad[i].versusgoodevil))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "VersusRace", ad[i].versusrace))
			return ERR(gff);
	}
	return errcode = 0;
}

int Creature::writeLvlStats(GFFFile &gff, uint32 top)
{
	uint32 list, sct, lists, scts;
	std::string str[] =
		{"KnownList0", "KnownList1", "KnownList2", "KnownList3", "KnownList4",
		 "KnownList5", "KnownList6", "KnownList7", "KnownList8", "KnownList9"};

	if(gff.writeList(list, top, "LvlStatList")) return ERR(gff);
	for(uint32 i=0;i<lvlstats.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, 0)) return ERR(gff);
		if(lvlstats[i].ability != 255)
			if(gff.writeVar(sct, NWN_VAR_INT8, "LvlStatAbility", lvlstats[i].ability))
				return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "LvlStatHitDie", lvlstats[i].hitdie))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "LvlStatClass", lvlstats[i].cclass))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT16, "SkillPoints", lvlstats[i].skillpoints))
			return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "EpicLevel", lvlstats[i].epic))
			return ERR(gff);
		if(gff.writeList(lists, sct, "SkillList")) return ERR(gff);
		for(uint32 j=0;j<lvlstats[i].skills.size();j++)
		{
			if(gff.writeListStruct(scts, lists)) return ERR(gff);
			if(gff.writeSetId(scts, 0)) return ERR(gff);
			if(gff.writeVar(scts, NWN_VAR_INT8, "Rank", lvlstats[i].skills[j]))
				return ERR(gff);
		}
		if(gff.writeList(lists, sct, "FeatList")) return ERR(gff);
		for(uint32 j=0;j<lvlstats[i].feats.size();j++)
		{
			if(gff.writeListStruct(scts, lists)) return ERR(gff);
			if(gff.writeSetId(scts, 0)) return ERR(gff);
			if(gff.writeVar(scts, NWN_VAR_UINT16, "Feat", lvlstats[i].feats[j]))
				return ERR(gff);
		}
		for(uint32 j=0;j<10;j++)
			if(!lvlstats[i].knownspells[j].empty())
			{
				if(gff.writeList(lists, sct, str[j])) return ERR(gff);
				for(uint32 k=0;k<lvlstats[i].knownspells[j].size();k++)
				{
					if(gff.writeListStruct(scts, lists)) return ERR(gff);
					if(gff.writeSetId(scts, 0)) return ERR(gff);
					if(gff.writeVar(scts, NWN_VAR_UINT16, "Spell",
								lvlstats[i].knownspells[j][k]))
						return ERR(gff);
				}
			}
	}
	return errcode = 0;
}

// vim:tabstop=2
