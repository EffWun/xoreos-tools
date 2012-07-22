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

#include <cstdlib>

BIFFile::BIFFile()
{
	initstages = 2;
	vrescount = frescount = offvreslist = prontowritten = 0;
	pronto = false;
	versionwanted = 1;
}

bool BIFFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_BIF) return true;
	return false;
}

int BIFFile::readHeader(void)
{
	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(8)) return errcode;
	if(binRead(3, &vrescount, &frescount, &offvreslist)) return errcode;
	if(frescount) return errcode = 14;
	inited.set(0);
	return errcode = 0;
}

int BIFFile::readResList(void)
{
	uint32 type;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offvreslist)) return errcode;
	resids.resize(vrescount);
	offsets.resize(vrescount);
	fsizes.resize(vrescount);
	restypes.resize(vrescount);
	for(uint32 i=0;i<vrescount;i++)
	{
		if(binRead(4, &resids[i], &offsets[i], &fsizes[i], &type)) return errcode;
		restypes[i] = (NWNFileType) type;
	}
	inited.set(1);
	return errcode = 0;
}

int BIFFile::init(int n)
{
	if(!reading()) return errcode = 6;
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readResList()) return errcode;
	return errcode = 0;
}

void BIFFile::deInit(void)
{
	close();
	vrescount = frescount = offvreslist = prontowritten = 0;
	resids.clear();
	offsets.clear();
	fsizes.clear();
	restypes.clear();
	for(uint32 i=0;i<datas.size();i++) if(datac[i]) free(datas[i]);
	datas.clear();
	datac.clear();
	variable.clear();
	pronto = false;
}

int BIFFile::getResListEntry(int n, uint32 &resid, uint32 &offset, uint32 &fsize,
		NWNFileType &restype)
{
	uint32 type;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offvreslist+n*16)) return errcode;
	if(binRead(4, &resid, &offset, &fsize, &type)) return errcode;
	restype = (NWNFileType) type;
	return errcode = 0;
}

int BIFFile::getIdx(uint32 resid)
{
	return (resid & 0x000FFFFF);
}

bool BIFFile::withCd(uint32 resid)
{
	if(((resid & 0xFFF00000) >> 20) == (resid & 0x000FFFFF)) return true;
	return false;
}

uint8 *BIFFile::dumpEntryIntoMem(uint32 offset, uint32 size)
{
	uint8 *mem;

	errcode = 0;
	if(!inited.isMax(0) && init(1)) return NULL;
	if(seek(offset)) return NULL;
	if(!(mem = (uint8 *) malloc(size)))
	{
		errcode = 4;
		return NULL;
	}
	if(rawRead((char *)mem, size))
	{
		free(mem);
		return NULL;
	}
	return mem;
}

uint8 *BIFFile::dumpEntryIntoMem(uint32 entry)
{
	errcode = 0;
	if(!inited.isMax(1) && init(2)) return NULL;
	if(entry >= vrescount)
	{
		errcode = 13;
		return NULL;
	}
	return dumpEntryIntoMem(offsets[entry], fsizes[entry]);
}

int BIFFile::dumpEntryIntoMem(uint32 entry, MemSeg &memseg)
{
	errcode = 0;
	if(!inited.isMax(1) && init(2)) return errcode;
	if(entry >= vrescount) return errcode = 13;
	if(!(memseg.mem = dumpEntryIntoMem(offsets[entry], fsizes[entry])))
		return errcode;
	memseg.size = fsizes[entry];
	return errcode = 0;
}

int BIFFile::dumpEntryIntoFile(uint32 offset, uint32 size, std::string fname)
{
	std::ofstream ofile;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offset)) return errcode;
	ofile.open(fname.c_str(), std::ios::out | std::ios::binary);
	if(!ofile) return errcode = 1;
	rawRead(&ofile, size);
	ofile.close();
	return errcode = 0;
}

int BIFFile::dumpEntryIntoFile(uint32 entry, std::string fname)
{
	std::ofstream ofile;

	if(!inited.isMax(1) && init(2)) return errcode;
	if(entry >= vrescount) return errcode = 13;
	return dumpEntryIntoFile(offsets[entry], fsizes[entry], fname);
}

int BIFFile::writePronto(uint32 n)
{
	pronto = true;
	vrescount = n;
	if(finalWriteCalc()) return errcode;
	if(finalWriteHeader()) return errcode;
	return errcode = 0;
}

