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

#include <cstdlib>

ERFFile::ERFFile()
{
	initstages = 3;
	langcount = locstrsize = entrycount = offlocstr = offkeylist = 0;
	offreslist = buildyear = buildday = prontowritten = 0;
	desc.stringref = NOTFOUND;
	pronto = false;
	versionwanted = 1;
}

bool ERFFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_ERF) return true;
	return false;
}

int ERFFile::readHeader(void)
{
	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(8)) return errcode;
	if(binRead(9, &langcount, &locstrsize, &entrycount, &offlocstr,
			&offkeylist, &offreslist, &buildyear, &buildday, &(desc.stringref)))
		return errcode;
	// Extra format checks with the offsets
	// In MODs, every KeyList is padded with 0x00 at the end so that each
	// KeyList entry could use 32 bytes (instead of 24)
	// DOESN'T APPLY TO HAK! O_o
	// Check?
	if(((offlocstr+locstrsize) != offkeylist)) // || ((offkeylist+entrycount*32) != offreslist))
		return errcode = 24;
	inited.set(0);
	return errcode = 0;
}

int ERFFile::readDescription(void)
{
	uint32 lang;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offlocstr)) return errcode;
	for(uint32 i=0;i<langcount;i++)
	{
		if(binRead(lang)) return errcode;
		// TODO: Asian languages [low]
		// WORKAROUND
//		if(lang >= NWN_LANG_MAX) return errcode = 12;
		if(lang >= NWN_LANG_MAX) lang = 0;
		if(readExoString(desc.str[lang])) return errcode;
	}
	inited.set(1);
	return errcode = 0;
}

int ERFFile::readKeyList(void)
{
	uint16 type;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offkeylist)) return errcode;
	resrefs.resize(entrycount);
	restypes.resize(entrycount);
	resids.resize(entrycount);
	offsets.resize(entrycount);
	sizes.resize(entrycount);
	for(uint32 i=0;i<entrycount;i++)
	{
		if(rawRead(resrefs[i], 16)) return errcode;
		if(binRead(resids[i])) return errcode;
		if(binRead(type)) return errcode;
		restypes[i] = (NWNFileType) type;
		lowerStr(resrefs[i]);
		if(seek(2, std::ios::cur)) return errcode;
	}
	if(seek(offreslist)) return errcode;
	for(uint32 i=0;i<entrycount;i++)
		if(binRead(2, &offsets[i], &sizes[i])) return errcode;
	inited.set(2);
	return errcode = 0;
}

int ERFFile::init(int n)
{
	if(!reading()) return errcode = 6;
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readDescription()) return errcode;
	if((n > 2) && !inited[2] && readKeyList()) return errcode;
	return errcode = 0;
}

void ERFFile::deInit(void)
{
	close();
	langcount = locstrsize = entrycount = offlocstr = offkeylist = 0;
	offreslist = buildyear = buildday = prontowritten = 0;
	desc.stringref = NOTFOUND;
	resrefs.clear();
	restypes.clear();
	resids.clear();
	offsets.clear();
	sizes.clear();
	for(uint32 i=0;i<NWN_LANG_MAX;i++) desc.str[i].clear();
	for(uint32 i=0;i<datas.size();i++) if(datac[i]) free(datas[i]);
	datas.clear();
	datac.clear();
	pronto = false;
}

uint32 ERFFile::getEntryNum(std::string resref, NWNFileType restype)
{
	if(!inited.isMax(2) && init(3)) return ERROR;
	lowerStr(resref);
	for(uint32 i=0;i<entrycount;i++)
		if((resrefs[i] == resref) && (restypes[i] == restype)) return i;
	aderrstr = resref;
	return NOTFOUND;
}

