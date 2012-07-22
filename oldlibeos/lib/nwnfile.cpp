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

const char *NWNFile::file_magic[] =
{
	"RES ", "BMP ", "MVE ", "TGA ", "WAV ", "PLT ", "INI ", "BMU ", "MPG ",
	"TXT ", "PLH ", "TEX ", "MDL ", "THG ", "FNT ", "LUA ", "SLT ", "NSS ",
	"NCS ", "MOD ", "ARE ", "SET ", "IFO ", "BIC ", "WOK ", "2DA ", "TLK ",
	"TXI ", "GIT ", "BTI ", "UTI ", "BTC ", "UTC ", "DLG ", "ITP ", "BTT ",
	"UTT ", "DDS ", "UTS ", "LTR ", "GFF ", "FAC ", "BTE ", "UTE ", "BTD ",
	"UTD ", "BTP ", "UTP ", "DFT ", "DTF ", "GIC ", "GUI ", "CSS ", "CCS ",
	"BTM ", "UTM ", "DWK ", "PWK ", "BTG ", "UTG ", "JRL ", "SAV ", "UTW ",
	"4PC ", "SSF ", "HAK ", "NWM ", "BIK ", "NDB ", "PTM ", "PTT ", "1DA ",
	"ERF ", "BIFF", "KEY ", "GVT ", "PT  ", "NFO "
};

const char *NWNFile::file_ext[] =
{
	"res", "bmp", "mve", "tga", "wav", "plt", "ini", "bmu", "mpg",
	"txt", "plh", "tex", "mdl", "thg", "fnt", "lua", "slt", "nss",
	"ncs", "mod", "are", "set", "ifo", "bic", "wok", "2da", "tlk",
	"txi", "git", "bti", "uti", "btc", "utc", "dlg", "itp", "btt",
	"utt", "dds", "uts", "ltr", "gff", "fac", "bte", "ute", "btd",
	"utd", "btp", "utp", "dft", "dtf", "gic", "gui", "css", "ccs",
	"btm", "utm", "dwk", "pwk", "btg", "utg", "jrl", "sav", "utw",
	"4pc", "ssf", "hak", "nwm", "bik", "ndb", "ptm", "ptt", "1da",
	"erf", "bif", "key", "res", "res", "res"
};

const int NWNFile::file_restype[] =
{
	0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 2000, 2001, 2002, 2003, 2005, 2007, 2008,
	2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2022, 2023,
	2024, 2025, 2026, 2027, 2029, 2030, 2031, 2032, 2033, 2035, 2036, 2037,
	2038, 2039, 2040, 2041, 2042, 2043, 2044, 2045, 2045, 2046, 2047, 2048,
	2049, 2050, 2051, 2052, 2053, 2054, 2055, 2056, 2057, 2058, 2059, 2060,
	2061, 2062, 2063, 2064, 2065, 2066, 9996, 9997, 9998, 9999, 20000, 20001, 20002
};

NWNFile::NWNFile()
{
	initstages = 1;
	type = NWN_FILE_UNDEFINED;
	version = 0;
	versionwanted = -1;
}

NWNBaseType NWNFile::getBaseType(NWNFileType type)
{
	if(type == NWN_FILE_UNDEFINED) return NWN_BASE_UNDEFINED;
	if((type == NWN_FILE_MOD) || (type == NWN_FILE_SAV) || (type == NWN_FILE_HAK) ||
	   (type == NWN_FILE_NWM) || (type == NWN_FILE_ERF))
		return NWN_BASE_ERF;
	if(type == NWN_FILE_2DA) return NWN_BASE_2DA;
	if(type == NWN_FILE_KEY) return NWN_BASE_KEY;
	if(type == NWN_FILE_BIF) return NWN_BASE_BIF;
	if((type == NWN_FILE_IFO) || (type == NWN_FILE_ARE) || (type == NWN_FILE_GIC) ||
	   (type == NWN_FILE_UTC) || (type == NWN_FILE_UTD) || (type == NWN_FILE_UTE) ||
	   (type == NWN_FILE_UTI) || (type == NWN_FILE_UTS) || (type == NWN_FILE_UTM) ||
	   (type == NWN_FILE_UTT) || (type == NWN_FILE_UTW) || (type == NWN_FILE_DLG) ||
	   (type == NWN_FILE_JRL) || (type == NWN_FILE_FAC) || (type == NWN_FILE_ITP) ||
	   (type == NWN_FILE_PTM) || (type == NWN_FILE_PTT) || (type == NWN_FILE_BIC) ||
		 (type == NWN_FILE_GIT) || (type == NWN_FILE_UTP) || (type == NWN_FILE_GUI))
		return NWN_BASE_GFF;
	if((type == NWN_FILE_GVT) || (type == NWN_FILE_PT) || (type == NWN_FILE_NFO))
		return NWN_BASE_GFF;
	if(type == NWN_FILE_SSF) return NWN_BASE_SSF;
	if(type == NWN_FILE_TLK) return NWN_BASE_TLK;
	return NWN_BASE_OTHER;
}