int BIFFile::beginWrite(std::string fname, uint32 n)
{
	if(openWrite(fname)) return errcode;
	if(n && writePronto(n)) return errcode;
	return errcode = 0;
}

int BIFFile::beginWrite(uint32 n)
{
	if(openWrite()) return errcode;
	if(n && writePronto(n)) return errcode;
	return errcode = 0;
}

int BIFFile::writeData(NWNFileType restype, bool withCd, bool var,
		uint8 *memseg, uint32 size, bool copy)
{
	uint32 n;

	if(!var) return errcode = 14;
	if(pronto)
	{
		if(prontowritten >= vrescount) return errcode = 31;
		if(withCd) {
			resids.push_back((uint32) ((prontowritten << 20) + prontowritten));
			prontowritten++;
		} else resids.push_back((uint32) (prontowritten++));
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
		if(withCd) {
			resids.push_back((uint32) ((vrescount << 20) + vrescount));
			vrescount++;
		} else resids.push_back((uint32) (vrescount++));
	}
	fsizes.push_back(size);
	restypes.push_back(restype);
	variable.push_back(var);
	return errcode = 0;
}

int BIFFile::writeData(NWNFileType restype, bool withCd, bool var, std::string fname)
{
	uint32 n, size;

	if(pronto && (prontowritten++ >= vrescount)) return errcode = 31;
	datas.resize((n=datas.size())+1);
	if(!(datas[n] = readFileIntoMem(fname, size))) return errcode;
	if(pronto)
	{
		if(prontowritten >= vrescount) return errcode = 31;
		if(withCd) {
			resids.push_back((uint32) ((prontowritten << 20) + prontowritten));
			prontowritten++;
		} else resids.push_back((uint32) (prontowritten++));
		if(offsets.empty()) return errcode = 25;
		if(offsets.size() < 2) offsets.push_back(offsets[0]);
		if(seek(offsets[1])) return errcode;
		if(rawWrite((char *) datas[n], size)) return errcode;
		offsets[1] += size;
		free(datas[n]);
		datas.resize(n);
	}
	else
	{
		datac.push_back(true);
		if(withCd) {
			vrescount++;
			resids.push_back((uint32) ((vrescount << 20) + vrescount));
		} else resids.push_back((uint32) (++vrescount));
	}
	fsizes.push_back(size);
	restypes.push_back(restype);
	variable.push_back(var);
	return errcode = 0;
}

int BIFFile::endwrite(uint8 **mem, uint32 *size)
{
	if(pronto && (prontowritten < vrescount)) return errcode = 32;
	if(!pronto)
	{
		if(finalWriteCalc()) return errcode;
		if(finalWriteHeader()) return errcode;
	}
	if(finalWriteVResList()) return errcode;
	if(!pronto && finalWriteVRes()) return errcode;
	deInit();
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	return errcode = 0;
}

int BIFFile::finalWriteCalc(void)
{
	if(!pronto && ((resids.size() != fsizes.size()) ||
			(resids.size() != restypes.size()) || (resids.size() != datas.size()) ||
			(resids.size() != datac.size()) || (resids.size() != variable.size())))
		return errcode = 25;
	offvreslist = 20;
	offsets.resize(1);
	offsets[0] = offvreslist + vrescount*16;
	return errcode = 0;
}

int BIFFile::finalWriteHeader(void)
{
	char *magic;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V1  ", 4)) return errcode;
	if(binWrite(3, vrescount, frescount, offvreslist)) return errcode;
	return errcode = 0;
}

int BIFFile::finalWriteVResList(void)
{
	if(seek(offvreslist)) return errcode;
	if(offsets.empty()) return errcode = 25;
	offsets.resize(2);
	offsets[1] = offsets[0];
	for(uint32 i=0;i<vrescount;i++)
	{
		if(binWrite(4, resids[i], offsets[1], fsizes[i], (uint32) restypes[i]))
			return errcode;
		offsets[1] += fsizes[i];
		// Overflow check, if that fires, something really bad happened
		if(tell() > offsets[0]) return errcode = 23;
	}
	return errcode = 0;
}

int BIFFile::finalWriteVRes(void)
{
	if(offsets.empty()) return errcode = 25;
	if(seek(offsets[0])) return errcode;
	for(uint32 i=0;i<vrescount;i++)
		if(rawWrite((char *) datas[i], fsizes[i])) return errcode;
	return errcode = 0;
}

// vim:tabstop=2
