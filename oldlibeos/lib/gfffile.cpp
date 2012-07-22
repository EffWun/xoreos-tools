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

GFFFile::GFFFile()
{
	initstages = 3;
	structcount = fieldcount = labelcount = 0;
	fielddatasize = fieldidxsize = listidxsize = 0;
	offstruct = offfield = offlabel = 0;
	offfielddata = offfieldidx = offlistidx = 0;
	topstruct.fieldcount = datasize = 0;
	versionwanted = 3.2;
}

bool GFFFile::typeIsCorrect(void)
{
	errcode = 0;
	if(getBaseType(type) == NWN_BASE_GFF) return true;
	return false;
}

int GFFFile::readHeader(void)
{
	if(!typeIsCorrect()) return errcode = 11;
	if(!versionIsCorrect()) return errcode = 10;
	if(seek(8)) return errcode;
	if(binRead(12, &offstruct, &structcount, &offfield, &fieldcount, &offlabel,
			&labelcount, &offfielddata, &fielddatasize, &offfieldidx, &fieldidxsize,
			&offlistidx, &listidxsize))
		return errcode;
	if(!structcount) return errcode = 15;
	inited.set(0);
	return errcode = 0;
}

int GFFFile::readLabels(void)
{
	std::string str;

	if(!inited.isMax(0) && init(1)) return errcode;
	if(seek(offlabel)) return errcode;
	labels.reserve(labelcount);
	for(uint32 i=0;i<labelcount;i++)
	{
		if(rawRead(str, 16)) return errcode;
		labels.push_back(str);
	}
	inited.set(1);
	return errcode = 0;
}

int GFFFile::getStruct(uint32 n, Struct &sct)
{
	uint32 data, fieldn;

	if(!inited.isMax(1) && init(2)) return errcode;
	if(n >= structcount) return errcode = 17;
	if(seek(offstruct + n*12)) return errcode;
	if(binRead(3, &(sct.id), &data, &(sct.fieldcount))) return errcode;
	sct.labels.reserve(sct.fieldcount);
	sct.types.reserve(sct.fieldcount);
	sct.values.reserve(sct.fieldcount);
	if(sct.fieldcount == 1) // data is index into field array
	{
		if(getField(data, sct)) return errcode;
	}
	else // data is offset into field indices array
		for(uint32 i=0;i<sct.fieldcount;i++)
		{
			if(seek(offfieldidx + data + i*4)) return errcode;
			if(binRead(fieldn) || getField(fieldn, sct)) return errcode;
		}
	inited.set(2);
	return errcode = 0;
}

int GFFFile::getField(uint32 n, Struct &sct)
{
	uint32 type, lblidx, value;
	uint64 complex;

	if(!inited.isMax(1) && init(2)) return errcode;
	if(n >= fieldcount) return errcode = 18;
	if(seek(offfield + n*12)) return errcode;
	if(binRead(3, &type, &lblidx, &value)) return errcode;
	if(lblidx >= labelcount) return errcode = 16;
	sct.labels.push_back(&(labels[lblidx]));
	sct.types.push_back((NWNVarType) type);
	if((type == NWN_VAR_INT64) || (type == NWN_VAR_UINT64) ||
	   (type == NWN_VAR_FLOAT64))
	{
		if((seek(offfielddata + value)) || binRead(complex)) return errcode;
		sct.values.push_back(complex);
	}
	else sct.values.push_back(value);
	return errcode = 0;
}

int GFFFile::init(int n)
{
	if(!reading()) return errcode = 6;
	if(n == -1) n = initstages;
	if((n > 0) && !inited[0] && readHeader()) return errcode;
	if((n > 1) && !inited[1] && readLabels()) return errcode;
	if((n > 2) && !inited[2] && getStruct(0, topstruct)) return errcode;
	return errcode = 0;
}

void GFFFile::deInit(void)
{
	close();
	structcount = fieldcount = labelcount = 0;
	fielddatasize = fieldidxsize = listidxsize = 0;
	offstruct = offfield = offlabel = 0;
	offfielddata = offfieldidx = offlistidx = 0;
	datasize = 0;
	topstruct.fieldcount = 0;
	topstruct.labels.clear();
	topstruct.types.clear();
	topstruct.values.clear();
	labels.clear();
	structs.clear();
	resrefs.clear();
	exostrs.clear();
	exolocstrs.clear();
	listoffs.clear();
	for(uint32 i=0;i<datas.size();i++)
		if(datac[i]) free(datas[i]);
	datas.clear();
	datac.clear();
	datasizes.clear();
	dataoffs.clear();
}

