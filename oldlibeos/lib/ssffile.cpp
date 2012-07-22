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

SSFFile::SSFFile()
{
	initstages = 4;
	entrycount = offtable = 0;
	versionwanted = 1;
}

bool SSFFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_SSF) return true;
	return false;
}

int SSFFile::readHeader(void)
{
	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(8)) return errcode;
	if(binRead(2, &entrycount, &offtable)) return errcode;
	inited.set(0);
	return errcode = 0;
}

int SSFFile::readEntries(void)
{
	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offtable)) return errcode;
	offsets.resize(entrycount);
	for(uint32 i=0;i<entrycount;i++)
		if(binRead(offsets[i])) return errcode;
	return errcode = 0;
}

int SSFFile::readData(void)
{
	if(!inited.isMax(1) && init(2)) return errcode;
	if(offsets.size() != entrycount) return errcode = 25;
	stringrefs.resize(entrycount);
	resrefs.resize(entrycount);
	for(uint32 i=0;i<entrycount;i++)
	{
		if(seek(offsets[i])) return errcode;
		if(rawRead(resrefs[i], 16)) return errcode;
		if(binRead(stringrefs[i])) return errcode;
	}
	offsets.clear();
	return errcode = 0;
}

int SSFFile::init(int n)
{
	if(!reading()) return errcode = 6;
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readEntries()) return errcode;
	if((n > 2) && !inited[2] && readData()) return errcode;
	if((n > 3) && !inited[3])
	{
		close();
		inited.set(3);
	}
	return errcode = 0;
}

void SSFFile::deInit(void)
{
	close();
	entrycount = offtable = 0;
	stringrefs.clear();
	resrefs.clear();
	offsets.clear();
}

int SSFFile::add(std::string resref, uint32 stringref)
{
	entrycount++;
	resrefs.push_back(resref);
	stringrefs.push_back(stringref);
	return errcode = 0;
}

int SSFFile::del(uint32 count)
{
	if(!count) return errcode = 0;
	if(count > entrycount) count = entrycount;
	resrefs.erase(resrefs.end()-count, resrefs.end()-1);
	stringrefs.erase(stringrefs.end()-count, stringrefs.end()-1);
	entrycount -= count;
	return errcode = 0;
}

int SSFFile::set(uint32 n, std::string resref, uint32 stringref)
{
	if(n >= entrycount) return errcode = 13;
	resrefs[n] = resref;
	stringrefs[n] = stringref;
	return errcode = 0;
}

int SSFFile::write(std::string fname)
{
	if(openWrite(fname)) return errcode;
	if(finalWrite()) return errcode;
	close();
	return errcode = 0;
}

int SSFFile::write(uint8 **mem, uint32 *size)
{
	if(openWrite()) return errcode;
	if(finalWrite()) return errcode;
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	close();
	return errcode = 0;
}

int SSFFile::finalWrite(void)
{
	if(finalWriteCalc()) return errcode;
	if(finalWriteHeader()) return errcode;
	if(finalWriteEntries()) return errcode;
	if(finalWriteData()) return errcode;
	return errcode = 0;
}

int SSFFile::finalWriteCalc(void)
{
	if(resrefs.size() != stringrefs.size()) return errcode = 25;
	entrycount = resrefs.size();
	offtable = 40;
	offsets.resize(1);
	offsets[0] = offtable + entrycount*4;
	return errcode = 0;
}

int SSFFile::finalWriteHeader(void)
{
	char *magic;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V1.0", 4)) return errcode;
	if(binWrite(2, entrycount, offtable)) return errcode;
	return errcode = 0;
}

int SSFFile::finalWriteEntries(void)
{
	if(offsets.empty()) return errcode = 25;
	if(seek(offtable)) return errcode;
	for(uint32 i=0;i<entrycount;i++)
		if(binWrite((uint32) (offsets[0] + i*20))) return errcode;
	return errcode = 0;
}

int SSFFile::finalWriteData(void)
{
	uint32 l;

	if(offsets.empty()) return errcode = 25;
	if(seek(offsets[0])) return errcode;
	for(uint32 i=0;i<entrycount;i++)
	{
		// ResRefs have to have at most 16 characters
		if(resrefs[i].length() > 16) resrefs[i].erase(16);
		if(rawWrite((char*) resrefs[i].c_str(), l=resrefs[i].length()))
			return errcode;
		// Pad to a total of 16 bytes
		for(l=l;l<16;l++) if(binWrite((uint8) 0)) return errcode;
		if(binWrite(stringrefs[i])) return errcode;
	}
	return errcode = 0;
}

// vim:tabstop=2
