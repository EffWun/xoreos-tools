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

Event::Event()
{
	type = NWN_EVENT_UNDEFINED;
	objectid = INVALID_OBJECT_ID;
	callerid = day = time = remove = playanimation = broadcast = 0;
	id = (uint64) -1;
}

int Event::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"CallerId", "ObjectId", "EventId", "Day", "Time", "EventData", "Id"};
	std::vector<uint32> n;

	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) callerid = top.values[n[0]];
	if(n[1] != NOTFOUND) objectid = top.values[n[1]];
	if(n[2] != NOTFOUND) type = (NWNEventType) top.values[n[2]];
	if(n[3] != NOTFOUND) day = top.values[n[3]];
	if(n[4] != NOTFOUND) time = top.values[n[4]];
	if(n[5] != NOTFOUND)
	{
		switch(type)
		{
			case NWN_EVENT_TIMED_EVENT:
				if(getScriptSituation(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_REMOVE_FROM_AREA:
				if(getRemove(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_APPLY_EFFECT:
				if(getEffect(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_SPELL_IMPACT:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_PLAY_ANIMATION:
				if(getPlay(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_SIGNAL_EVENT:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_REMOVE_EFFECT:
				if(getEffect(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_ON_MELEE_ATTACKED:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_SPAWN_BODY_BAG:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_FORCED_ACTION:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_ITEM_ON_HIT_SPELL_IMPACT:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_BROADCAST_AOO:
				if(getBroadCast(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_BROADCAST_SAFE_PROJECTILE:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_FEEDBACK_MESSAGE:
				scriptevent.registerLabels("Type", "IntList", "FloatList",
						"StringList", "ObjectIDList", "IntegerValue", "FloatValue",
						"StringValue", "ObjectValue");
				scriptevent.registerSid(47789);
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			case NWN_EVENT_SUMMON_CREATURE:
				if(getScriptEvent(gff, top.values[n[5]])) return errcode;
				break;
			default:
				break;
		}
	}
	if(n[6] != NOTFOUND) id = top.values[n[6]];
	return errcode = 0;
}

int Event::getRemove(GFFFile &gff, uint32 rsn)
{
	GFFFile::Struct esct;
	uint32 n;

  aderrstr = "Value";
	if(gff.getStruct(rsn, esct)) return ERR(gff);
	if((n = gff.getIdxByLabel(aderrstr, esct)) == ERROR) return errcode = 20;
	if(n != NOTFOUND) remove = esct.values[n];
	aderrstr.clear();
	return errcode = 0;
}

int Event::getPlay(GFFFile &gff, uint32 psn)
{
	GFFFile::Struct psct;
	uint32 n;

  aderrstr = "Value";
	if(gff.getStruct(psn, psct)) return ERR(gff);
	if((n = gff.getIdxByLabel(aderrstr, psct)) == ERROR) return errcode = 20;
	if(n != NOTFOUND) playanimation = psct.values[n];
	aderrstr.clear();
	return errcode = 0;
}

int Event::getBroadCast(GFFFile &gff, uint32 bsn)
{
	GFFFile::Struct bsct;
	uint32 n;

  aderrstr = "Value";
	if(gff.getStruct(bsn, bsct)) return ERR(gff);
	if((n = gff.getIdxByLabel(aderrstr, bsct)) == ERROR) return errcode = 20;
	if(n != NOTFOUND) broadcast = bsct.values[n];
	aderrstr.clear();
	return errcode = 0;
}

int Event::getEffect(GFFFile &gff, uint32 esn)
{
	GFFFile::Struct esct;

	if(gff.getStruct(esn, esct)) return ERR(gff);
	if(effect.load(gff, esct)) return ERR(effect);
	return errcode = 0;
}

int Event::getScriptSituation(GFFFile &gff, uint32 sssn)
{
	GFFFile::Struct sssct;

	if(gff.getStruct(sssn, sssct)) return ERR(gff);
	if(scriptsituation.load(gff, sssct)) return ERR(scriptsituation);
	return errcode = 0;
}

int Event::getScriptEvent(GFFFile &gff, uint32 sesn)
{
	GFFFile::Struct sesct;

	if(gff.getStruct(sesn, sesct)) return ERR(gff);
	if(scriptevent.load(gff, sesct)) return ERR(scriptevent);
	return errcode = 0;
}

int Event::save(GFFFile &gff, uint32 top)
{
	uint32 sct;
	if(gff.writeVar(top, NWN_VAR_UINT32, "CallerId", callerid)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "ObjectId", objectid)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "EventId", (uint32) type))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Day", day)) return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "Time", time)) return ERR(gff);
	if(id != ((uint64) -1))
		if(gff.writeVar(top, NWN_VAR_UINT64, "Id", id)) return ERR(gff);
	switch(type)
	{
		case NWN_EVENT_TIMED_EVENT:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x7777)) return ERR(gff);
			if(scriptsituation.save(gff, sct)) return ERR(scriptsituation);
			break;
		case NWN_EVENT_REMOVE_FROM_AREA:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x9999)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT8, "Value", remove)) return ERR(gff);
			break;
		case NWN_EVENT_APPLY_EFFECT:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x1111)) return ERR(gff);
			if(effect.save(gff, sct)) return ERR(effect);
			break;
		case NWN_EVENT_SPELL_IMPACT:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x6666)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_PLAY_ANIMATION:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x3333)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_INT32, "Value", playanimation))
				return ERR(gff);
			break;
		case NWN_EVENT_SIGNAL_EVENT:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x4444)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_REMOVE_EFFECT:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x1111)) return ERR(gff);
			if(effect.save(gff, sct)) return ERR(effect);
			break;
		case NWN_EVENT_ON_MELEE_ATTACKED:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x2222)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_SPAWN_BODY_BAG:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x5555)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_FORCED_ACTION:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x8888)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_ITEM_ON_HIT_SPELL_IMPACT:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x6666)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_BROADCAST_AOO:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0xAAAA)) return ERR(gff);
			if(gff.writeVar(sct, NWN_VAR_UINT32, "Value", broadcast)) return ERR(gff);
			break;
		case NWN_EVENT_BROADCAST_SAFE_PROJECTILE:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0x2222)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_FEEDBACK_MESSAGE:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0xCCCC)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		case NWN_EVENT_SUMMON_CREATURE:
			if(gff.writeStruct(sct, "EventData", top)) return ERR(gff);
			if(gff.writeSetId(sct, 0xDDDD)) return ERR(gff);
			if(scriptevent.save(gff, sct)) return ERR(scriptevent);
			break;
		default:
			break;
	}
	return errcode = 0;
}

// vim:tabstop=2