int GFFFile::prepareGet(uint32 n, Struct &sct, NWNVarType reqtype, int32 off,
		bool dseek)
{
	if(!inited.isMax(1) && init(2)) return errcode;
	if(n == NOTFOUND) return errcode = 19;
	if(n >= sct.fieldcount) return errcode = 18;
	if(sct.types[n] != reqtype) return errcode = 11;
	if(dseek && seek(off + sct.values[n])) return errcode;
	return errcode = 0;
}

int GFFFile::getFloat(uint32 n, Struct &sct, float32 &flt)
{
	if(prepareGet(n, sct, NWN_VAR_FLOAT32, 0, false)) return errcode;
	flt = *((float32 *) &sct.values[n]);
	return errcode = 0;
}

int GFFFile::getFloat(std::string label, Struct &sct, float32 &flt)
{
	return getFloat(getIdxByLabel(label, sct), sct, flt);
}

int GFFFile::getFloat(uint32 n, Struct &sct, float64 &flt)
{
	if(prepareGet(n, sct, NWN_VAR_FLOAT64, 0, false)) return errcode;
	flt = *((float64 *) &sct.values[n]);
	return errcode = 0;
}

int GFFFile::getFloat(std::string label, Struct &sct, float64 &flt)
{
	return getFloat(getIdxByLabel(label, sct), sct, flt);
}

int GFFFile::getResRef(uint32 n, Struct &sct, std::string &str)
{
	if(prepareGet(n, sct, NWN_VAR_RESREF, offfielddata)) return errcode;
	readResRef(str);
	return errcode = 0;
}

int GFFFile::getResRef(std::string label, Struct &sct, std::string &str)
{
	return getResRef(getIdxByLabel(label, sct), sct, str);
}

int GFFFile::getExoString(uint32 n, Struct &sct, std::string &str)
{
	if(prepareGet(n, sct, NWN_VAR_EXOSTR, offfielddata)) return errcode;
	readExoString(str);
	return errcode = 0;
}

int GFFFile::getExoString(std::string label, Struct &sct, std::string &str)
{
	return getExoString(getIdxByLabel(label, sct), sct, str);
}

int GFFFile::getExoLocString(uint32 n, Struct &sct, ExoLocString &lstr)
{
	uint32 count, lang;

	if(prepareGet(n, sct, NWN_VAR_EXOLOCSTR, offfielddata+4)) return errcode;
	if(binRead(2, &(lstr.stringref), &count)) return errcode;
	for(uint32 i=0;i<count;i++)
	{
		if(binRead(lang)) return errcode;
		// TODO: Asian languages [low]
		// WORKAROUND
//		if(lang >= NWN_LANG_MAX) return errcode = 12;
		if(lang >= NWN_LANG_MAX) lang = 0;
		if(readExoString(lstr.str[lang])) return errcode;
	}
	return errcode = 0;
}

int GFFFile::getExoLocString(std::string label, Struct &sct, ExoLocString &lstr)
{
	return getExoLocString(getIdxByLabel(label, sct), sct, lstr);
}

uint8 *GFFFile::getRaw(uint32 n, Struct &sct, uint32 &size)
{
	uint8 *data;

	errcode = 0;
	if(prepareGet(n, sct, NWN_VAR_VOID, offfielddata)) return NULL;
	if(binRead(size)) return NULL;
	if(!(data = (uint8 *) malloc(size)))
	{
		errcode = 4;
		return NULL;
	}
	if(rawRead((char *)data, size))
	{
		free(data);
		return NULL;
	}
	return data;
}

uint8 *GFFFile::getRaw(std::string label, Struct &sct, uint32 &size)
{
	return getRaw(getIdxByLabel(label, sct), sct, size);
}

int GFFFile::getRaw(uint32 n, Struct &sct, MemSeg &memseg)
{
	memseg.mem = getRaw(n, sct, memseg.size);
	return errcode;
}

int GFFFile::getRaw(std::string label, Struct &sct, MemSeg &memseg)
{
	memseg.mem = getRaw(label, sct, memseg.size);
	return errcode;
}

