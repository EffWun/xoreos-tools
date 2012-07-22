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

#include <string>

KEYFile::KEYFile()
{
	initstages = 4;
	bifcount = keycount = offbiflist = offkeylist = buildyear = buildday = 0;
	versionwanted = 1;
}

bool KEYFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_KEY) return true;
	return false;
}

int KEYFile::readHeader(void)
{
	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(8)) return errcode;
	if(binRead(6, &bifcount, &keycount, &offbiflist, &offkeylist,
			&buildyear, &buildday))
		return errcode;
	inited.set(0);
	return errcode = 0;
}

int KEYFile::readBifList(void)
{
	uint32 offset;
	uint16 length;
	long curpos;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offbiflist)) return errcode;
	fsizes.resize(bifcount);
	fnames.resize(bifcount);
	drives.resize(bifcount);
	for(uint32 i=0;i<bifcount;i++)
	{
		if(binRead(2, &fsizes[i], &offset)) return errcode;
		if(binRead(2, &length, &drives[i])) return errcode;
		curpos = tell();
		if(seek(offset)) return errcode;
		if(rawRead(fnames[i], length)) return errcode;
		// TODO: Really always use '/' as a dir-separator? [high]
		fnames[i].replace(fnames[i].begin(), fnames[i].end(), '\\', '/');
		if(seek(curpos)) return errcode;
	}
	inited.set(1);
	return errcode = 0;
}

int KEYFile::readKeyList(void)
{
	uint16 restype;
	uint32 n, bif, resid;
	std::string resref;

	if(!inited.isMax(1) && init(2)) return errcode;
	if(seek(offkeylist)) return errcode;
	resources.resize(bifcount);
	for(uint32 i=0;i<keycount;i++)
	{
		if(rawRead(resref, 16) || binRead(restype) || binRead(resid))
			return errcode;
		if((bif = ((resid & 0xFFF00000) >> 20)) >= bifcount) return errcode = 28;
		lowerStr(resref);
		resources[bif].resize((n=resources[bif].size()) + 1);
		resources[bif][n].index = (resid & 0x000FFFFF);
		resources[bif][n].resref = resref;
		resources[bif][n].restype = (NWNFileType) restype;
	}
	inited.set(2);
	return errcode = 0;
}

int KEYFile::init(int n)
{
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readBifList()) return errcode;
	if((n > 2) && !inited[2] && readKeyList()) return errcode;
	if((n > 3) && !inited[3])
	{
		close();
		inited.set(3);
	}
	return errcode = 0;
}

void KEYFile::deInit(void)
{
	close();
	bifcount = keycount = 0;
	offbiflist = offkeylist = offfnamelist = 0;
	buildyear = buildday = 0;
	fsizes.clear();
	drives.clear();
	fnames.clear();
	for(uint32 i=0;i<resources.size();i++) resources[i].clear();
	resources.clear();
}

int KEYFile::getListEntry(uint32 n, std::string &fname, uint16 &drive, uint32 &size)
{
	uint32 offset;
	uint16 length;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offbiflist + n*12)) return errcode;
	if(binRead(2, &size, &offset)) return errcode;
	if(binRead(2, &length, &drive)) return errcode;
	if(seek(offset)) return errcode;
	if(rawRead(fname, length)) return errcode;
	fname.replace(fname.begin(), fname.end(), '\\', '/');
	return errcode = 0;
}

int KEYFile::getKeyEntry(uint32 n, std::string &resref, NWNFileType &restype,
		uint32 &resid)
{
	uint16 type;

	if(inited[2]) return errcode = 29;
	if(!inited.isMax(1) && init(2)) return errcode;
	if(n >= keycount) return errcode = 13;
	if(seek(offkeylist + n*22)) return errcode;
	if(rawRead(resref, 16) || binRead(type) || binRead(resid)) return errcode;
	lowerStr(resref);
	restype = (NWNFileType) type;
	return errcode = 0;
}

int KEYFile::getKeyEntry(uint32 bif, uint32 n, std::string &resref,
		NWNFileType &restype, uint32 &index)
{
	if(!inited.isMax(2) && init(3)) return errcode;
	if(bif >= bifcount) return errcode = 28;
	if(n >= resources[bif].size()) return errcode = 13;
	resref = resources[bif][n].resref;
	restype = resources[bif][n].restype;
	index = resources[bif][n].index;
	return errcode = 0;
}

int KEYFile::getKeyEntry(std::string resref, NWNFileType restype, uint32 &bif,
		uint32 &n)
{
	for(uint32 i=0;i<bifcount;i++)
	{
		for(uint32 j=0;j<resources[i].size();j++)
		{
			if((resources[i][j].resref == resref) &&
				 (resources[i][j].restype == restype))
			{
				bif = j;
				n = i;
				return errcode = 0;
			}
		}
	}
	return errcode = 20;
}

uint32 KEYFile::getNumResources(uint32 bif)
{
	if(bif >= bifcount) return 0;
	return resources[bif].size();
}

