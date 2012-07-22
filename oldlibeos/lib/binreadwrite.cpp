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

BinReadWrite::BinReadWrite()
{
	opmode = memsize = 0;
	mems = mem = NULL;
	tofree = false;
}

BinReadWrite::~BinReadWrite()
{
	binClose();
}

BinReadWrite &BinReadWrite::operator=(const BinReadWrite &src)
{
	errcode = src.errcode;
	opmode = src.opmode;
	filename = src.filename;
	if(opmode == 1)
	{
		binfile = new std::fstream;
		binfile->open(filename.c_str(), std::ios::in | std::ios::binary);
	}
	else if(opmode == 3)
	{
		binfile = new std::fstream;
		binfile->open(filename.c_str(), std::ios::out | std::ios::binary);
	}
	memsize = src.memsize;
	if(!(mems = (uint8*) malloc(memsize))) errcode = 4;
	else memcpy(mems, src.mems, memsize);
	mem = mems + (src.mem-src.mems);
	tofree = true;
	return *this;
}

BinReadWrite::BinReadWrite(const BinReadWrite &src)
{
	*this = src;
}

bool BinReadWrite::fail(std::ios *stream)
{
	if(!stream) stream = binfile;
	if(stream->fail())
	{
		stream->clear();
		return true;
	}
	return false;
}

int BinReadWrite::openRead(std::string fname)
{
	binClose();
	mems = mem = NULL;
	binfile = new std::fstream;
	binfile->open(fname.c_str(), std::ios::in | std::ios::binary);
	if(!(*binfile)) return errcode = 1;
	filename = fname;
	opmode = 1;
	return errcode = 0;
}

int BinReadWrite::openRead(uint8 *memseg, uint32 size)
{
	binClose();
	mems = mem = memseg;
	memsize = size;
	opmode = 2;
	return errcode = 0;
}

int BinReadWrite::openWrite(std::string fname)
{
	binClose();
	mems = mem = NULL;
	binfile = new std::fstream;
	binfile->open(fname.c_str(), std::ios::out | std::ios::binary);
	if(!(*binfile)) return errcode = 1;
	filename = fname;
	opmode = 3;
	return errcode = 0;
}

int BinReadWrite::openWrite(void)
{
	binClose();
	memsize = 0;
	if(!(mems = (uint8*) malloc(1))) return errcode = 4;
	mem = mems;
	opmode = 4;
	return errcode = 0;
}

int BinReadWrite::reserve(uint32 size)
{
	if((opmode != 3) && (opmode != 4)) return errcode = 7;
	if(opmode == 3) return errcode = 0;
	long offset = mem - mems;
	if(!(mems = (uint8*) realloc(mems, memsize += size))) return errcode = 4;
	mem = mems + offset;
	return errcode = 0;
}

void BinReadWrite::binClose(void)
{
	errcode = 0;
	filename.erase();
	if(((opmode == 1) || (opmode == 3)) && binfile->is_open())
	{
		binfile->close();
		delete binfile;
	}
	else if(opmode == 2)
	{
		if(mems && tofree) free(mems);
		mems = mem = NULL;
		memsize = 0;
		tofree = false;
	}
	opmode = 0;
}

uint32 BinReadWrite::swap32(uint32 x)
{
	return (x>>24) |
	      ((x<<8) & 0x00FF0000) |
	      ((x>>8) & 0x0000FF00) | (x<<24);
}

uint64 BinReadWrite::swap64(uint64 x)
{
	return (x>>56) |
	      ((x<<40) & 0x00FF000000000000ll) |
	      ((x<<24) & 0x0000FF0000000000ll) |
	      ((x<<8)  & 0x000000FF00000000ll) |
	      ((x>>8)  & 0x00000000FF000000ll) |
	      ((x>>24) & 0x0000000000FF0000ll) |
	      ((x>>40) & 0x000000000000FF00ll) | (x<<56);
}

uint8 *BinReadWrite::readFileIntoMem(std::string fname, uint32 &size)
{
	std::ifstream datafile;
	uint8 *data, *buf;
	uint32 ss;

	datafile.open(fname.c_str(), std::ios::in | std::ios::binary);
	if(!datafile)
	{
		errcode = 1;
		return NULL;
	}
	datafile.seekg(0, std::ios::end);
	if(!(data = buf = (uint8*) malloc(size = ss = datafile.tellg())))
	{
		errcode = 4;
		return NULL;
	}
	datafile.seekg(0, std::ios::beg);
	while(ss)
	{
		uint32 s=MIN(2048, ss), g;
		if((g = ((uint32) datafile.readsome((char *) buf, s))) == 0)
		{
			free(data);
			errcode = 3;
			return NULL;
		}
		ss -= g;
		buf += g;
	}
	return data;
}

int BinReadWrite::rawRead(char *buf, uint32 n)
{
	if((opmode != 1) && (opmode != 2)) return errcode = 6;

	if(opmode == 1)
	{
		binfile->read(buf, n);
		if(fail()) return errcode = 3;
	}
	else if(opmode == 2)
	{
		memcpy(buf, mem, n);
		mem += n;
	}
	return errcode = 0;
}