int GFFFile::getList(uint32 n, Struct &sct, std::vector<uint32> &list)
{
	uint32 count, entry;

	if(prepareGet(n, sct, NWN_VAR_LIST, offlistidx)) return errcode;
	if(binRead(count)) return errcode;
	list.reserve(count);
	for(uint32 i=0;i<count;i++)
	{
		if(binRead(entry)) return errcode;
		list.push_back(entry);
	}
	return errcode = 0;
}

int GFFFile::getList(std::string label, Struct &sct, std::vector<uint32> &list)
{
	return getList(getIdxByLabel(label, sct), sct, list);
}

int GFFFile::getList(uint32 n, Struct &sct, std::vector<Struct> &structs)
{
	uint32 count, entry, pos;

	if(prepareGet(n, sct, NWN_VAR_LIST, offlistidx)) return errcode;
	if(binRead(count)) return errcode;
	structs.reserve(count);
	for(uint32 i=0;i<count;i++)
	{
		Struct sct;
		if(binRead(entry)) return errcode;
		pos = tell();
		getStruct(entry, sct);
		if(seek(pos)) return errcode;
		structs.push_back(sct);
	}
	return errcode = 0;
}

int GFFFile::getList(std::string label, Struct &sct, std::vector<Struct> &structs)
{
	return getList(getIdxByLabel(label, sct), sct, structs);
}

uint32 GFFFile::getIdxByLabel(std::string label, Struct &sct)
{
	if(!inited.isMax(1) && init(2)) return ERROR;
	for(uint32 i=0;i<sct.fieldcount;i++)
		if(*(sct.labels[i]) == label) return i;
	aderrstr = label;
	return NOTFOUND;
}

int GFFFile::getIdxByLabel(std::string *labels, uint32 count, GFFFile::Struct &top,
		std::vector<uint32> &n, std::string errstr)
{
	n.resize(count);
	for(uint32 i=0;i<n.size();i++)
	{
		errstr = labels[i];
		if((n[i] = getIdxByLabel(labels[i], top)) == ERROR) return errcode;
	}
	errstr.clear();
	return errcode = 0;
}

uint32 GFFFile::addLabel(std::string lbl)
{
	// Labels have to have at most 16 characters
	if(lbl.length() > 16) lbl.erase(16);
	for(uint32 i=0;i<labels.size();i++) if(labels[i] == lbl) return i;
	labels.push_back(lbl);
	return labels.size()-1;
}

int GFFFile::beginWrite(std::string fname)
{
	if(openWrite(fname)) return errcode;
	return errcode = 0;
}

int GFFFile::beginWrite(void)
{
	if(openWrite()) return errcode;
	return errcode = 0;
}

int GFFFile::writeStruct(uint32 &n, std::string lbl, uint32 parent)
{
	structs.resize((n=structs.size())+1);
	if(n)
	{
		if(parent >= structs.size()) return errcode = 17;
		structs[parent].types.push_back(NWN_VAR_STRUCT);
		// Yay, abusing pointers! ^^;
		structs[parent].values.push_back((uint64) n);
		if(lbl == "") structs[parent].labels.push_back((std::string *) -1);
		else if(writeLabel(parent, lbl)) return errcode;
		fieldcount++;
	}
	return errcode = 0;
}

int GFFFile::writeLabel(uint32 sct, std::string lbl)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].labels.push_back((std::string *) (uint64) addLabel(lbl));
	return errcode = 0;
}

int GFFFile::writeSetLabel(uint32 sct, std::string lbl)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].labels[structs[sct].labels.size()-1] = (std::string *) (uint64) addLabel(lbl);
	return errcode = 0;
}

int GFFFile::writeSetId(uint32 sct, uint32 id)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].id = id;
	return errcode = 0;
}

int GFFFile::writeVar(uint32 sct, NWNVarType type, std::string lbl, uint64 value)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].types.push_back(type);
	structs[sct].values.push_back(value);
	if(writeLabel(sct, lbl)) return errcode;
	fieldcount++;
	if((type == NWN_VAR_UINT64) || (type == NWN_VAR_INT64) ||
	   (type == NWN_VAR_FLOAT64))
		fielddatasize += 8;
	return errcode = 0;
}

