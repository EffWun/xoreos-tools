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

TwoDAFile::TwoDAFile()
{
	initstages = 3;
	tablestart = -1;
	versionwanted = 2;
}

bool TwoDAFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_2DA) return true;
	return false;
}

int TwoDAFile::readHeader(void)
{
	std::string str;
	int n;
	uint32 start=0;

	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(0)) return errcode;
	if(rawReadLine(str)) return errcode;

	defstr.clear();
	if(rawReadLine(str)) return errcode;
	if((n = str.find("DEFAULT:")) > -1)
		getElementStr(str, defstr, n+8);

	names.clear();
	if(rawReadLine(str)) return errcode;
	do
		names.resize((n=names.size())+1);
	while(getElementStr(str, names[n], start, &start) != 1);
	names.pop_back();

	tablestart = tell();
	inited.set(0);
	return errcode = 0;
}

int TwoDAFile::readArray(void)
{
	std::string str;
	uint32 n;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(tablestart == -1) return errcode = 24;
	if(seek(tablestart)) return errcode;
	while(!rawReadLine(str))
	{
		array.resize((n=array.size())+1);
		getElements(str, array[n]);
	}
	inited.set(1);
	return errcode = 0;
}

int TwoDAFile::init(int n)
{
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readArray()) return errcode;
	if((n > 2) && !inited[2])
	{
		close();
		inited.set(2);
	}
	return errcode = 0;
}

void TwoDAFile::deInit(void)
{
	close();
	tablestart = -1;
	defstr.clear();
	names.clear();
	for(uint32 i=0;i<array.size();i++) array[i].clear();
	array.clear();
}

uint32 TwoDAFile::getNumRows(void)
{
	return array.size();
}

uint32 TwoDAFile::getNumCols(void)
{
	return names.size();
}

void TwoDAFile::strToVal(std::string &str, int32 &val)
{
	int64 tmp;
	if(sscanf(str.c_str(), S_INT64, &tmp)) val = (int32) tmp;
	else val = 0;
}

void TwoDAFile::strToVal(std::string &str, float32 &val)
{
	if(!sscanf(str.c_str(), S_FLOAT32, &val)) val = 0;
}

void TwoDAFile::strToVal(std::string &str, std::string &val)
{
	val = str;
}

void TwoDAFile::valToStr(int32 val, std::string &str)
{
	std::stringstream strs;

	strs << val;
	str = strs.str();
}

void TwoDAFile::valToStr(float32 val, std::string &str)
{
	std::stringstream strs;

	strs.precision(6);
	strs << val;
	str = strs.str();
}

void TwoDAFile::valToStr(std::string &val, std::string &str)
{
	str = val;
}

bool TwoDAFile::hasSpace(std::string &val)
{
	for(uint32 i=0;i<val.length();i++)
		if(val[i] == ' ') return true;
	return false;
}

uint32 TwoDAFile::getColByName(std::string name)
{
	for(uint32 i=0;i<names.size();i++)
		if(names[i] == name) return i;
	return NOTFOUND;
}

int TwoDAFile::getElementStr(std::string &str, std::string &elem, uint32 start,
		uint32 *found)
{
	uint32 s=start;
	char delim = ' ';

	while((s < str.length()) && (str[s] == ' ')) s++;
	if(s >= str.length())
	{
		if(found) *found = 0;
		elem = defstr;
		return 1;
	}
	if(str[s] == '\"') delim = str[s++];
	uint32 e = s+1;
	while((e < str.length()) && (str[e] != delim)) e++;
	elem.assign(str, s, e-s);
	if(delim == '\"') e++;
	if(found) *found = e;

	return 0;
}

int TwoDAFile::getElements(std::string &str, std::vector<std::string> &list)
{
	uint32 n, start=0;

	for(uint32 i=0;i<=names.size();i++)
	{
		list.resize((n=list.size())+1);
		getElementStr(str, list[n], start, &start);
		if(!i) list.pop_back();
	}
	return errcode = 0;
}

int TwoDAFile::addRow(void)
{
	uint32 n;

	array.resize((n=array.size())+1);
	array[n].resize(names.size());
	return n;
}

int TwoDAFile::addColumn(std::string name)
{
	names.push_back(name);
	for(uint32 i=0;i<array.size();i++) array[i].push_back("");
	return names.size()-1;
}

int TwoDAFile::delRow(uint32 row)
{
	if(row >= array.size()) return errcode = 13;
	for(uint32 i=0;i<array[row].size();i++) array[row][i] = "****";
	return errcode = 0;
}

int TwoDAFile::delColumn(uint32 col)
{
	if(col >= names.size()) return errcode = 13;
	for(uint32 i=0;i<array.size();i++)
		if(col >= array[i].size()) return errcode = 27;
		else array[i][col]  = "****";
	return errcode = 0;
}

int TwoDAFile::write(std::string fname)
{
	if(openWrite(fname)) return errcode;
	if(finalWrite()) return errcode;
	close();
	return errcode = 0;
}

int TwoDAFile::write(uint8 **mem, uint32 *size)
{
	if(openWrite()) return errcode;
	if(finalWrite()) return errcode;
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	close();
	return errcode = 0;
}

inline uint32 TwoDAFile::max(uint32 a, uint32 b)
{
	if(a > b) return a;
	return b;
}

int TwoDAFile::colLengths(std::vector<uint32> &lengths)
{
	uint32 n=2, i=array.size();

	while(i /= 10) n++;
	lengths.push_back(n);
	for(i=0;i<names.size();i++)
	{
		n = names[i].length();
		for(uint32 j=0;j<array.size();j++)
		{
			if(i >= array[j].size()) return errcode = 27;
			n = max(n, array[j][i].length());
		}
		lengths.push_back(n+1);
	}
	return errcode = 0;
}

int TwoDAFile::finalWrite(void)
{
	char *magic;
	std::string tmp;
	bool space;
	std::vector<uint32> lengths;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V2.0\r\n", 6)) return errcode;
	if(!defstr.empty())
	{
		if(rawWrite("DEFAULT: ", 9)) return errcode;
		if(rawWrite(defstr)) return errcode;
	}
	if(rawWrite("\r\n", 2)) return errcode;
	if(colLengths(lengths)) return errcode;
	for(uint32 l=0;l<=lengths[0];l++)
		if(rawWrite(" ", 1)) return errcode;
	for(uint32 i=0;i<names.size();i++)
	{
		if(rawWrite(names[i])) return errcode;
		for(uint32 l=names[i].length();l<=lengths[i+1];l++)
			if(rawWrite(" ", 1)) return errcode;
	}
	for(uint32 i=0;i<array.size();i++)
	{
		if(rawWrite("\r\n", 2)) return errcode;
		valToStr((int32) i, tmp);
		if(rawWrite(tmp)) return errcode;
		for(uint32 l=tmp.length();l<=lengths[0];l++)
			if(rawWrite(" ", 1)) return errcode;
		for(uint32 j=0;j<array[i].size();j++)
		{
			if((space = hasSpace(array[i][j])))
				if(rawWrite("\"", 1)) return errcode;
			if(rawWrite(array[i][j])) return errcode;
			if(space && rawWrite("\"", 1)) return errcode;
			for(uint32 l=array[i][j].length();l<=lengths[j+1];l++)
				if(rawWrite(" ", 1)) return errcode;
		}
	}
	if(rawWrite("\r\n", 2)) return errcode;
	return errcode = 0;
}

// vim:tabstop=2