int BinReadWrite::rawRead(std::string &str, uint32 n)
{
	int e;

	if((opmode != 1) && (opmode != 2)) return errcode = 6;

	if(!n)
	{
		str.clear();
		return errcode = 0;
	}
	if(opmode == 1)
	{
		char buf[n];
		binfile->read(buf, n);
		if(fail()) return errcode = 3;
		for(e=n-1;e>=0;e--) if(buf[e]) break;
		if(e >= 0) str.assign(buf, e+1);
		else str.clear();
	}
	else if(opmode == 2)
	{
		if((mem+n) > (mems+memsize)) return errcode = 3;
		for(e=n-1;e>=0;e--) if(mem[e]) break;
		if(e >= 0) str.assign((char *) mem, e+1);
		else str.clear();
		mem += n;
	}
	return errcode = 0;
}

int BinReadWrite::rawRead(std::ofstream *ofile, uint32 n)
{
	if((opmode != 1) && (opmode != 2)) return errcode = 6;

	if(opmode == 1)
	{
		char buf[2048];
		while(n > 0)
		{
			uint32 i=MIN(2048, n);
			if(rawRead(buf, i)) return errcode;
			ofile->write(buf, i);
			if(fail(ofile)) return errcode = 5;
			n -= i;
		}
	}
	else if(opmode == 2)
	{
		ofile->write((const char*)mem, n);
		if(fail(ofile)) return errcode = 5;
		mem += n;
	}
	return errcode = 0;
}

int BinReadWrite::rawReadLine(std::string &str)
{
	int n;

	if((opmode != 1) && (opmode != 2)) return errcode = 6;

	if(opmode == 1)
	{
		getline(*binfile, str);
		if(!str.empty() && ((n = str.find('\r')) > -1)) str.erase(n);
		if(fail()) return errcode = 3;
	}
	else if(opmode == 2)
	{
		uint8 *it=mem;
		if(it >= (mems+memsize)) return errcode = 3;
		while(it < (mems+memsize))
			if(*it++ == '\n') break;
		if((it-mem-1) > 0)
		{
			str.assign((char*) mem, it-mem-1);
			if((n = str.find('\r')) > -1) str.erase(n);
		}
		else str.erase();
		mem = it;
	}
	return errcode = 0;
}

int BinReadWrite::rawWrite(const char *buf, uint32 n)
{
	if((opmode != 3) && (opmode != 4)) return errcode = 7;

	if(!n) return errcode = 0;
	if(opmode == 3)
	{
		binfile->write(buf, n);
		if(fail()) return errcode = 3;
	}
	else if(opmode == 4)
	{
		int nfree = memsize-(mem-mems);
		if(nfree < (int32) n) reserve(n-nfree);
		memcpy(mem, buf, n);
		mem += n;
	}
	return errcode = 0;
}

int BinReadWrite::rawWrite(std::string &str)
{
	return rawWrite((char *) str.c_str(), str.length());
}

int BinReadWrite::seek(long offset, std::_Ios_Seekdir whence)
{
	if(!opmode) return errcode = 2;
	if((opmode == 1) || (opmode == 3))
	{
		binfile->seekg(offset, whence);
		if(fail()) return errcode = 3;
	}
	else
	{
		int tr;
		switch(whence)
		{
			case std::ios::beg:
				if((opmode == 4) && ((tr = (offset-memsize)) > 0))
					reserve(tr);
				mem = mems + offset;
				break;
			case std::ios::cur:
				if((opmode == 4) && (tr = ((mem-mems)+offset) > 0))
					reserve(tr);
				mem += offset;
				break;
			case std::ios::end:
				mem = mems + memsize - offset;
				if(mem < mems) mem = mems;
				break;
			default:
				return errcode = 9;
				break;
		}
		if((mem < mems) || (mem > (mems + memsize))) return errcode = 3;
	}
	return errcode = 0;
}

bool BinReadWrite::reading(void)
{
	if((opmode == 1) || (opmode == 2)) return true;
	return false;
}

bool BinReadWrite::writing(void)
{
	if((opmode == 3) || (opmode == 4)) return true;
	return false;
}

uint32 BinReadWrite::tell(void)
{
	if(!opmode) return 0;
	if((opmode == 1) || (opmode == 3))
		return binfile->tellg();
	else
		return mem-mems;
}

uint32 BinReadWrite::getFileSize(void)
{
	if(!opmode) return 0;
	if((opmode == 2) || (opmode == 4)) return memsize;

	uint32 cpos = tell();
	if(seek(0, std::ios::end)) return 0;
	uint32 epos = tell();
	if(seek(cpos)) return 0;
	return epos;
}

uint32 BinReadWrite::getMemSize(void)
{
	return memsize;
}

uint8 *BinReadWrite::getMem(void)
{
	return mems;
}

// vim:tabstop=2