int GFFFile::writeFloat(uint32 sct, std::string lbl, float32 value)
{
	return writeVar(sct, NWN_VAR_FLOAT32, lbl, reinterpret_cast<uint32&>(value));
}

int GFFFile::writeFloat(uint32 sct, std::string lbl, float64 value)
{
	return writeVar(sct, NWN_VAR_FLOAT32, lbl, reinterpret_cast<uint64&>(value));
}

int GFFFile::writeResRef(uint32 sct, std::string lbl, std::string value)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].types.push_back(NWN_VAR_RESREF);
	if(writeLabel(sct, lbl)) return errcode;
	// ResRefs have to have at most 16 characters
	if(value.length() > 16) value.erase(16);
	resrefs.push_back(value);
	structs[sct].values.push_back((uint64) (resrefs.size()-1));
	fieldcount++;
	fielddatasize += value.length()+1;
	return errcode = 0;
}

int GFFFile::writeExoString(uint32 sct, std::string lbl, std::string value)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].types.push_back(NWN_VAR_EXOSTR);
	if(writeLabel(sct, lbl)) return errcode;
	exostrs.push_back(value);
	structs[sct].values.push_back((uint64) (exostrs.size()-1));
	fieldcount++;
	fielddatasize += value.length()+4;
	return errcode = 0;
}

int GFFFile::writeExoLocString(uint32 sct, std::string lbl, ExoLocString &value)
{
	if(sct >= structs.size()) return errcode = 17;
	structs[sct].types.push_back(NWN_VAR_EXOLOCSTR);
	if(writeLabel(sct, lbl)) return errcode;
	exolocstrs.push_back(value);
	structs[sct].values.push_back((uint64) (exolocstrs.size()-1));
	fieldcount++;
	fielddatasize += 12;
	for(int i=0;i<NWN_LANG_MAX;i++)
		if(!value.str[i].empty()) fielddatasize += value.str[i].length()+8;
	return errcode = 0;
}

int GFFFile::writeList(uint32 &n, uint32 sct, std::string lbl)
{
	if(sct >= structs.size()) return errcode = 17;
	lists.resize((n=lists.size())+1);
	structs[sct].types.push_back(NWN_VAR_LIST);
	structs[sct].values.push_back((uint64) n);
	if(lbl == "") structs[sct].labels.push_back((std::string *) -1);
	else if(writeLabel(sct, lbl)) return errcode;
	fieldcount++;
	return errcode = 0;
}

int GFFFile::writeListStruct(uint32 &n, uint32 list)
{
	if(list >= lists.size()) return errcode = 22;
	structs.resize((n=structs.size())+1);
	lists[list].push_back(n);
	return errcode = 0;
}

int GFFFile::writeData(uint32 sct, std::string lbl, uint8 *memseg, uint32 size,
		bool copy)
{
	int n;

	datas.resize((n=datas.size())+1);
	if(copy)
	{
		if(!(datas[n] = (uint8 *) malloc(size))) return errcode = 4;
		memcpy(datas[n], memseg, size);
	}
	else datas[n] = memseg;
	structs[sct].types.push_back(NWN_VAR_VOID);
	structs[sct].values.push_back(n);
	if(writeLabel(sct, lbl)) return errcode;
	datasizes.push_back(size);
	dataoffs.push_back(datasize);
	datac.push_back(copy);
	datasize += size+4;
	fielddatasize += size+4;
	fieldcount++;
	return errcode = 0;
}

int GFFFile::writeData(uint32 sct, std::string lbl, MemSeg memseg, bool copy)
{
	return writeData(sct, lbl, memseg.mem, memseg.size, copy);
}

int GFFFile::writeData(uint32 sct, std::string lbl, std::string fname)
{
	uint32 n, size;

	datas.resize((n=datas.size())+1);
	if(!(datas[n] = readFileIntoMem(fname, size))) return errcode;
	structs[sct].types.push_back(NWN_VAR_VOID);
	structs[sct].values.push_back(n);
	if(writeLabel(sct, lbl)) return errcode;
	dataoffs.push_back(datasize);
	fielddatasize += size + 4;
	datasize += size + 4;
	datasizes.push_back(size);
	datac.push_back(true);
	fieldcount++;
	return errcode = 0;
}

