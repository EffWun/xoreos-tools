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

TLKFile::TLKFile()
{
	initstages = 4;
	strcount = offstrings = 0;
	language = NWN_LANG_ENGLISHM;
	versionwanted = 3;
}

bool TLKFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_TLK) return true;
	return false;
}

int TLKFile::readHeader(void)
{
	uint32 lang;

	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(8)) return errcode;
	if(binRead(3, &lang, &strcount, &offstrings)) return errcode;
	language = (NWNLang) lang;
	inited.set(0);
	return errcode = 0;
}

int TLKFile::readTable(void)
{
	uint32 flag, tmp;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(20)) return errcode;
	flags.resize(strcount);
	sndresrefs.resize(strcount);
	sndlengths.resize(strcount);
	offsets.resize(strcount);
	strlengths.resize(strcount);
	for(uint32 i=0;i<strcount;i++)
	{
		if(binRead(flag)) return errcode;
		if(rawRead(sndresrefs[i], 16)) return errcode;
		if(binRead(4, &tmp, &tmp, &offsets[i], &strlengths[i])) return errcode;
		if(binRead(sndlengths[i])) return errcode;
		lowerStr(sndresrefs[i]);
		flags[i] = (uint16) flag;
	}
	inited.set(1);
	return errcode = 0;
}

int TLKFile::readStrings(void)
{
	if(!inited.isMax(1) && init(2)) return errcode;
	strings.resize(strcount);
	for(uint32 i=0;i<strcount;i++)
	{
		if(seek(offstrings+offsets[i])) return errcode;
		if(rawRead(strings[i], strlengths[i])) return errcode;
	}
	offsets.clear();
	strlengths.clear();
	inited.set(2);
	return errcode = 0;
}

int TLKFile::init(int n)
{
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readTable()) return errcode;
	if((n > 2) && !inited[2] && readStrings()) return errcode;
	if((n > 3) && !inited[3])
	{
		close();
		inited.set(3);
	}
	return errcode = 0;
}

int TLKFile::getString(uint32 n, std::string &str)
{
	if(!inited.isMax(1) && init(2)) return errcode;
	if(n >= strcount) return errcode = 13;
	if(inited[2]) str = strings[n];
	else
	{
		if(seek(offstrings+offsets[n])) return errcode;
		if(rawRead(str, strlengths[n])) return errcode;
	}
	return errcode = 0;
}

void TLKFile::deInit(void)
{
	close();
	strcount = offstrings = 0;
	language = NWN_LANG_ENGLISHM;
	flags.clear();
	sndresrefs.clear();
	strings.clear();
	sndlengths.clear();
	offsets.clear();
	strlengths.clear();
}

int TLKFile::add(uint16 flag, std::string sndresref, float32 sndlen, std::string str)
{
	lowerStr(sndresref);
	flags.push_back(flag);
	sndresrefs.push_back(sndresref);
	sndlengths.push_back(sndlen);
	strings.push_back(str);
	return errcode = 0;
}

int TLKFile::set(uint32 n, uint16 flag, std::string sndresref, float32 sndlen,
		std::string str)
{
	if(n >= strcount) return errcode = 13;
	lowerStr(sndresref);
	flags[n] = flag;
	sndresrefs[n] = sndresref;
	sndlengths[n] = sndlen;
	strings[n] = str;
	return errcode = 0;
}

int TLKFile::del(uint32 count)
{
	if(count > (strcount-count)) count = strcount-count;
	flags.erase(flags.end()-count, flags.end()-1);
	sndresrefs.erase(sndresrefs.end()-count, sndresrefs.end()-1);
	strings.erase(strings.end()-count, strings.end()-1);
	sndlengths.erase(sndlengths.end()-count, sndlengths.end()-1);
	strcount -= count;
	return errcode = 0;
}

int TLKFile::write(std::string fname)
{
	if(openWrite(fname)) return errcode;
	if(finalWrite()) return errcode;
	close();
	return errcode = 0;
}

int TLKFile::write(uint8 **mem, uint32 *size)
{
	if(openWrite()) return errcode;
	if(finalWrite()) return errcode;
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	close();
	return errcode = 0;
}

int TLKFile::finalWrite(void)
{
	if(finalWriteCalc()) return errcode;
	if(finalWriteHeader()) return errcode;
	if(finalWriteTable()) return errcode;
	if(finalWriteStrings()) return errcode;
	return errcode = 0;
}

int TLKFile::finalWriteCalc(void)
{
	if((flags.size() != sndresrefs.size()) || (flags.size() != strings.size()) ||
		 (flags.size() != sndlengths.size()))
		return errcode = 25;
	strcount = flags.size();
	offstrings = 20 + strcount*40;
	return errcode = 0;
}

int TLKFile::finalWriteHeader(void)
{
	char *magic;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V3.0", 4)) return errcode;
	if(binWrite(3, (uint32) language, strcount, offstrings)) return errcode;
	return errcode = 0;
}

int TLKFile::finalWriteTable(void)
{
	uint32 l;

	if(seek(20)) return errcode;
	offsets.resize(1);
	offsets[0] = 0;
	for(uint32 i=0;i<strcount;i++)
	{
		if(binWrite((uint32) flags[i])) return errcode;
		// ResRefs have to have at most 16 characters
		if(sndresrefs[i].length() > 16) sndresrefs[i].erase(16);
		if(rawWrite((char*) sndresrefs[i].c_str(), l=sndresrefs[i].length()))
			return errcode;
		// Pad to a total of 16 bytes
		for(l=l;l<16;l++) if(binWrite((uint8) 0)) return errcode;
		if(binWrite(2, (uint32) 0, (uint32) 0)) return errcode;
		if(binWrite(2, offsets[0], (uint32) strings[i].length())) return errcode;
		if(binWrite(sndlengths[i])) return errcode;
		offsets[0] += strings[i].length();
	}
	return errcode = 0;
}

int TLKFile::finalWriteStrings(void)
{
	if(seek(offstrings)) return errcode;
	for(uint32 i=0;i<strcount;i++)
		if(rawWrite(strings[i])) return errcode;
	return errcode = 0;
}

// vim:tabstop=2