int NWNFile::open(std::string fname)
{
	close();
	if(openRead(fname)) return errcode;
	if(checkFormat()) return errcode;
	return errcode = 0;
}

int NWNFile::open(uint8 *memseg, uint32 size)
{
	close();
	if(openRead(memseg, size) || checkFormat()) return errcode;
	return errcode = 0;
}

int NWNFile::open(MemSeg &memseg)
{
	close();
	if(openRead(memseg.mem, memseg.size) || checkFormat()) return errcode;
	return errcode = 0;
}

int NWNFile::checkFormat(void)
{
	if((type = getFileType()) == NWN_FILE_UNDEFINED)
	{
		binClose();
		return errcode = 24;
	}
	version = getFileVersion();
	if(errcode)
	{
		binClose();
		return errcode;
	}
	return errcode = 0;
}

void NWNFile::close(void)
{
	binClose();
}

int NWNFile::readHeader(void)
{
	inited.set(0);
	return errcode = 0;
}

int NWNFile::init(int n)
{
	if(!reading()) return errcode = 6;
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	return errcode = 0;
}

void NWNFile::deInit(void)
{
	close();
	type = NWN_FILE_UNDEFINED;
	inited.clear();
	version = errcode = 0;
}

NWNFileType NWNFile::getFileType(void)
{
	std::string buf;

	if(seek(0) || rawRead(buf, 4)) return NWN_FILE_UNDEFINED;
	return getFileResTypeByMagic(buf);
}

float64 NWNFile::getFileVersion(void)
{
	char buf[5];
	float64 version=0;

	if(seek(4) || rawRead(buf, 4)) return errcode = 24;
	buf[4] = '\0';
	sscanf(buf, "V" S_FLOAT64, &version);
	return version;
}

const char *NWNFile::getFileMagicByResType(int restype)
{
	for(int i=0;i<(int)(sizeof(file_restype)/sizeof(int));i++)
		if(file_restype[i] == restype) return file_magic[i];
	return NULL;
}

const char *NWNFile::getFileMagicByExt(std::string ext)
{
	for(int i=0;i<(int)(sizeof(file_ext)/sizeof(char *));i++)
		if(file_ext[i] == ext) return file_magic[i];
	return NULL;
}

const char *NWNFile::getFileExtByResType(int restype)
{
	for(int i=0;i<(int)(sizeof(file_restype)/sizeof(int));i++)
		if(file_restype[i] == restype) return file_ext[i];
	return NULL;
}

const char *NWNFile::getFileExtByMagic(std::string magic)
{
	for(int i=0;i<(int)(sizeof(file_magic)/sizeof(char *));i++)
		if(file_magic[i] == magic) return file_ext[i];
	return NULL;
}

NWNFileType NWNFile::getFileResTypeByMagic(std::string magic)
{
	for(int i=0;i<(int)(sizeof(file_magic)/sizeof(char *));i++)
		if(file_magic[i] == magic) return (NWNFileType) file_restype[i];
	return (NWNFileType) -1;
}

NWNFileType NWNFile::getFileResTypeByExt(std::string ext)
{
	for(int i=0;i<(int)(sizeof(file_ext)/sizeof(char *));i++)
		if(file_ext[i] == ext) return (NWNFileType) file_restype[i];
	return (NWNFileType) -1;
}

bool NWNFile::typeIsCorrect(void)
{
	errcode = 0;
	if(type == NWN_FILE_UNDEFINED) return false;
	return true;
}

bool NWNFile::versionIsCorrect(void)
{
	errcode = 0;
	if((versionwanted == -1) || (version == versionwanted)) return true;
	return false;
}

int NWNFile::readExoString(std::string &str)
{
	uint32 size;
	if(binRead(size) || rawRead(str, size)) return errcode;
	return errcode = 0;
}

int NWNFile::readResRef(std::string &str)
{
	uint8 size;
	if(binRead(size) || rawRead(str, size)) return errcode;
	return errcode = 0;
}

// vim:tabstop=2
