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

/** @file aurora/erffile.h
 *  Handling BioWare's ERFs (encapsulated resource file).
 */

#ifndef AURORA_ERFFILE_H
#define AURORA_ERFFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/archive.h"
#include "aurora/aurorafile.h"
#include "aurora/locstring.h"

namespace Common {
	class SeekableReadStream;
	class File;
}

namespace Aurora {

/** Class to hold resource data of an ERF file. */
class ERFFile : public Archive, public AuroraBase {
public:
	ERFFile(const Common::UString &fileName, bool noResources = false);
	~ERFFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

	/** Return the year the ERF was built. */
	uint32 getBuildYear() const;

	/** Return the day of year the ERF was built. */
	uint32 getBuildDay() const;

	/** Return the description. */
	const LocString &getDescription() const;

private:
	/** The header of an ERF file. */
	struct ERFHeader {
		uint32 langCount;      ///< Number of language strings in the description.
		uint32 descriptionID;  ///< ID of the description.

		uint32 offDescription; ///< Offset to the description.
		uint32 offKeyList;     ///< Offset to the key list.
		uint32 offResList;     ///< Offset to the resource list.
	};

	/** Internal resource information. */
	struct IResource {
		uint32 offset; ///< The offset of the resource within the BIF.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	bool _noResources;

	/** The ERF's description. */
	LocString _description;

	/** The year the ERF archive was built. */
	uint32 _buildYear;
	/** The day of the year the ERF archive was built. */
	uint32 _buildDay;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	/** The name of the ERF file. */
	Common::UString _fileName;

	void open(Common::File &file) const;

	void load();

	void readERFHeader  (Common::SeekableReadStream &erf,       ERFHeader &header);
	void readDescription(Common::SeekableReadStream &erf, const ERFHeader &header);
	void readResources  (Common::SeekableReadStream &erf, const ERFHeader &header);

	// V1.0
	void readV1ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	void readV1KeyList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V2.0
	void readV2ResList(Common::SeekableReadStream &erf, const ERFHeader &header);

	const IResource &getIResource(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_ERFFILE_H