uint8 *ERFFile::dumpEntryIntoMem(uint32 entry)
{
	uint8 *mem;

	if((entry == ERROR) && errcode) return NULL;
	if(entry == NOTFOUND)
	{
		errcode = 30;
		return NULL;
	}
	errcode = 0;
	if(!inited.isMax(2) && init(3)) return NULL;
	if(entry >= entrycount)
	{
		errcode = 13;
		return NULL;
	}
	if(seek(offsets[entry])) return NULL;
	if(!(mem = (uint8 *) malloc(sizes[entry])))
	{
		errcode = 4;
		return NULL;
	}
	if(rawRead((char *)mem, sizes[entry]))
	{
		free(mem);
		return NULL;
	}
	return mem;
}

int ERFFile::dumpEntryIntoMem(uint32 entry, MemSeg &memseg)
{
	if(!(memseg.mem = dumpEntryIntoMem(entry))) return errcode;
	memseg.size = sizes[entry];
	return errcode = 0;
}

int ERFFile::dumpEntryIntoFile(uint32 entry, std::string fname)
{
	std::ofstream ofile;

	if((entry == ERROR) && errcode) return errcode;
	if(entry == NOTFOUND) return errcode = 30;
	if(!inited.isMax(2) && init(3)) return errcode;
	if(entry >= entrycount) return errcode = 13;
	if(seek(offsets[entry])) return errcode;

	ofile.open(fname.c_str(), std::ios::out | std::ios::binary);
	if(!ofile) return errcode = 1;
	rawRead(&ofile, sizes[entry]);
	ofile.close();
	return errcode = 0;
}

int ERFFile::writePronto(uint32 n, ExoLocString &descr)
{
	pronto = true;
	entrycount = n;
	desc = descr;
	if(finalWriteCalc()) return errcode;
	if(finalWriteHeader()) return errcode;
	if(finalWriteDescription()) return errcode;
	return errcode = 0;
}

int ERFFile::beginWrite(std::string fname, uint32 n, ExoLocString *descr)
{
	if(openWrite(fname)) return errcode;
	if((n && descr) && writePronto(n, *descr)) return errcode;
	return errcode = 0;
}

int ERFFile::beginWrite(uint32 n, ExoLocString *descr)
{
	if(openWrite()) return errcode;
	if((n && descr) && writePronto(n, *descr)) return errcode;
	return errcode = 0;
}

int ERFFile::writeData(std::string resref, NWNFileType restype, uint8 *memseg,
	uint32 size, bool copy)
{
	uint32 n;

	if(pronto)
	{
		if(prontowritten++ >= entrycount) return errcode = 31;
		if(offsets.empty()) return errcode = 25;
		if(offsets.size() < 2) offsets.push_back(offsets[0]);
		if(seek(offsets[1])) return errcode;
		if(rawWrite((char *) memseg, size)) return errcode;
		offsets[1] += size;
	}
	else
	{
		datas.resize((n=datas.size())+1);
		if(copy)
		{
			if(!(datas[n] = (uint8 *) malloc(size))) return errcode = 4;
			memcpy(datas[n], memseg, size);
		}
		else datas[n] = memseg;
		datac.push_back(copy);
	}
	resrefs.push_back(resref);
	restypes.push_back(restype);
	sizes.push_back(size);
	return errcode = 0;
}

int ERFFile::writeData(std::string resref, NWNFileType restype, std::string fname)
{
	uint32 n, size;

	if(pronto && (prontowritten++ >= entrycount)) return errcode = 31;
	datas.resize((n=datas.size())+1);
	if(!(datas[n] = readFileIntoMem(fname, size))) return errcode;
	if(pronto)
	{
		if(offsets.empty()) return errcode = 25;
		if(offsets.size() < 2) offsets.push_back(offsets[0]);
		if(seek(offsets[1])) return errcode;
		if(rawWrite((char *) datas[n], size)) return errcode;
		offsets[1] += size;
		free(datas[n]);
		datas.resize(n);
	}
	else datac.push_back(true);
	resrefs.push_back(resref);
	restypes.push_back(restype);
	sizes.push_back(size);
	return errcode = 0;
}

