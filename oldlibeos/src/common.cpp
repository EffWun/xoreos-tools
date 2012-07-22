/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <eos.h>
#include "common.h"

// All valid GFF XML tags, first 16 also all valid GFF VarTypes
const char *validgfftags[] = {".int8", ".uint8", ".uint16", ".int16", ".uint32",
	".int32", ".uint64", ".int64", ".float", ".double", ".exostr", ".resref",
	".exolocstr", ".data", ".struct", ".list", ".substr"};
const char *validerftags[] = {".description", ".substr", ".resource"};
const char *validkeytags[] = {".bif", ".resource"};
const char *validbiftags[] = {".variable", ".resource"};
const char *validssftags[] = {".sound"};
const char *validkeybiftags[] = {".bif", ".variable", ".resource"};
const char *validtlktags[] = {".std::string"};
const char *langs[] = { "EnglishMale", "EnglishFemale", "FrenchMale", "FrenchFemale",
	"GermanMale", "GermanFemale", "ItalianMale", "ItalianFemale", "SpanishMale",
	"SpanishFemale", "PolishMale", "PolishFemale" };
std::string basedir; // The directory NWN is installed
int numdata=0;

bool printerr(std::string error)
{
	std::cerr << "Error: " << error << "\n";
	return false;
}

bool printwarn(std::string error)
{
	std::cerr << "Warning: " << error << "\n";
	return false;
}

bool printnwnerr(NWNFile &nwn)
{
	return printerr(nwn.getStrError());
	return false;
}

bool init(NWNFile &nwn, const char *fname, bool silent)
{
	if(nwn.open(fname))
	{
		std::cerr << "Can't open file \"" << fname << "\": " << nwn.getStrError() << "\n";
		return false;
	}

	if(!silent)
	{
		std::cout << "File \"" << fname << "\" opened\n";
		std::cout << "Format: " << nwn.type << " Version: " << nwn.version << "\n";
	}

	if(!nwn.typeIsCorrect())
		return printerr("Invalid file format");

	if(nwn.init()) return printnwnerr(nwn);
	return true;
}

bool init(NWNFile &nwn, uint8 *mem, uint32 size)
{
	if(nwn.open(mem, size))
	{
		std::cerr << "Can't open: " << nwn.getStrError() << "\n";
		return false;
	}

	if(!nwn.typeIsCorrect())
		return printerr("Invalid file format");

	if(nwn.init()) return printnwnerr(nwn);
	return true;
}

bool deInit(NWNFile &nwn)
{
	nwn.close();
	return true;
}

// In a GFF, recursion is possible, in the XML format, however, not.
// Since it wouldn't make sense in a GFF anyway, that's no problem,
// but it has to be checked...
bool checkrecursion(int sct, std::vector<int> &scts)
{
	for(uint32 i=0;i<scts.size();i++)
		if(sct == scts[i]) return printerr("Recursion detected");
	return true;
}

void doindent(std::ofstream &f, int indent)
{
	for(int i=0;i<indent;i++) f << " ";
}

void replace(std::string &haystack, std::string needle, std::string replacer)
{
	int pos=0;
	while((pos = haystack.find(needle, pos)) != (int)std::string::npos)
	{
		haystack.replace(pos, needle.length(), replacer);
		pos += replacer.length() - needle.length() + 1;
	}
}

std::string quotestr(std::string str)
{
	std::string rstr;
	for(unsigned int i=0;i<str.length();i++)
		switch(str[i])
		{
			case '\\':
				rstr += "\\\\";
				break;
			case '\n':
				rstr += "\\n";
				break;
			case '\r':
				rstr += "\\r";
				break;
			case '<':
				rstr += "\\<";
				break;
			case '>':
				rstr += "\\>";
				break;
			default:
				rstr += str[i];
				break;
		}
	return rstr;
}

std::string unquotestr(std::string str)
{
	std::string rstr;
	bool bsp=false;

	for(unsigned int i=0;i<str.length();i++)
		if(bsp)
		{
			switch(str[i])
			{
				case 'n':
					rstr += '\n';
					break;
				case 'r':
					rstr += '\r';
					break;
				default:
					rstr += str[i];
					break;
			}
			bsp = false;
		}
		else
			if(str[i] == '\\') bsp = true;
			else rstr += str[i];
	return rstr;
}

std::string rsubstr(std::string str, int n)
{
	int s = str.size();
	if(s < n) return "";
	return str.substr(s-n, n);
}

void printxmlpos(XMLFile &xml)
{
	std::cerr << xml.filename << ":" << xml.line+1 << ":" << xml.linepos+1 << ": ";
}

bool printxmlerr(XMLFile &xml, std::string error)
{
	printxmlpos(xml);
	return printerr(error);
}

bool checkwerr(std::ofstream &f)
{
	if(f.fail() || f.bad()) return printerr("Write error");
	return true;
}

int getpos(char *haystack[], int haystacksize, std::string needle, int hayoff)
{
	for(int i=0;i<haystacksize;i++)
		if(rsubstr(needle, strlen(haystack[i])) == (haystack[i]+hayoff))
			return i;
	return -1;
}

char *getbasename(char *filename)
{
#ifndef HAVE_BASENAME
	if(!filename) return NULL;
	for(char *f=filename;*f;++f)
		if((*f == '/') || (*f == '\\') || (*f == ':'))
			filename = f+1;
	return filename;
#else
	return basename(filename);
#endif
}

// vim:tabstop=2
