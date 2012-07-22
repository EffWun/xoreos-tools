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
#include "info.h"
#include "common.h"

bool infonwn(char *fname)
{
	NWNFile nwnfile;
	if(!init(nwnfile, fname)) return false;
	return deInit(nwnfile);
}

bool infoerf(char *fname)
{
	ERFFile erffile;

	if(!init(erffile, fname)) return false;
	std::cout.fill('0');
	std::cout << "Buildyear: " << erffile.buildyear+1900 << " (";
	std::cout << erffile.buildyear << ") Buildday: " << erffile.buildday << "\n";
	std::cout << "# Objects packed: " << erffile.entrycount << "\n";
	std::cout << "LocStrList: Offset = " << erffile.offlocstr << "\n";
	std::cout << "            # Description strings = " << erffile.langcount << "\n";
	std::cout << "            Size std::string = " << erffile.locstrsize << " bytes\n";
	std::cout << "            Description StrRef = ";
	std::cout << std::hex << std::uppercase << "0x" << erffile.desc.stringref << std::dec << "\n";
	for(int i=0;i<12;i++)
		if(!erffile.desc.str[i].empty())
		{
			printf("=== LANG: %02u ===\n", i);
			std::cout << erffile.desc.str[i] << "\n";
		}
	std::cout << "================\n";
	std::cout << "KeyList: Offset = " << erffile.offkeylist << "\n";
	std::cout << "ResList: Offset = " << erffile.offreslist << "\n";
	for(uint32 i=0;i<erffile.entrycount;i++)
		printf("- %5u: %-16s (Type: %u, Offset: %u, Size: %u)\n",
				erffile.resids[i], erffile.resrefs[i].c_str(), erffile.restypes[i],
				erffile.offsets[i], erffile.sizes[i]);
	deInit(erffile);
	return true;
}

bool infotwoda(char *fname)
{
	TwoDAFile twodafile;

	if(!init(twodafile, fname)) return false;
	std::cout << "Default: \" " << twodafile.defstr << "\"\n";
	std::cout << "# of cols: " << twodafile.names.size() << "\n";
	std::cout << "Colnames:\n  ";
	for(uint32 i=0;i<twodafile.names.size();i++)
		std::cout << "\"" << twodafile.names[i] << "\", ";
	std::cout << "\n# of rows: " << twodafile.getNumRows() << "\n";
	deInit(twodafile);
	return true;
}

bool infokey(char *fname)
{
	KEYFile keyfile;
	std::string resref;
	NWNFileType restype;
	uint32 index;

	if(!init(keyfile, fname)) return false;
	std::cout << "Buildyear: " << keyfile.buildyear+1900 << " (";
	std::cout << keyfile.buildyear << ") Buildday: " << keyfile.buildday << "\n";
	std::cout << "# of BIFs: " << keyfile.bifcount << "\n";
	std::cout << "# of KEYs: " << keyfile.keycount << "\n";
	std::cout << "Offset to BIF list: " << keyfile.offbiflist << "\n";
	std::cout << "Offset to KEY list: " << keyfile.offkeylist << "\n";
	std::cout << "--- BIFS and their KEYs: ---\n";
	for(uint32 i=0;i<keyfile.bifcount;i++)
	{
		printf("- %5u: \"%s\" (Drive: %u, Size: %u)\n", i,
				keyfile.fnames[i].c_str(), keyfile.drives[i], keyfile.fsizes[i]);
		for(uint32 j=0;j<keyfile.getNumResources(i);j++)
		{
			if(keyfile.getKeyEntry(i, j, resref, restype, index))
			{
				std::cerr << "Error reading KEY " << j << " from BIF " << i << ": ";
				std::cerr << keyfile.getStrError() << "\n";
				return false;
			}
			printf("  - %5u: %-16s (Type: %u, Idx: %u)\n", j, resref.c_str(),
					restype, index);
		}
	}
	deInit(keyfile);
	return true;
}

bool infobif(char *fname)
{
	BIFFile biffile;

	if(!init(biffile, fname)) return false;
	std::cout << "# of variable ressources: " << biffile.vrescount << "\n";
	std::cout << "# of fixed ressources: " << biffile.frescount << "\n";
	for(uint32 i=0;i<biffile.vrescount;i++)
		printf("- %5u (Index: %u, WithCD: %u, Size: %u, Type: %u)\n", i,
				biffile.getIdx(biffile.resids[i]), biffile.withCd(biffile.resids[i]),
				biffile.fsizes[i], biffile.restypes[i]);
	deInit(biffile);
	return true;
}

bool infogff(char *fname)
{
	GFFFile gfffile;

	if(!init(gfffile, fname)) return false;
	std::cout << "# of elements in Struct: " << gfffile.structcount << "\n";
	std::cout << "# of elements in Field Array: " << gfffile.fieldcount << "\n";
	std::cout << "# of elements in Label Array: " << gfffile.labelcount << "\n";
	std::cout << "Size of Field Data Block: " << gfffile.fielddatasize << "\n";
	std::cout << "Size of Field Indices Block: " << gfffile.fieldidxsize << "\n";
	std::cout << "Size of List Indices Block: " << gfffile.listidxsize << "\n";
	std::cout << "Offset to Struct Array: " << gfffile.offstruct << "\n";
	std::cout << "Offset to Field Array: " << gfffile.offfield << "\n";
	std::cout << "Offset to Label Array: " << gfffile.offlabel << "\n";
	std::cout << "Offset to Field Data: " << gfffile.offfielddata << "\n";
	std::cout << "Offset to Field Indices: " << gfffile.offfieldidx << "\n";
	std::cout << "Offset to List Indices: " << gfffile.offlistidx << "\n";
	std::cout << "--- Labels: ---\n";
	for(uint32 i=0;i<gfffile.labelcount;i++)
		printf("- %5u: \"%s\"\n", i, gfffile.labels[i].c_str());
	std::cout << "--- Top Level Struct: ---\n";
	for(uint32 i=0;i<gfffile.topstruct.fieldcount;i++)
		printf("- %5u: \"%s\"\n", i, gfffile.topstruct.labels[i]->c_str());
	deInit(gfffile);
	return true;
}

bool infossf(char *fname)
{
	SSFFile ssffile;

	if(!init(ssffile, fname)) return false;
	std::cout << "# of entries: " << ssffile.entrycount << "\n";
	for(uint32 i=0;i<ssffile.entrycount;i++)
		printf("- %5u: \"%s\" (%u)\n", i, ssffile.resrefs[i].c_str(),
				ssffile.stringrefs[i]);
	deInit(ssffile);
	return true;
}

bool infotlk(char *fname)
{
	TLKFile tlkfile;

	if(!init(tlkfile, fname)) return false;
	std::cout << "# of strings: " << tlkfile.strcount << "\n";
	std::cout << "Language: " << tlkfile.language << "\n";
	for(uint32 i=0;i<tlkfile.strcount;i++)
	{
		printf("- %5u: \"%s\": \"%s\"\n", i, tlkfile.sndresrefs[i].c_str(),
				tlkfile.strings[i].c_str());
	}
	deInit(tlkfile);
	return true;
}

// vim:tabstop=2
