/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#include <cstring>
#include <libgen.h>
#include <eos.h>
#include "assemble.h"
#include "common.h"

#include <cstdlib>

extern char *validgfftags[];
extern char *validerftags[];
extern char *validkeytags[];
extern char *validbiftags[];
extern char *validssftags[];
extern char *validkeybiftags[];
extern char *validtlktags[];
extern char *langs[];

uint8 *expd(XMLFile &xml, NWNBaseType base, std::string filen, uint32 &size)
{
	uint8 *data;

	switch(base)
	{
		case NWN_BASE_GFF:
			if(!assemblegff((char*) filen.c_str(), &data, &size)) return NULL;
			break;
		case NWN_BASE_KEY:
			if(!assemblekey((char*) filen.c_str(), &data, &size)) return NULL;
			break;
		case NWN_BASE_BIF:
			if(!assemblebif((char*) filen.c_str(), &data, &size)) return NULL;
			break;
		case NWN_BASE_ERF:
			if(!assembleerf((char*) filen.c_str(), &data, &size)) return NULL;
			break;
		case NWN_BASE_SSF:
			if(!assemblessf((char*) filen.c_str(), &data, &size)) return NULL;
			break;
		default:
			printxmlerr(xml, "Don't know how to assemble that");
			return NULL;
			break;
	}
	return data;
}

bool assemblegff(char *fname)
{
	return assemblegff(fname, NULL, NULL);
}

