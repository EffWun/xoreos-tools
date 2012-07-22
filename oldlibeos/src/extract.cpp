/* eos 0.0.1
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
#include "extract.h"
#include "common.h"

bool extracterf(char *fname)
{
	ERFFile erffile;
	std::string filename;
	const char *ext;

	if(!init(erffile, fname)) return false;
	for(uint32 i=0;i<erffile.entrycount;i++)
	{
		printf("- %5u: %-16s (Type: %u, Offset: %u, Size: %u)\n",
				erffile.resids[i], erffile.resrefs[i].c_str(), erffile.restypes[i],
				erffile.offsets[i], erffile.sizes[i]);
		if(!(ext = erffile.getFileExtByResType(erffile.restypes[i])))
			return printerr("ResType Unknown");
		filename = erffile.resrefs[i] + '.' + ext;
		erffile.dumpEntryIntoFile(i, filename);
	}
	deInit(erffile);
	return true;
}

// vim:tabstop=2