int KEYFile::addBif(std::string fname, uint16 drive, uint32 fsize)
{
	fsizes.push_back(fsize);
	drives.push_back(drive);
	fnames.push_back(fname);
	resources.resize(resources.size()+1);
	bifcount++;
	return errcode = 0;
}

int KEYFile::addResource(uint32 bif, std::string &resref, NWNFileType restype,
		uint32 index)
{
	uint32 n;

	if(bif >= bifcount) return errcode = 28;
	lowerStr(resref);
	resources[bif].resize((n=resources[bif].size()) + 1);
	resources[bif][n].index = index;
	resources[bif][n].resref = resref;
	resources[bif][n].restype = (NWNFileType) restype;
	keycount++;
	return errcode = 0;
}

int KEYFile::delBif(uint32 n, uint32 count)
{
	if(n >= bifcount) return errcode = 28;
	if(count > (bifcount-n)) count = bifcount-n;
	resources.erase(resources.begin()+n, resources.begin()+n+count-1);
	fsizes.erase(fsizes.begin()+n, fsizes.begin()+n+count-1);
	drives.erase(drives.begin()+n, drives.begin()+n+count-1);
	fnames.erase(fnames.begin()+n, fnames.begin()+n+count-1);
	bifcount -= n;
	return errcode = 0;
}

int KEYFile::delResource(uint32 bif, uint32 n, uint32 count)
{
	if(bif >= bifcount) return errcode = 28;
	if(n >= resources[bif].size()) return errcode = 13;
	if(count > (resources[bif].size()-n)) count = resources[bif].size()-n;
	resources[bif].erase(resources[bif].begin()+n, resources[bif].begin()+n+count-1);
	return errcode = 0;
}

uint32 KEYFile::getIdxFile(uint32 resid)
{
	return ((resid & 0xFFF00000) >> 20);
}

uint32 KEYFile::getIdxBif(uint32 resid)
{
	return (resid & 0x000FFFFF);
}

int KEYFile::write(std::string fname)
{
	if(openWrite(fname)) return errcode;
	if(finalWrite()) return errcode;
	close();
	return errcode = 0;
}

int KEYFile::write(uint8 **mem, uint32 *size)
{
	if(openWrite()) return errcode;
	if(finalWrite()) return errcode;
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	return errcode = 0;
}

int KEYFile::finalWrite(void)
{
	if(finalWriteCalc()) return errcode;
	if(finalWriteHeader()) return errcode;
	if(finalWriteBifList()) return errcode;
	if(finalWriteFNameList()) return errcode;
	if(finalWriteKeyList()) return errcode;
	return errcode = 0;
}

int KEYFile::finalWriteCalc(void)
{
	if((fsizes.size() != drives.size()) || (fnames.size() != drives.size()))
		return errcode = 25;
	offbiflist = 64;
	offfnamelist = offkeylist = offbiflist + bifcount*12;
	for(uint32 i=0;i<fnames.size();i++)
		offkeylist += fnames[i].length();
	return errcode = 0;
}

int KEYFile::finalWriteHeader(void)
{
	char *magic;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V1  ", 4)) return errcode;
	if(binWrite(6, bifcount, keycount, offbiflist, offkeylist,
	               buildyear, buildday))
		return errcode;
	return errcode = 0;
}

int KEYFile::finalWriteBifList(void)
{
	uint32 ofname=offfnamelist;

	if(seek(offbiflist)) return errcode;
	for(uint32 i=0;i<bifcount;i++)
	{
		if(binWrite(2, fsizes[i], ofname)) return errcode;
		if(binWrite((uint16) fnames[i].length())) return errcode;
		if(binWrite(drives[i])) return errcode;
		ofname += fnames[i].length();
		// Overflow check, if that fires, something really bad happened
		if(tell() > offfnamelist) return errcode = 23;
	}
	return errcode = 0;
}

int KEYFile::finalWriteFNameList(void)
{
	if(seek(offfnamelist)) return errcode;
	for(uint32 i=0;i<bifcount;i++)
	{
		if(rawWrite(fnames[i])) return errcode;
		// Overflow check, if that fires, something really bad happened
		if(tell() > offkeylist) return errcode = 23;
	}
	return errcode = 0;
}

int KEYFile::finalWriteKeyList(void)
{
	uint32 l;

	if(seek(offkeylist)) return errcode;
	for(uint32 i=0;i<resources.size();i++)
		for(uint32 j=0;j<resources[i].size();j++)
		{
			Resource &res = resources[i][j];
			// ResRefs have to have at most 16 characters
			if(res.resref.length() > 16) res.resref.erase(16);
			if(rawWrite((char*) res.resref.c_str(), l=res.resref.length()))
				return errcode;
			// Pad to a total of 16 bytes
			for(l=l;l<16;l++) if(binWrite((uint8) 0)) return errcode;
			if(binWrite((uint16) res.restype)) return errcode;
			if(binWrite((uint32) ((i << 20) + res.index))) return errcode;
		}
	return errcode = 0;
}

// vim:tabstop=2
