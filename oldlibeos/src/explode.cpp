/* eos 0.0.1
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
#include <sys/stat.h>
#include <sys/types.h>
#include <eos.h>
#include "explode.h"
#include "common.h"

#include <cstdlib>

extern char *validgfftags[];
extern char *langs[];
extern std::string basedir;
extern int numdata;

// There are too many similarities here, need to find a way to clean up... :/

bool explodegff(char *fname)
{
	GFFFile gfffile;
	std::string sfname;

	if(!init(gfffile, fname)) return false;
	return explodegff(gfffile, ((std::string) getbasename(fname)));
}

bool explodegff(uint8 *gff, uint32 size, std::string fname)
{
	GFFFile gfffile;

	if(!init(gfffile, gff, size)) return false;
	gfffile.filename = fname;
	return explodegff(gfffile, fname);
}

bool explodegff(GFFFile &gff, std::string fname)
{
	std::ofstream exgff;
	std::vector<int> scts;
	std::string sfname;

	sfname = fname + ".xml";
	exgff.open(sfname.c_str());
	if(!exgff)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	exgff << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	exgff << "<GFF filename=\"" << fname << "\" filetype=\"";
	exgff << gff.getFileExtByResType(gff.type) << "\">" << std::endl;
	if(!checkwerr(exgff)) return false;
	scts.push_back(0);
	doindent(exgff, 2);
	if(!explodegffstruct(exgff, gff, gff.topstruct, "", 2, scts))
		return false;
	exgff << "</GFF>" << std::endl;
	if(!checkwerr(exgff)) return false;
	exgff.close();
	return true;
}

// Recursive Struct->XML transformation function
bool explodegffstruct(std::ofstream &f, GFFFile &gff, GFFFile::Struct &sct,
		std::string lbl, int indent, std::vector<int> scts)
{
	std::string sstr;
	std::stringstream dname;
	ExoLocString exolocstr;
	uint8 *data;
	uint32 size;
	std::ofstream fdata;
	GFFFile::Struct *nsct;
	std::vector<uint32> sctl;

	f << std::fixed << std::uppercase << "<struct ";
	if(!lbl.empty()) f << "label=\"" << lbl << "\" ";
	f << "id=\"" << sct.id << "\">\n";
	for(uint32 j=0;j<sct.fieldcount;j++)
	{
		doindent(f, indent+2);
		switch(sct.types[j])
		{
			case NWN_VAR_INT8:
				f << "<int8 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (int) ((int8) sct.values[j]) << "\"/>\n";
				break;
			case NWN_VAR_UINT8:
				f << "<uint8 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (int) ((uint8) sct.values[j]) << "\"/>\n";
				break;
			case NWN_VAR_INT16:
				f << "<int16 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (int16) sct.values[j] << "\"/>\n";
				break;
			case NWN_VAR_UINT16:
				f << "<uint16 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (uint16) sct.values[j] << "\"/>\n";
				break;
			case NWN_VAR_INT32:
				f << "<int32 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (int32) sct.values[j] << "\"/>\n";
				break;
			case NWN_VAR_UINT32:
				f << "<uint32 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (uint32) sct.values[j] << "\"/>\n";
				break;
			case NWN_VAR_INT64:
				f << "<int64 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (int64) sct.values[j] << "\"/>\n";
				break;
			case NWN_VAR_UINT64:
				f << "<uint64 label=\"" << *sct.labels[j] << "\" value=\"";
				f << (uint64) sct.values[j] << "\"/>\n";
				break;
			case NWN_VAR_FLOAT32:
				f << "<float label=\"" << *sct.labels[j] << "\" value=\"";
				f.precision(6);
				f << *((float32 *) &sct.values[j]) << "\"/>\n";
				break;
			case NWN_VAR_FLOAT64:
				f << "<double label=\"" << *sct.labels[j] << "\" value=\"";
				f.precision(10);
				f << *((float64 *) &sct.values[j]) << "\"/>\n";
				break;
			case NWN_VAR_RESREF:
				if(gff.getResRef(j, sct, sstr)) return printnwnerr(gff);
				f << "<resref label=\"" << *sct.labels[j] << "\" value=\"";
				f << sstr << "\"/>\n";
				sstr.clear();
				break;
			case NWN_VAR_EXOSTR:
				if(gff.getExoString(j, sct, sstr)) return printnwnerr(gff);
				f << "<exostr label=\"" << *sct.labels[j] << "\">";
				f << quotestr(sstr) << "</exostr>\n";
				sstr.clear();
				break;
			case NWN_VAR_EXOLOCSTR:
				if(gff.getExoLocString(j, sct, exolocstr)) return printnwnerr(gff);
				f << "<exolocstr label=\"" << *sct.labels[j] << "\" strref=\"";
				f << std::dec << exolocstr.stringref << "\">\n";
				for(int i=0;i<NWN_LANG_MAX;i++)
					if(!exolocstr.str[i].empty())
					{
						doindent(f, indent+4);
						f << "<substr language=\"" << langs[i] << "\">";
						f << quotestr(exolocstr.str[i]) << "</substr>\n";
						exolocstr.str[i].clear();
					}
				doindent(f, indent+2);
				f << "</exolocstr>\n";
				break;
			case NWN_VAR_VOID:
				if((data = gff.getRaw(j, sct, size)) == NULL)
					return printnwnerr(gff);
				dname.str("");
				dname << getbasename((char*) gff.filename.c_str()) << ".";
				dname.width(3); dname.fill('0'); dname << numdata++; dname.width(0);
				fdata.open(dname.str().c_str());
				if(!fdata)
					return printerr("Can't open file \"" + dname.str() + "\" for writing");
				fdata.write((char *)data, size);
				fdata.close();
				free(data);
				f << "<data label=\"" << *sct.labels[j] << "\" file=\"";
				f << dname.str() << "\"/>\n";
				break;
			case NWN_VAR_STRUCT:
				nsct = new GFFFile::Struct;
				if(!checkrecursion(sct.values[j], scts)) return false;
				scts.push_back(sct.values[j]);
				if(gff.getStruct(sct.values[j], *nsct)) return printnwnerr(gff);
				if(!explodegffstruct(f, gff, *nsct, *sct.labels[j], indent+2, scts))
					return false;
				if(!checkwerr(f)) return false;
				scts.pop_back();
				delete nsct;
				break;
			case NWN_VAR_LIST:
				f << "<list label=\"" << *sct.labels[j] << "\">\n";
				if(gff.getList(j, sct, sctl)) return printnwnerr(gff);
				for(uint32 i=0;i<sctl.size();i++)
				{
					GFFFile::Struct nsct;
					if(!checkrecursion(sctl[i], scts)) return false;
					scts.push_back(sctl[i]);
					if(gff.getStruct(sctl[i], nsct)) return printnwnerr(gff);
					doindent(f, indent+4);
					if(!explodegffstruct(f, gff, nsct, "", indent+4, scts)) return false;
					if(!checkwerr(f)) return false;
					scts.pop_back();
				}
				doindent(f, indent+2);
				f << "</list>\n";
				sctl.clear();
				break;
			default:
				f << "<TYPE=\"" << (int) sct.types[j] << "\" label=\"" << *sct.labels[j] << "\"/>\n";
				break;
		}
	}
	doindent(f, indent);
	f << "</struct>\n";
	return true;
}

bool explodeerf(char *fname)
{
	ERFFile erffile;
	std::string sfname;

	if(!init(erffile, fname)) return false;
	return explodeerf(erffile, ((std::string) getbasename(fname)));
}

bool explodeerf(uint8 *erf, uint32 size, std::string fname, std::string base)
{
	ERFFile erffile;

	if(!init(erffile, erf, size)) return false;
	erffile.filename = fname;
	return explodeerf(erffile, fname, base);
}

bool explodeerf(ERFFile &erf, std::string fname, std::string base)
{
	std::string sfname, rfname;
	std::ofstream exerf;
	const char *ext;
	uint8 *data;

	sfname = fname + ".xml";
	exerf.open(sfname.c_str());
	if(!exerf)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	exerf << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	exerf << "<ERF filename=\"" << fname << "\" filetype=\"";
	exerf << erf.getFileExtByResType(erf.type) << "\" buildyear=\"";
	exerf << erf.buildyear+1900 << "\" buildday=\"" << erf.buildday;
	exerf << "\">" << std::endl;
	if(!checkwerr(exerf)) return false;
	exerf << "  <description strref=\"" << std::dec << erf.desc.stringref << "\">\n";
	for(int i=0;i<NWN_LANG_MAX;i++)
		if(!erf.desc.str[i].empty())
		{
			std::string str = erf.desc.str[i];
			exerf << "    <substr language=\"" << langs[i] << "\">";
			exerf << quotestr(str) << "</substr>\n";
		}
	exerf << "  </description>" << std::endl;
	if(!checkwerr(exerf)) return false;
	for(uint32 i=0;i<erf.entrycount;i++)
	{
		if(!(i % 20)) std::cout << i << "/" << erf.entrycount << std::endl;
		if(!(ext = erf.getFileExtByResType(erf.restypes[i])))
			return printerr("ResType Unknown");
		rfname = base + erf.resrefs[i] + "." + ext;
		exerf << "  <resource resref=\"" << erf.resrefs[i];
		exerf << "\" restype=\"" << ext << "\" exploded=\"";
		switch(erf.getBaseType(erf.restypes[i]))
		{
			case NWN_BASE_ERF:
				if(!(data = erf.dumpEntryIntoMem(i))) return printnwnerr(erf);
				if(!explodeerf(data, erf.sizes[i], rfname, rfname + "."))
					printwarn("While exploding \"" + rfname + "\"");
				exerf << "1\" filename=\"" << rfname << ".xml\"/>\n";
				break;
			case NWN_BASE_BIF:
				if(!(data = erf.dumpEntryIntoMem(i))) return printnwnerr(erf);
				if(!explodebif(data, erf.sizes[i], rfname, rfname + "."))
					printwarn("While exploding \"" + rfname + "\"");
				exerf << "1\" filename=\"" << rfname << ".xml\"/>\n";
				break;
			case NWN_BASE_GFF:
				if(!(data = erf.dumpEntryIntoMem(i))) return printnwnerr(erf);
				if(!explodegff(data, erf.sizes[i], rfname))
					printwarn("While exploding \"" + rfname + "\"");
				exerf << "1\" filename=\"" << rfname << ".xml\"/>\n";
				break;
			case NWN_BASE_KEY:
				if(!(data = erf.dumpEntryIntoMem(i))) return printnwnerr(erf);
				if(!explodekey(data, erf.sizes[i], rfname))
					printwarn("While exploding \"" + rfname + "\"");
				exerf << "1\" filename=\"" << rfname << ".xml\"/>\n";
				break;
			case NWN_BASE_SSF:
				if(!(data = erf.dumpEntryIntoMem(i))) return printnwnerr(erf);
				if(!explodessf(data, erf.sizes[i], rfname))
					printwarn("While exploding \"" + rfname + "\"");
				exerf << "1\" filename=\"" << rfname << ".xml\"/>\n";
				break;
			case NWN_BASE_TLK:
				if(!(data = erf.dumpEntryIntoMem(i))) return printnwnerr(erf);
				if(!explodetlk(data, erf.sizes[i], rfname))
					printwarn("While exploding \"" + rfname + "\"");
				exerf << "1\" filename=\"" << rfname << ".xml\"/>\n";
				break;
			default:
				if(erf.dumpEntryIntoFile(i, rfname))
					printwarn("While extracting \"" + rfname + "\"");
				exerf << "0\" filename=\"" << rfname << "\"/>\n";
				break;
		}
	}
	exerf << "</ERF>" << std::endl;
	if(!checkwerr(exerf)) return false;
	exerf.close();
	return true;
}

bool explodebif(char *fname)
{
	BIFFile biffile;
	std::string sfname;

	if(!init(biffile, fname)) return false;
	return explodebif(biffile, ((std::string) getbasename(fname)));
}

bool explodebif(uint8 *bif, uint32 size, std::string fname, std::string base)
{
	BIFFile biffile;

	if(!init(biffile, bif, size)) return false;
	biffile.filename = fname;
	return explodebif(biffile, fname, base);
}

bool explodebif(BIFFile &bif, std::string fname, std::string base)
{
	std::string sfname;
	std::stringstream rfname;
	std::ofstream exbif;
	const char *ext;
	uint8 *data;

	sfname = fname + ".xml";
	exbif.open(sfname.c_str());
	if(!exbif)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	exbif << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	exbif << "<BIF filename=\"" << fname << "\" filetype=\"";
	exbif << bif.getFileExtByResType(bif.type) << "\">" << std::endl;
	exbif << "  <variable>" << std::endl;
	if(!checkwerr(exbif)) return false;
	for(uint32 i=0;i<bif.vrescount;i++)
	{
		if(!(i % 20)) std::cout << i << "/" << bif.vrescount << std::endl;
		if(!(ext = bif.getFileExtByResType(bif.restypes[i])))
			return printerr("ResType Unknown");
		rfname.str("");
		rfname << base;
		rfname.width(8); rfname.fill('0'); rfname << i;
		rfname.width(0); rfname << "." << ext;
		exbif << "    <resource restype=\"" << ext << "\" withCd=\"";
		exbif << bif.withCd(bif.resids[i]) << "\" exploded=\"";
		switch(bif.getBaseType(bif.restypes[i]))
		{
			case NWN_BASE_ERF:
				if(!(data = bif.dumpEntryIntoMem(i))) return printnwnerr(bif);
				if(!explodeerf(data, bif.fsizes[i], rfname.str(), rfname.str() + "."))
					printwarn("While exploding \"" + rfname.str() + "\"");
				exbif << "1\" filename=\"" << rfname.str() << ".xml\"/>\n";
				break;
			case NWN_BASE_BIF:
				if(!(data = bif.dumpEntryIntoMem(i))) return printnwnerr(bif);
				if(!explodebif(data, bif.fsizes[i], rfname.str(), rfname.str() + "."))
					printwarn("While exploding \"" + rfname.str() + "\"");
				exbif << "1\" filename=\"" << rfname.str() << ".xml\"/>\n";
				break;
			case NWN_BASE_GFF:
				if(!(data = bif.dumpEntryIntoMem(i))) return printnwnerr(bif);
				if(!explodegff(data, bif.fsizes[i], rfname.str()))
					printwarn("While exploding \"" + rfname.str() + "\"");
				exbif << "1\" filename=\"" << rfname.str() << ".xml\"/>\n";
				break;
			case NWN_BASE_KEY:
				if(!(data = bif.dumpEntryIntoMem(i))) return printnwnerr(bif);
				if(!explodekey(data, bif.fsizes[i], rfname.str()))
					printwarn("While exploding \"" + rfname.str() + "\"");
				exbif << "1\" filename=\"" << rfname.str() << ".xml\"/>\n";
				break;
			case NWN_BASE_SSF:
				if(!(data = bif.dumpEntryIntoMem(i))) return printnwnerr(bif);
				if(!explodessf(data, bif.fsizes[i], rfname.str()))
					printwarn("While exploding \"" + rfname.str() + "\"");
				exbif << "1\" filename=\"" << rfname.str() << ".xml\"/>\n";
				break;
			case NWN_BASE_TLK:
				if(!(data = bif.dumpEntryIntoMem(i))) return printnwnerr(bif);
				if(!explodetlk(data, bif.fsizes[i], rfname.str()))
					printwarn("While exploding \"" + rfname.str() + "\"");
				exbif << "1\" filename=\"" << rfname.str() << ".xml\"/>\n";
				break;
			default:
				if(bif.dumpEntryIntoFile(i, rfname.str()))
					printwarn("While extracting \"" + rfname.str() + "\"");
				exbif << "0\" filename=\"" << rfname.str() << "\"/>\n";
				break;
		}
	}
	exbif << "  </variable>\n</BIF>" << std::endl;
	if(!checkwerr(exbif)) return false;
	exbif.close();
	return true;
}

bool explodekey(char *fname)
{
	KEYFile keyfile;
	std::string sfname;

	if(!init(keyfile, fname)) return false;
	return explodekey(keyfile, ((std::string) getbasename(fname)));
}

bool explodekey(uint8 *key, uint32 size, std::string fname)
{
	KEYFile keyfile;

	if(!init(keyfile, key, size)) return false;
	keyfile.filename = fname;
	return explodekey(keyfile, fname);
}

bool explodekey(KEYFile &key, std::string fname)
{
	std::ofstream exkey;
	std::string sfname, resref;
	NWNFileType restype;
	uint32 index;
	const char *ext;

	sfname = fname + ".xml";
	exkey.open(sfname.c_str());
	if(!exkey)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	exkey << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	exkey << "<KEY filename=\"" << fname << "\" filetype=\"";
	exkey << key.getFileExtByResType(key.type) << "\" buildyear=\"";
	exkey << key.buildyear+1900 << "\" buildday=\"" << key.buildday;
	exkey << "\">" << std::endl;
	if(!checkwerr(exkey)) return false;

	for(uint32 i=0;i<key.bifcount;i++)
	{
		exkey << "  <bif filename=\"" << key.fnames[i] << "\" filesize=\"";
		exkey << key.fsizes[i] << "\" drives=\"" << key.drives[i] << "\">\n";
		for(uint32 j=0;j<key.getNumResources(i);j++)
		{
			if(key.getKeyEntry(i, j, resref, restype, index)) return printnwnerr(key);
			if(!(ext = key.getFileExtByResType(restype)))
			{
				std::cerr << (uint32) restype << " - " << resref << std::endl;
				return printerr("ResType Unknown");
			}
			exkey << "    <resource resref=\"" << resref << "\" restype=\"";
			exkey << ext << "\" index=\"" << index << "\"/>\n";
		}
		exkey << "  </bif>\n";
	}

	exkey << "</KEY>" << std::endl;
	if(!checkwerr(exkey)) return false;
	exkey.close();
	return true;
}

bool explodessf(char *fname)
{
	SSFFile ssffile;
	std::string sfname;

	if(!init(ssffile, fname)) return false;
	return explodessf(ssffile, ((std::string) getbasename(fname)));
}

bool explodessf(uint8 *ssf, uint32 size, std::string fname)
{
	SSFFile ssffile;

	if(!init(ssffile, ssf, size)) return false;
	ssffile.filename = fname;
	return explodessf(ssffile, fname);
}

bool explodessf(SSFFile &ssf, std::string fname)
{
	std::ofstream exssf;
	std::string sfname;

	sfname = fname + ".xml";
	exssf.open(sfname.c_str());
	if(!exssf)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	exssf << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	exssf << "<SSF filename=\"" << fname << "\" filetype=\"";
	exssf << ssf.getFileExtByResType(ssf.type) << "\">" << std::endl;
	if(!checkwerr(exssf)) return false;
	for(uint32 i=0;i<ssf.entrycount;i++)
	{
		exssf << "  <sound resref=\"" << ssf.resrefs[i] << "\" stringref=\"";
		exssf << ssf.stringrefs[i] << "\"/>\n";
	}
	exssf << "</SSF>" << std::endl;
	if(!checkwerr(exssf)) return false;
	exssf.close();
	return true;
}

bool explodekeybif(char *fname)
{
	KEYFile keyfile;
	std::string sfname;

	if(!init(keyfile, fname)) return false;
	return explodekeybif(keyfile, ((std::string) getbasename(fname)));
}

bool explodekeybif(uint8 *key, uint32 size, std::string fname, std::string base)
{
	KEYFile keyfile;

	if(!init(keyfile, key, size)) return false;
	keyfile.filename = fname;
	return explodekeybif(keyfile, fname, base);
}

bool explodekeybif(KEYFile &key, std::string fname, std::string base)
{
	std::ofstream exkey;
	std::string sfname, bfname, resref, rfname;
	NWNFileType restype;
	uint8 *data;
	uint32 index;
	const char *ext;

	sfname = fname + ".xml";
	exkey.open(sfname.c_str());
	if(!exkey)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	exkey << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	exkey << "<KEY filename=\"" << fname << "\" filetype=\"";
	exkey << key.getFileExtByResType(key.type) << "\" buildyear=\"";
	exkey << key.buildyear+1900 << "\" buildday=\"" << key.buildday;
	exkey << "\">" << std::endl;
	if(!checkwerr(exkey)) return false;

	for(uint32 i=0;i<key.bifcount;i++)
	{
		BIFFile bif;
		if(!basedir.empty()) bfname = basedir;
		bfname += key.fnames[i];
		if(!init(bif, (char*) bfname.c_str(), true)) return false;
		exkey << "  <bif filename=\"" << key.fnames[i] << "\" drives=\"";
		exkey << key.drives[i] << "\">\n";
		exkey << "    <variable>" << std::endl;
		for(uint32 j=0;j<key.getNumResources(i);j++)
		{
			if(key.getKeyEntry(i, j, resref, restype, index)) return printnwnerr(key);
			if(!(ext = bif.getFileExtByResType(bif.restypes[j])))
			{
				std::cerr << (uint32) restype << " - " << resref << std::endl;
				return printerr("ResType Unknown");
			}
			rfname = base + getbasename((char*) bfname.c_str());
			rfname += "." + resref + "." + ext;
			exkey << "      <resource resref=\"" << resref << "\" restype=\"";
			exkey << ext << "\" withCd=\"" << bif.withCd(bif.resids[j]);
			exkey << "\" exploded=\"";
			switch(bif.getBaseType(bif.restypes[j]))
			{
				case NWN_BASE_ERF:
					if(!(data = bif.dumpEntryIntoMem(j))) return printnwnerr(bif);
					if(!explodeerf(data, bif.fsizes[j], rfname, rfname + "."))
						printwarn("While exploding \"" + rfname + "\"");
					exkey << "1\" filename=\"" << rfname << ".xml\"/>\n";
					break;
				case NWN_BASE_BIF:
					if(!(data = bif.dumpEntryIntoMem(j))) return printnwnerr(bif);
					if(!explodebif(data, bif.fsizes[j], rfname, rfname + "."))
						printwarn("While exploding \"" + rfname + "\"");
					exkey << "1\" filename=\"" << rfname << ".xml\"/>\n";
					break;
				case NWN_BASE_GFF:
					if(!(data = bif.dumpEntryIntoMem(j))) return printnwnerr(bif);
					if(!explodegff(data, bif.fsizes[j], rfname))
						printwarn("While exploding \"" + rfname + "\"");
					exkey << "1\" filename=\"" << rfname << ".xml\"/>\n";
					break;
				case NWN_BASE_KEY:
					if(!(data = bif.dumpEntryIntoMem(j))) return printnwnerr(bif);
					if(!explodekey(data, bif.fsizes[j], rfname))
						printwarn("While exploding \"" + rfname + "\"");
					exkey << "1\" filename=\"" << rfname << ".xml\"/>\n";
					break;
				case NWN_BASE_SSF:
					if(!(data = bif.dumpEntryIntoMem(j))) return printnwnerr(bif);
					if(!explodessf(data, bif.fsizes[j], rfname))
						printwarn("While exploding \"" + rfname + "\"");
					exkey << "1\" filename=\"" << rfname << ".xml\"/>\n";
					break;
				case NWN_BASE_TLK:
					if(!(data = bif.dumpEntryIntoMem(j))) return printnwnerr(bif);
					if(!explodetlk(data, bif.fsizes[j], rfname))
						printwarn("While exploding \"" + rfname + "\"");
					exkey << "1\" filename=\"" << rfname << ".xml\"/>\n";
					break;
				default:
					if(bif.dumpEntryIntoFile(j, rfname))
						printwarn("While extracting \"" + rfname + "\"");
					exkey << "0\" filename=\"" << rfname << "\"/>\n";
			}
		}
		bif.close();
		exkey << "    </variable>\n  </bif>\n";
	}

	exkey << "</KEY>" << std::endl;
	if(!checkwerr(exkey)) return false;
	exkey.close();
	return true;
}

bool explodetlk(char *fname)
{
	TLKFile tlkfile;
	std::string sfname;

	if(!init(tlkfile, fname)) return false;
	return explodetlk(tlkfile, ((std::string) getbasename(fname)));
}

bool explodetlk(uint8 *tlk, uint32 size, std::string fname)
{
	TLKFile tlkfile;

	if(!init(tlkfile, tlk, size)) return false;
	tlkfile.filename = fname;
	return explodetlk(tlkfile, fname);
}

bool explodetlk(TLKFile &tlk, std::string fname)
{
	std::ofstream extlk;
	std::string sfname;

	sfname = fname + ".xml";
	extlk.open(sfname.c_str());
	if(!extlk)
	{
		std::cerr << "Can't open file \"" << sfname << "\" for writing\n";
		return false;
	}
	if(((uint32) tlk.language) >= LANGS)
		return printerr("Invalid language");
	extlk << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	extlk << "<TLK filename=\"" << fname << "\" filetype=\"";
	extlk << tlk.getFileExtByResType(tlk.type) << "\" language=\"";
	extlk << langs[(int) tlk.language] << "\">" << std::endl;
	if(!checkwerr(extlk)) return false;
	for(uint32 i=0;i<tlk.strcount;i++)
	{
		extlk << "  <std::string flags=\"" << (uint16) tlk.flags[i] << "\" sndresref=\"";
		extlk << tlk.sndresrefs[i] << "\" sndlength=\"" << tlk.sndlengths[i];
		extlk << "\">" << quotestr(tlk.strings[i]) << "</std::string>\n";
	}
	extlk << "</TLK>" << std::endl;
	if(!checkwerr(extlk)) return false;
	extlk.close();
	return true;
}

// vim:tabstop=2