bool assemblegff(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	uint32 n=0;
	XMLFile xml;
	GFFFile gff;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
//		std::cout << pr << ": " << xml.section << " = \"" << xml.value << "\"\n";
			case 0:
				if(!mem && (xml.section == ".gff.filename"))
				{
					if(gff.beginWrite(xml.value))
						return printxmlerr(xml, gff.getStrError());
				}
				else if(xml.section == ".gff.filetype")
				{
					if((gff.type = gff.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!gff.typeIsCorrect()) return printxmlerr(xml, "No GFF type");
				}
				break;
			case 1:
				if(xml.section == ".gff.struct")
				{
					if(n)
						return printxmlerr(xml, "Must have exactly one top level struct");
					if(!mem)
					{
						if(gff.filename.empty()) return printxmlerr(xml, "Missing filename");
					}
					else
					{
						if(gff.beginWrite())
							return printxmlerr(xml, gff.getStrError());
					}
					if(gff.writeStruct(n, "", 0))
						return printxmlerr(xml, gff.getStrError());
					if(!assemblegffstruct(xml, gff, n++, NULL)) return false;
				}
				else if((xml.section != ".?xml") && (xml.section != ".gff"))
				{
					if(getpos(validgfftags, GFFTAGS, xml.section) == -1)
						return printxmlerr(xml, "Invalid tag");
					else
						return printxmlerr(xml, "Element outside the top level struct");
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(gff.endwrite(mem, size)) return printxmlerr(xml, gff.getStrError());
	return true;
}

bool assemblegffstruct(XMLFile &xml, GFFFile &gff, uint32 n, std::string *slbl)
{
	int pr, i;
	std::string sl, lbl, valstr;
	uint64 value=0, id=0;

	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(rsubstr(xml.section, 13) == ".struct.label")
				{
					if(slbl) *slbl = xml.value;
				}
				else if(rsubstr(xml.section, 3) == ".id")
				{
					sscanf(xml.value.c_str(), S_UINT64, &id);
					if(gff.writeSetId(n, (uint32) id))
						return printxmlerr(xml, gff.getStrError());
				}
				else if(rsubstr(xml.section, 6) == ".label")
					lbl = xml.value;
				else if(rsubstr(xml.section, 12) == ".uint8.value")
					sscanf(xml.value.c_str(), S_UINT64, &value);
				else if(rsubstr(xml.section, 11) == ".int8.value")
					sscanf(xml.value.c_str(), S_INT64, &value);
				else if(rsubstr(xml.section, 13) == ".uint16.value")
					sscanf(xml.value.c_str(), S_UINT64, &value);
				else if(rsubstr(xml.section, 12) == ".int16.value")
					sscanf(xml.value.c_str(), S_INT64, &value);
				else if(rsubstr(xml.section, 13) == ".uint32.value")
					sscanf(xml.value.c_str(), S_UINT64, &value);
				else if(rsubstr(xml.section, 12) == ".int32.value")
					sscanf(xml.value.c_str(), S_INT64, &value);
				else if(rsubstr(xml.section, 13) == ".uint64.value")
					sscanf(xml.value.c_str(), S_UINT64, &value);
				else if(rsubstr(xml.section, 12) == ".int64.value")
					sscanf(xml.value.c_str(), S_INT64, &value);
				else if(rsubstr(xml.section, 12) == ".float.value")
					sscanf(xml.value.c_str(), S_FLOAT32, (float32*) &value);
				else if(rsubstr(xml.section, 13) == ".double.value")
					sscanf(xml.value.c_str(), S_FLOAT64, (float64*) &value);
				else if(rsubstr(xml.section, 13) == ".resref.value")
					valstr = xml.value;
				else if(rsubstr(xml.section, 10) == ".data.file")
				{
					if(gff.writeData(n, lbl, xml.value))
						return printxmlerr(xml, gff.getStrError());
				}
				else if(rsubstr(xml.section, 7) == ".exostr")
					valstr = unquotestr(xml.value);
				break;
			case 1:
				if(getpos(validgfftags, GFFTAGS, xml.section) == -1)
					return printxmlerr(xml, "Invalid tag");
				if(rsubstr(xml.section, 7) == ".struct")
				{
					uint32 nsct;
					sl.clear();
					if(gff.writeStruct(nsct, "", n))
						return printxmlerr(xml, gff.getStrError());
					if(!assemblegffstruct(xml, gff, nsct, &sl)) return false;
					if(gff.writeSetLabel(n, sl))
						return printxmlerr(xml, gff.getStrError());
				}
				else if(rsubstr(xml.section, 5) == ".list")
				{
					uint32 list;
					std::string lbl;
					if(gff.writeList(list, n, ""))
						return printxmlerr(xml, gff.getStrError());
					if(!assemblegfflist(xml, gff, list, &lbl)) return false;
					if(gff.writeSetLabel(n, lbl));
				}
				else if(rsubstr(xml.section, 10) == ".exolocstr")
				{
					if(!assemblegffexolocstr(xml, gff, n)) return false;
				}
				break;
			case 2:
				if((i = getpos(validgfftags, 10, xml.value)) != -1)
				{
					if(gff.writeVar(n, (NWNVarType) i, lbl, value))
						return printxmlerr(xml, gff.getStrError());
					lbl = valstr = "";
					value = 0;
				}
				else if(rsubstr(xml.value, 7) == ".resref")
				{
					if(gff.writeResRef(n, lbl, valstr))
						return printxmlerr(xml, gff.getStrError());
					lbl = valstr = "";
					value = 0;
				}
				else if(rsubstr(xml.value, 7) == ".exostr")
				{
					if(gff.writeExoString(n, lbl, valstr))
						return printxmlerr(xml, gff.getStrError());
					lbl = valstr = "";
					value = 0;
				}
				else if(rsubstr(xml.value, 7) == ".struct") return true;
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());
	printxmlerr(xml, "Unexpected end of file");
	return false;
}

bool assemblegffexolocstr(XMLFile &xml, GFFFile &gff, uint32 n)
{
	int pr, lang=0;
	std::string lbl;
	ExoLocString exo;
	uint64 tmp;

	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(rsubstr(xml.section, 6) == ".label") lbl = xml.value;
				if(rsubstr(xml.section, 7) == ".strref")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					exo.stringref = (uint32) tmp;
				}
				if(rsubstr(xml.section, 16) == ".substr.language")
				{
					if((lang = getpos(langs, LANGS, xml.value)) == -1)
						return printxmlerr(xml, "Invalid language");
				}
				if(rsubstr(xml.section, 7) == ".substr")
					exo.str[lang] = unquotestr(xml.value);
				break;
			case 1:
				if(rsubstr(xml.section, 7) != ".substr")
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(rsubstr(xml.value, 10) == ".exolocstr")
				{
					if(gff.writeExoLocString(n, lbl, exo))
						return printxmlerr(xml, gff.getStrError());
					return true;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());
	printxmlerr(xml, "Unexpected end of file");
	return false;
}

bool assemblegfflist(XMLFile &xml, GFFFile &gff, uint32 n, std::string *llbl)
{
	int pr;

	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(rsubstr(xml.section, 11) == ".list.label")
					if(llbl) *llbl = xml.value;
				break;
			case 1:
				if(rsubstr(xml.section, 7) == ".struct")
				{
					uint32 nsct;
					if(gff.writeListStruct(nsct, n))
						return printxmlerr(xml, gff.getStrError());
					if(!assemblegffstruct(xml, gff, nsct, NULL)) return false;
				}
				else printxmlerr(xml, "Element invalid in list");
				break;
			case 2:
				if(rsubstr(xml.value, 5) == ".list")
					return true;
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());
	printxmlerr(xml, "Unexpected end of file");
	return false;
}

