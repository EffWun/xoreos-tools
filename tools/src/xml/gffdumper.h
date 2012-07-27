/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file xml/gffdumper.h
 *  Dump GFFs into XML files.
 */

#ifndef XML_GFFDUMPER_H
#define XML_GFFDUMPER_H

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
	class WriteStream;
}

namespace Aurora {
	class LocString;
}

namespace XML {

class XMLWriter;

class GFFDumper {
public:
	GFFDumper();
	~GFFDumper();

	static void dump(Common::WriteStream &output, Common::SeekableReadStream &input);

private:
	static Aurora::FileType identify(Common::SeekableReadStream &input);

	static uint32 getID(Aurora::FileType type);
	static Common::UString getTypeName(Aurora::FileType type);

	static void dumpStruct(XMLWriter &xml, const Aurora::GFFStruct &strct);
	static void dumpList(XMLWriter &xml, const Aurora::GFFList &list);

	static void dumpField(XMLWriter &xml, const Aurora::GFFStruct &strct, const Common::UString &field);

	static void dumpLocString(XMLWriter &xml, const Aurora::LocString &locString);
};

} // End of namespace XML

#endif // XML_GFFDUMPER_H
