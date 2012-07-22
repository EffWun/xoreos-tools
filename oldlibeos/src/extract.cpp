/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