int GFFFile::endwrite(uint8 **mem, uint32 *size)
{
	if((datas.size() != datac.size()) || (datas.size() != dataoffs.size()) ||
	   (datas.size() != datasizes.size()))
		return errcode = 25;
	for(uint32 i=0;i<structs.size();i++)
	{
		if((structs[i].fieldcount = structs[i].values.size()) > 1)
			fieldidxsize += structs[i].fieldcount*4;
		// Yeah, I know this is kind of a bad hack, but I need it that way...
		for(uint32 j=0;j<structs[i].labels.size();j++)
			if(structs[i].labels[j] == (std::string *) -1)
				structs[i].labels[j] = (std::string *) (uint64) addLabel("");
	}
	for(uint32 i=0;i<lists.size();i++)
		listidxsize += lists[i].size()*4 + 4;
	structcount = structs.size();
	labelcount = labels.size();
	offstruct = 56;
	offfield = offstruct + structcount*12;
	offlabel = offfield + fieldcount*12;
	offfielddata = offlabel + labelcount*16;
	offfieldidx = offfielddata + fielddatasize;
	offlistidx = offfieldidx + fieldidxsize;

	if(finalWriteHeader()) return errcode;
	if(finalWriteData()) return errcode;
	if(finalWriteLists()) return errcode;
	if(finalWriteLabels()) return errcode;
	if(finalWriteStructs()) return errcode;
	deInit();
	if(mem) *mem = getMem();
	if(size) *size = getMemSize();
	return 0;
}

int GFFFile::finalWriteHeader(void)
{
	char *magic;

	if(seek(0)) return errcode;
	if(!(magic = (char*) getFileMagicByResType(type))) return errcode = 11;
	if(rawWrite(magic, 4)) return errcode;
	if(rawWrite("V3.2", 4)) return errcode;
	if(binWrite(12, offstruct, structcount, offfield, fieldcount, offlabel,
	                labelcount, offfielddata, fielddatasize, offfieldidx,
	                fieldidxsize, offlistidx, listidxsize))
		return errcode;
	return errcode = 0;
}

int GFFFile::finalWriteStructs(void)
{
	uint32 ostruct=offstruct, ofield=offfield;
	uint32 ofieldd=offfielddata+datasize, ofieldi=offfieldidx;
	uint32 nfield=0;

	if(seek(ostruct)) return errcode;
	for(uint32 i=0;i<structs.size();i++)
	{
		if(binWrite(structs[i].id)) return errcode;
		if(structs[i].fieldcount == 1)
		{
			if(binWrite(nfield++)) return errcode;
			if(seek(ofield)) return errcode;
			if(finalWriteField(i, 0, ofield, ofieldd)) return errcode;
		}
		else
		{
			if(binWrite((uint32) (ofieldi-offfieldidx))) return errcode;
			for(uint32 j=0;j<structs[i].fieldcount;j++)
			{
				if(seek(ofield)) return errcode;
				if(finalWriteField(i, j, ofield, ofieldd)) return errcode;
				if(seek(ofieldi)) return errcode;
				if(binWrite(nfield++)) return errcode;
				// Overflow check, if that fires, something really bad happened
				if((ofieldi += 4) > offlistidx) return errcode = 23;
			}
		}
		if(seek(ostruct+8)) return errcode;
		if(binWrite(structs[i].fieldcount));
		// Overflow check, if that fires, something really bad happened
		if((ostruct += 12) > offfield) return errcode = 23;
	}
	return errcode = 0;
}

int GFFFile::finalWriteField(uint32 sct, uint32 f, uint32 &ofield,
		uint32 &ofieldd)
{
	NWNVarType &type = structs[sct].types[f];
	if(binWrite(2, (uint32) type, (uint32) ((uint64) structs[sct].labels[f])))
		return errcode;
	switch(type)
	{
		case NWN_VAR_UINT64: case NWN_VAR_INT64: case NWN_VAR_FLOAT64:
			if(binWrite((uint32) (ofieldd-offfielddata))) return errcode;
			if(seek(ofieldd)) return errcode;
			if(binWrite((uint64) structs[sct].values[f])) return errcode;
			// Overflow check, if that fires, something really bad happened
			if((ofieldd += 8) > offfieldidx) return errcode = 23;
			break;
		case NWN_VAR_RESREF:
			if(finalWriteResRef(resrefs[structs[sct].values[f]], ofieldd))
				return errcode;
			break;
		case NWN_VAR_EXOSTR:
			if(finalWriteExoStr(exostrs[structs[sct].values[f]], ofieldd))
				return errcode;
			break;
		case NWN_VAR_EXOLOCSTR:
			if(finalWriteExoLocStr(exolocstrs[structs[sct].values[f]], ofieldd))
				return errcode;
			break;
		case NWN_VAR_VOID:
			if(structs[sct].values[f] >= dataoffs.size()) return errcode = 13;
			if(binWrite(dataoffs[structs[sct].values[f]])) return errcode;
			break;
		case NWN_VAR_LIST:
			if(structs[sct].values[f] >= lists.size()) return errcode = 22;
			if(binWrite(listoffs[structs[sct].values[f]])) return errcode;
			break;
		default:
			if(binWrite((uint32) structs[sct].values[f])) return errcode;
			break;
	}
	// Overflow check, if that fires, something really bad happened
	if((ofield += 12) > offlabel) return errcode = 23;
	if(seek(ofield)) return errcode;

	return errcode = 0;
}