bool assembleerf(char *fname)
{
	return assembleerf(fname, NULL, NULL);
}

bool assembleerf(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	uint32 asize;
	uint64 tmp;
	std::vector<uint8 *> datas;
	bool opened=false, exploded=false;
	std::string resref, filename;
	NWNFileType restype=NWN_FILE_UNDEFINED;
	XMLFile xml;
	ERFFile erf;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(!mem && (xml.section == ".erf.filename"))
				{
					if(erf.beginWrite(xml.value))
						return printxmlerr(xml, erf.getStrError());
				}
				else if(xml.section == ".erf.filetype")
				{
					if((erf.type = erf.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!erf.typeIsCorrect()) return printxmlerr(xml, "No ERF type");
				}
				else if(xml.section == ".erf.buildyear")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					erf.buildyear = (uint32) (tmp - 1900);
				}
				else if(xml.section == ".erf.buildday")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					erf.buildday = (uint32) tmp;
				}
				else if(xml.section == ".erf.resource.resref")
					resref = xml.value;
				else if(xml.section == ".erf.resource.filename")
					filename = xml.value;
				else if(xml.section == ".erf.resource.restype")
				{
					if((restype = erf.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
				}
				else if(xml.section == ".erf.resource.exploded")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					exploded = (bool) tmp;
				}
				break;
			case 1:
				if(xml.section == ".erf.description") {
					if(!assembleerfdesc(xml, erf)) return false;
				} else if((xml.section != ".?xml") && (xml.section != ".erf") &&
				        (getpos(validerftags, ERFTAGS, xml.section) == -1))
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(xml.value == ".erf.resource")
				{
					if(!opened)
					{
						if(!mem)
						{
							if(erf.filename.empty())
								return printxmlerr(xml, "Missing filename");
						}
						else
							if(erf.beginWrite()) return printxmlerr(xml, erf.getStrError());
						opened = true;
					}
					if(resref.empty()) return printxmlerr(xml, "Missing resref");
					if(filename.empty()) return printxmlerr(xml, "Missing filename");
					if(restype == NWN_FILE_UNDEFINED)
						return printxmlerr(xml, "Missing restype");
					if(exploded)
					{
						datas.resize((tmp=datas.size())+1);
						if(!(datas[tmp] = expd(xml, erf.getBaseType(restype), filename, asize)))
							return false;
						if(erf.writeData(resref, restype, datas[tmp], asize, false))
							return printxmlerr(xml, erf.getStrError());
					}
					else
						if(erf.writeData(resref, restype, filename))
							return printxmlerr(xml, erf.getStrError());
					resref.clear();
					filename.clear();
					exploded = false;
					restype=NWN_FILE_UNDEFINED;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(erf.endwrite(mem, size)) return printxmlerr(xml, erf.getStrError());
	for(uint32 i=0;i<datas.size();i++) free(datas[i]);
	return true;
}

bool assembleerfdesc(XMLFile &xml, ERFFile &erf)
{
	int pr, lang=0;
	ExoLocString exo;
	uint64 tmp;

	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(rsubstr(xml.section, 7) == ".strref")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					exo.stringref = (uint32) tmp;
				}
				if(rsubstr(xml.section, 16) == ".substr.language")
				{
					if((lang = getpos(langs, LANGS, xml.value)) == -1)
						return printxmlerr(xml, "Invalid language");
				}
				if(rsubstr(xml.section, 7) == ".substr")
					exo.str[lang] = unquotestr(xml.value);
				break;
			case 1:
				if(rsubstr(xml.section, 7) != ".substr")
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(rsubstr(xml.value, 12) == ".description")
				{
					erf.desc = exo;
					return true;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());
	printxmlerr(xml, "Unexpected end of file");
	return false;
}

bool assemblekey(char *fname)
{
	return assemblekey(fname, NULL, NULL);
}