int ERFFile::endwrite(uint8 **mem, uint32 *size)
{
	if(pronto && (prontowritten < entrycount)) return errcode = 32;
	if(!pronto)
	{
		if(finalWriteCalc()) return errcode;
		if(finalWriteHeader()) return errcode;
		if(finalWriteDescription()) return errcode;
	}
	if(finalWriteKeyList()) return errcode;
	if(finalWriteResList()) return errcode;
	if(!pronto && finalWriteEntries()) return errcode;
	deInit();
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	return errcode = 0;
}

int ERFFile::finalWriteCalc(void)
{
	if(!pronto && ((datas.size() != datac.size()) ||
			(datas.size() != resrefs.size()) ||
			(datas.size() != restypes.size()) || (datas.size() != sizes.size())))
		return errcode = 25;
	for(uint32 i=0;i<NWN_LANG_MAX;i++)
		if(!desc.str[i].empty())
		{
			langcount++;
			locstrsize += 8 + desc.str[i].length();
		}
	if(!pronto) entrycount = datas.size();
	offlocstr = 160;
	offkeylist = offlocstr + locstrsize;
	offreslist = offkeylist + entrycount*24;
	offsets.resize(1);
	offsets[0] = offreslist + entrycount*8;
	return errcode = 0;
}

int ERFFile::finalWriteHeader(void)
{
	char *magic;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V1.0", 4)) return errcode;
	if(binWrite(9, langcount, locstrsize, entrycount, offlocstr, offkeylist,
	               offreslist, buildyear, buildday, desc.stringref))
		return errcode;
	return errcode = 0;
}

int ERFFile::finalWriteDescription(void)
{
	if(seek(offlocstr)) return errcode;
	for(uint32 i=0;i<NWN_LANG_MAX;i++)
		if(!desc.str[i].empty())
		{
			if(binWrite(2, i, (uint32) desc.str[i].length())) return errcode;
			if(rawWrite((char*) desc.str[i].c_str(), desc.str[i].size()))
				return errcode;
		}
	// Overflow check, if that fires, something really bad happened
	if(tell() > offkeylist) return errcode = 23;
	return errcode = 0;
}

int ERFFile::finalWriteKeyList(void)
{
	int l;

	if(seek(offkeylist)) return errcode;
	for(uint32 i=0;i<entrycount;i++)
	{
		// ResRefs have to have at most 16 characters
		if(resrefs[i].length() > 16) resrefs[i].erase(16);
		lowerStr(resrefs[i]);
		if(rawWrite((char*) resrefs[i].c_str(), l=resrefs[i].length()))
			return errcode;
		// Pad to a total of 16 bytes
		for(l=l;l<16;l++) if(binWrite((uint8) 0)) return errcode;
		if(binWrite(i)) return errcode;
		if(binWrite((uint16) restypes[i])) return errcode;
		if(binWrite((uint16) 0)) return errcode;
		// Overflow check, if that fires, something really bad happened
		if(tell() > offreslist) return errcode = 23;
	}
	return errcode = 0;
}

int ERFFile::finalWriteResList(void)
{
	if(seek(offreslist)) return errcode;
	if(offsets.empty()) return errcode = 25;
	offsets.resize(2);
	offsets[1] = offsets[0];
	for(uint32 i=0;i<entrycount;i++)
	{
		if(binWrite(2, offsets[1], sizes[i])) return errcode;
		offsets[1] += sizes[i];
		// Overflow check, if that fires, something really bad happened
		if(tell() > offsets[0]) return errcode = 23;
	}
	return errcode = 0;
}

int ERFFile::finalWriteEntries(void)
{
	if(pronto) return errcode = 29;
	if(offsets.empty()) return errcode = 25;
	if(seek(offsets[0])) return errcode;
	for(uint32 i=0;i<entrycount;i++)
		if(rawWrite((char *) datas[i], sizes[i])) return errcode;
	return errcode = 0;
}

// vim:tabstop=2