int GFFFile::finalWriteResRef(std::string &resref, uint32 &ofieldd)
{
	if(binWrite((uint32) (ofieldd-offfielddata))) return errcode;
	if(seek(ofieldd)) return errcode;
	if(binWrite((uint8) resref.size())) return errcode;
	if(rawWrite((char*) resref.c_str(), resref.size())) return errcode;
	// Overflow check, if that fires, something really bad happened
	if((ofieldd += resref.size()+1) > offfieldidx) return errcode = 23;
	return errcode = 0;
}

int GFFFile::finalWriteExoStr(std::string &exostr, uint32 &ofieldd)
{
	if(binWrite((uint32) (ofieldd-offfielddata))) return errcode;
	if(seek(ofieldd)) return errcode;
	if(binWrite((uint32) exostr.size())) return errcode;
	if(rawWrite((char*) exostr.c_str(), exostr.size())) return errcode;
	// Overflow check, if that fires, something really bad happened
	if((ofieldd += exostr.size()+4) > offfieldidx) return errcode = 23;
	return errcode = 0;
}

int GFFFile::finalWriteExoLocStr(ExoLocString &exolocstr, uint32 &ofieldd)
{
	uint32 size=0, count=0;
	if(binWrite((uint32) (ofieldd-offfielddata))) return errcode;
	if(seek(ofieldd+12)) return errcode;
	for(uint32 i=0;i<NWN_LANG_MAX;i++)
		if(!exolocstr.str[i].empty())
		{
			if(binWrite(2, i, (uint32) exolocstr.str[i].size()))
				return errcode;
			if(rawWrite((char*) exolocstr.str[i].c_str(), exolocstr.str[i].size()))
				return errcode;
			count++;
			size += exolocstr.str[i].size()+8;
		}
	if(seek(ofieldd)) return errcode;
	if(binWrite(3, (uint32) size+8, exolocstr.stringref, count)) return errcode;
	// Overflow check, if that fires, something really bad happened
	if((ofieldd += size+12) > offfieldidx) return errcode = 23;
	return errcode = 0;
}

int GFFFile::finalWriteLists(void)
{
	if(seek(offlistidx)) return errcode;
	for(uint32 i=0;i<lists.size();i++)
	{
		listoffs.push_back(tell()-offlistidx);
		if(binWrite((uint32) lists[i].size())) return errcode;
		for(uint32 j=0;j<lists[i].size();j++)
			if(binWrite(lists[i][j])) return errcode;
	}
	return errcode = 0;
}

int GFFFile::finalWriteLabels(void)
{
	int l;
	if(seek(offlabel)) return errcode;
	for(uint32 i=0;i<labels.size();i++)
	{
		if(rawWrite(labels[i].c_str(), l=labels[i].length()))
			return errcode;
		// Pad to a total of 16 bytes
		for(l=l;l<16;l++) if(binWrite((uint8) 0)) return errcode;
	}
	return errcode = 0;
}

int GFFFile::finalWriteData(void)
{
	if(seek(offfielddata)) return errcode;
	for(uint32 i=0;i<datas.size();i++)
	{
		if(binWrite(datasizes[i])) return errcode;
		if(binWrite(datas[i], datasizes[i])) return errcode;
//		offfielddata += datasizes[i] + 4;
	}
	return errcode = 0;
}

// vim:tabstop=2