bool assemblekey(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	std::string filename, resref;
	uint16 drives=0;
	uint32 filesize=0, index=0xFFFFFFFF;
	uint64 tmp;
	NWNFileType restype=NWN_FILE_UNDEFINED;
	bool newbif=true;
	XMLFile xml;
	KEYFile key;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(!mem && (xml.section == ".key.filename"))
					key.filename = xml.value;
				else if(xml.section == ".key.filetype")
				{
					if((key.type = key.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!key.typeIsCorrect()) return printxmlerr(xml, "No KEY type");
				}
				else if(xml.section == ".key.buildyear")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					key.buildyear = (uint32) (tmp - 1900);
				}
				else if(xml.section == ".key.buildday")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					key.buildday = (uint32) tmp;
				}
				else if(xml.section == ".key.bif.filename")
					filename = xml.value;
				else if(xml.section == ".key.bif.filesize")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					filesize = (uint32) tmp;
				}
				else if(xml.section == ".key.bif.drives")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					drives = (uint16) tmp;
				}
				else if(xml.section == ".key.bif.resource.resref")
					resref = xml.value;
				else if(xml.section == ".key.bif.resource.index")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					index = (uint32) tmp;
				}
				else if(xml.section == ".key.bif.resource.restype")
				{
					if((restype = key.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
				}
				break;
			case 1:
				if((xml.section != ".?xml") && (xml.section != ".key") &&
				   (getpos(validkeytags, KEYTAGS, xml.section) == -1))
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(xml.value == ".key.bif")
				{
					newbif = true;
					filesize = drives = 0;
					filename.clear();
				}
				else if(xml.value == ".key.bif.resource")
				{
					if(newbif)
					{
						if(filename.empty()) return printxmlerr(xml, "Missing filename");
						if(key.addBif(filename, drives, filesize))
						return printxmlerr(xml, key.getStrError());
						newbif = false;
					}
					if(resref.empty()) return printxmlerr(xml, "Missing resref");
					if(restype == NWN_FILE_UNDEFINED)
						return printxmlerr(xml, "Missing restype");
					if(index == 0xFFFFFFFF) printxmlerr(xml, "Missing index");
					if(key.addResource(key.bifcount-1, resref, restype, index))
						return printxmlerr(xml, key.getStrError());
					resref.clear();
					restype = NWN_FILE_UNDEFINED;
					index = 0xFFFFFFFF;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(!mem)
	{
		if(key.filename.empty()) return printxmlerr(xml, "Missing filename");
		else if(key.write(key.filename)) return printnwnerr(key);
	}
	else if(key.write(mem, size)) return printnwnerr(key);
	return true;
}

bool assemblebif(char *fname)
{
	return assemblebif(fname, NULL, NULL);
}

bool assemblebif(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	uint32 asize;
	uint64 tmp;
	std::vector<uint8 *> datas;
	std::string filename;
	bool opened=false, exploded=false, withCd=false;
	NWNFileType restype=NWN_FILE_UNDEFINED;
	XMLFile xml;
	BIFFile bif;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(!mem && (xml.section == ".bif.filename"))
				{
					if(bif.beginWrite(xml.value))
						return printxmlerr(xml, bif.getStrError());
				}
				else if(xml.section == ".bif.filetype")
				{
					if((bif.type = bif.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!bif.typeIsCorrect()) return printxmlerr(xml, "No BIF type");
				}
				else if(xml.section == ".bif.variable.resource.filename")
					filename = xml.value;
				else if(xml.section == ".bif.variable.resource.restype")
				{
					if((restype = bif.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
				}
				else if(xml.section == ".bif.variable.resource.exploded")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					exploded = (bool) tmp;
				}
				else if(xml.section == ".bif.variable.resource.withCd")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					withCd = (bool) tmp;
				}
				break;
			case 1:
				if(xml.section == ".bif.fixed")
					return printxmlerr(xml, "Fixed resources aren't supported");
				else if((xml.section != ".?xml") && (xml.section != ".bif") &&
				        (getpos(validbiftags, BIFTAGS, xml.section) == -1))
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(xml.value == ".bif.variable.resource")
				{
					if(!opened)
					{
						if(!mem)
						{
							if(bif.filename.empty())
								return printxmlerr(xml, "Missing filename");
						}
						else
							if(bif.beginWrite()) return printxmlerr(xml, bif.getStrError());
						opened = true;
					}
					if(filename.empty()) return printxmlerr(xml, "Missing filename");
					if(restype == NWN_FILE_UNDEFINED)
						return printxmlerr(xml, "Missing restype");
					if(exploded)
					{
						datas.resize((tmp=datas.size())+1);
						if(!(datas[tmp] = expd(xml, bif.getBaseType(restype), filename, asize)))
							return false;
						if(bif.writeData(restype, withCd, true, datas[tmp], asize, false))
							return printxmlerr(xml, bif.getStrError());
					}
					else
						if(bif.writeData(restype, withCd, true, filename))
							return printxmlerr(xml, bif.getStrError());
					filename.clear();
					exploded = withCd = false;
					restype=NWN_FILE_UNDEFINED;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(bif.endwrite(mem, size)) return printxmlerr(xml, bif.getStrError());
	for(uint32 i=0;i<datas.size();i++) free(datas[i]);
	return true;
}

bool assemblessf(char *fname)
{
	return assemblessf(fname, NULL, NULL);
}

bool assemblessf(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	uint32 stringref=0xFFFFFFFF;
	uint64 tmp;
	std::string resref;
	XMLFile xml;
	SSFFile ssf;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(!mem && (xml.section == ".ssf.filename"))
					ssf.filename = xml.value;
				else if(xml.section == ".ssf.filetype")
				{
					if((ssf.type = ssf.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!ssf.typeIsCorrect()) return printxmlerr(xml, "No SSF type");
				}
				else if(xml.section == ".ssf.sound.resref")
					resref = xml.value;
				else if(xml.section == ".ssf.sound.stringref")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					stringref = (uint32) tmp;
				}
				break;
			case 1:
				if((xml.section != ".?xml") && (xml.section != ".ssf") &&
						(getpos(validssftags, SSFTAGS, xml.section) == -1))
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(xml.value == ".ssf.sound")
				{
					if(ssf.add(resref, stringref))
						return printxmlerr(xml, ssf.getStrError());
					resref.clear();
					stringref = 0xFFFFFFFF;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(!mem)
	{
		if(ssf.filename.empty()) return printxmlerr(xml, "Missing filename");
		else if(ssf.write(ssf.filename)) return printnwnerr(ssf);
	}
	else if(ssf.write(mem, size)) return printnwnerr(ssf);
	return true;
}

bool assemblekeybif(char *fname)
{
	return assemblekeybif(fname, NULL, NULL);
}

bool assemblekeybif(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	std::string bfname, rfname, resref;
	std::vector<uint8 *> datas;
	uint16 drives=0;
	uint32 asize;
	uint64 tmp;
	NWNFileType restype=NWN_FILE_UNDEFINED;
	bool newbif=true, withCd=false, exploded=false;
	XMLFile xml;
	KEYFile key;
	BIFFile bif;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(!mem && (xml.section == ".key.filename"))
					key.filename = xml.value;
				else if(xml.section == ".key.filetype")
				{
					if((key.type = key.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!key.typeIsCorrect()) return printxmlerr(xml, "No KEY type");
				}
				else if(xml.section == ".key.buildyear")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					key.buildyear = (uint32) (tmp - 1900);
				}
				else if(xml.section == ".key.buildday")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					key.buildday = (uint32) tmp;
				}
				else if(xml.section == ".key.bif.filename")
					bfname = xml.value;
				else if(xml.section == ".key.bif.drives")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					drives = (uint16) tmp;
				}
				else if(xml.section == ".key.bif.variable.resource.resref")
					resref = xml.value;
				else if(xml.section == ".key.bif.variable.resource.filename")
					rfname = xml.value;
				else if(xml.section == ".key.bif.variable.resource.restype")
				{
					if((restype = key.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
				}
				else if(xml.section == ".key.bif.variable.resource.withCd")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					withCd = (bool) tmp;
				}
				else if(xml.section == ".key.bif.variable.resource.exploded")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					exploded = (bool) tmp;
				}
				break;
			case 1:
				if(xml.section == ".bif.fixed")
					return printxmlerr(xml, "Fixed resources aren't supported");
				else if((xml.section != ".?xml") && (xml.section != ".key") &&
				   (getpos(validkeybiftags, KEYBIFTAGS, xml.section) == -1))
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(xml.value == ".key.bif")
				{
					bfname = bif.filename;
					if(bif.endwrite()) return printxmlerr(xml, bif.getStrError());
					if(bif.open(bfname)) return printxmlerr(xml, bif.getStrError());
					key.fsizes[key.bifcount-1] = bif.getFileSize();
					bif.deInit();
					newbif = true;
					bfname.clear();
					drives = 0;
				}
				else if(xml.value == ".key.bif.variable.resource")
				{
					if(newbif)
					{
						if(bfname.empty()) return printxmlerr(xml, "Missing filename");
						if(key.addBif(bfname, drives, 0))
							return printxmlerr(xml, key.getStrError());
						if(bif.beginWrite(getbasename((char*)bfname.c_str())))
							return printxmlerr(xml, bif.getStrError());
						bif.type = NWN_FILE_BIF;
						newbif = false;
					}
					if(resref.empty()) return printxmlerr(xml, "Missing resref");
					if(rfname.empty()) return printxmlerr(xml, "Missing filename");
					if(restype == NWN_FILE_UNDEFINED)
						return printxmlerr(xml, "Missing restype");
					if(exploded)
					{
						datas.resize((tmp=datas.size())+1);
						if(!(datas[tmp] = expd(xml, bif.getBaseType(restype), rfname, asize)))
							return false;
						if(bif.writeData(restype, withCd, true, datas[tmp], asize, false))
							return printxmlerr(xml, bif.getStrError());
					}
					else
						if(bif.writeData(restype, withCd, true, rfname))
							return printxmlerr(xml, bif.getStrError());
					if(key.addResource(key.bifcount-1, resref, restype, bif.vrescount-1))
						return printxmlerr(xml, key.getStrError());
					resref.clear();
					rfname.clear();
					restype = NWN_FILE_UNDEFINED;
					withCd = exploded = false;
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(!mem)
	{
		if(key.filename.empty()) return printxmlerr(xml, "Missing filename");
		else if(key.write(key.filename)) return printnwnerr(key);
	}
	else if(key.write(mem, size)) return printnwnerr(key);
	for(uint32 i=0;i<datas.size();i++) free(datas[i]);
	return true;
}

bool assembletlk(char *fname)
{
	return assembletlk(fname, NULL, NULL);
}

bool assembletlk(char *fname, uint8 **mem, uint32 *size)
{
	int pr;
	uint32 flags=0;
	uint64 tmp;
	float32 sndlength=0;
	std::string sndresref, str;
	XMLFile xml;
	TLKFile tlk;

	if(xml.open(fname))
	{
		std::cerr << "Error opening file \"" << fname << "\": ";
		std::cerr << xml.getStrError() << "\n";
		return false;
	}
	while((pr = xml.parse()) >= 0)
	{
		switch(pr)
		{
			case 0:
				if(!mem && (xml.section == ".tlk.filename"))
					tlk.filename = xml.value;
				else if(xml.section == ".tlk.filetype")
				{
					if((tlk.type = tlk.getFileResTypeByExt(xml.value)) == -1)
						return printxmlerr(xml, "Unknown filetype");
					if(!tlk.typeIsCorrect()) return printxmlerr(xml, "No TLK type");
				}
				else if(xml.section == ".tlk.language")
				{
					if((tlk.language = (NWNLang) getpos(langs, LANGS, xml.value)) == -1)
						return printxmlerr(xml, "Invalid language");
				}
				else if(xml.section == ".tlk.std::string.sndresref")
					sndresref = xml.value;
				else if(xml.section == ".tlk.std::string.flags")
				{
					sscanf(xml.value.c_str(), S_UINT64, &tmp);
					flags = (uint32) tmp;
				}
				else if(xml.section == ".tlk.std::string.sndlength")
					sscanf(xml.value.c_str(), S_FLOAT32, &sndlength);
				else if(xml.section == ".tlk.std::string")
					str = xml.value;
				break;
			case 1:
				if((xml.section != ".?xml") && (xml.section != ".tlk") &&
						(getpos(validtlktags, TLKTAGS, xml.section) == -1))
					return printxmlerr(xml, "Invalid tag");
				break;
			case 2:
				if(xml.value == ".tlk.std::string")
				{
					if(tlk.add(flags, sndresref, sndlength, unquotestr(str)))
						return printxmlerr(xml, tlk.getStrError());
					sndlength = flags = 0;
					str.clear();
					sndresref.clear();
				}
				break;
		}
	}
	if(pr != -6) return printxmlerr(xml, xml.getStrError());

	xml.close();
	if(!mem)
	{
		if(tlk.filename.empty()) return printxmlerr(xml, "Missing filename");
		else if(tlk.write(tlk.filename)) return printnwnerr(tlk);
	}
	else if(tlk.write(mem, size)) return printnwnerr(tlk);
	return true;
}

// vim:tabstop=2
