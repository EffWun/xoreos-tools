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

Door::Door()
{
	generictype = linkedtoflags = loadscreen = 0;
}

int Door::load(std::string fname)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Door::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Door::load(GFFFile &gff, GFFFile::Struct &top)
{
	std::string tlabels[] =
		{"GenericType", "LinkedToFlags", "LoadScreenID", "LinkedTo",
		 "OnClick", "OnFailToOpen"};
	std::vector<uint32> n;

	if(Situated::load(gff, top)) return errcode;
	if(gff.getIdxByLabel(tlabels, sizeof(tlabels)/sizeof(std::string),
				top, n, aderrstr))
		return ERR(gff);
	if(n[0] != NOTFOUND) generictype = top.values[n[0]];
	if(n[1] != NOTFOUND) linkedtoflags = top.values[n[1]];
	if(n[2] != NOTFOUND) loadscreen = top.values[n[2]];
	if(n[3] != NOTFOUND)
		if(gff.getExoString(n[3], top, linkedto)) return ERR(gff);
	if(n[4] != NOTFOUND)
		if(gff.getResRef(n[4], top, onclick)) return ERR(gff);
	if(n[5] != NOTFOUND)
		if(gff.getResRef(n[5], top, onfailtoopen)) return ERR(gff);
	checkRanges();

	return errcode = 0;
}

void Door::checkRanges(void)
{
	Situated::checkRanges();
	if(animationstate > 2) animationstate = 2;
	if(linkedtoflags > 2) linkedtoflags = 2;
}

int Door::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_UTD;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Door::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_UTD;
	objectstate = NWN_OBJECT_BLUEPRINT;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Door::save(GFFFile &gff, uint32 top)
{
	if(Situated::save(gff, top)) return errcode;
	checkRanges();
	if(gff.writeVar(top, NWN_VAR_INT8, "GenericType", generictype))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_INT8, "LinkedToFlags", linkedtoflags))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT16, "LoadScreenID", loadscreen))
		return ERR(gff);
	if(gff.writeExoString(top, "LinkedTo", linkedto)) return ERR(gff);
	if(gff.writeResRef(top, "OnClick", onclick)) return ERR(gff);
	if(gff.writeResRef(top, "OnFailToOpen", onfailtoopen)) return ERR(gff);
	if(objectstate == NWN_OBJECT_SAVE)
		if(gff.writeVar(top, NWN_VAR_UINT8, "SecretDoorDC", 0)) return ERR(gff);
	return errcode = 0;
}

// vim:tabstop=2
