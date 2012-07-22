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
#include "xmlstuff.h"

static int xmlerr;

XMLFile::XMLFile(void)
{
	opened = eof = false;
	tagstate = tagstart = line = linepos = 0;
}

int XMLFile::open(std::string fname)
{
	if(opened) return xmlerr = -1;
	filename = fname;
	xmlfile.open(filename.c_str());
	if(!xmlfile) return xmlerr = -2;
	opened = true;
	return xmlerr = 0;
}

int XMLFile::close(void)
{
	if(!opened) return xmlerr = -3;
	xmlfile.close();
	opened = false;
	return xmlerr = 0;
}

int XMLFile::clear(void)
{
	if(!opened) return xmlerr = -3;
	xmlfile.clear();
	return xmlerr = 0;
}

int XMLFile::reset(void)
{
	xmlfile.seekg(0, std::ios::beg);
	if(xmlfile.fail())
	{
		xmlfile.clear();
		return xmlerr = -5;
	}
	section.erase();
	eof = false;
	line = linepos = 0;
	return xmlerr = 0;
}

int XMLFile::tell(void)
{
	if(!opened) return xmlerr = -3;
	return xmlfile.tellg();
}

int XMLFile::seek(size_t off, std::ios_base::seekdir dir)
{
	int pos = xmlfile.tellg();
	xmlfile.seekg(off, dir);
	if(xmlfile.fail())
	{
		xmlfile.seekg(pos, std::ios::beg);
		xmlfile.clear();
		return xmlerr = -4;
	}
	return xmlerr = 0;
}

int XMLFile::parse(void)
{
	if(!opened) return xmlerr = -3;

	char xchar;
	bool secend=true;

	while(!eof)
	{
		if(tagstate == 5)
		{
			if(buffer.empty()) value = section;
			int p = section.rfind(".");
			if(p != (int)std::string::npos) section.erase(p, section.size());
			if(buffer.empty()) return xmlerr = (tagstate = 0)+2;
		}
		if(!buffer.empty() && (tagstate != 5)) tagstate = 4;
		else if(buffer.empty()) tagstate = 0;
		while(tagstate < 2)
		{
			xmlfile.get(xchar);
			linepos++;
			if((eof = xmlfile.eof())) return xmlerr = -6;
			else if(xmlfile.fail()) return xmlerr = -7;
			if(xchar == '\n') // TODO: Other line-endings [high]
			{
				linepos = 0;
				line++;
			}
			if((xchar == '\n') || (xchar == '\r') || (xchar == '\t') || !xchar)
			{
				secend = true;
				continue;
			}
			if((xchar == '<') && !tagstate && buffer.empty())
			{
				tagstate = 1;
				tagstart = xmlfile.tellg();
			}
			else if((xchar == '<') && !tagstate && !buffer.empty() && secend)
			{
				xmlfile.putback(xchar);
				linepos--;
				tagstate = 3;
			}
			else if((xchar == '>') && (tagstate == 1)) tagstate = 2;
			else buffer += xchar;
			if(xchar == '\\') secend = !secend; else secend = true;
		}
		if(tagstate == 2)
		{
			bool addsec = true;
			if(buffer[0] == '/') buffer.erase((addsec = false), 1);
			size_t end = buffer.find(" ");
			if(end == std::string::npos) end = buffer.size();
			std::string secstr(buffer, 0, end);
			buffer.erase(0, end+1);
			if(addsec)
			{
				addsection(secstr);
				value = section;
				return xmlerr = 1;
			}
			else
			{
				value = section;
				delsection(secstr);
				return xmlerr = 2;
			}
		}
		else if(tagstate == 3)
		{
			value = buffer;
			buffer.erase();
			return xmlerr = 0;
		}
		else if((tagstate == 4) || (tagstate == 5))
		{
			unsigned int i;
			for(i=0;i<buffer.size();i++) if(buffer[i] != ' ') break;
			if(i) buffer.erase(0, i);
			size_t s = buffer.find("=\""), e=0;
			if(s != std::string::npos) e = getnextunquoted(buffer, s+2, "\"");
			tagstate = 5;
			if(buffer[0] == '/') buffer.erase();
			else if((buffer[0] == '?') && (buffer.size() == 1))
			{
				int p = section.rfind(".?");
				if(p != (int)std::string::npos) section.erase(p, section.size());
				buffer.erase();
				tagstate = 0;
			}
			else if((s == std::string::npos) || (e == std::string::npos))
			{
				tagstate = 0;
				buffer.erase();
			}
			else
			{
				addsection(buffer.substr(0, s));
				value = buffer.substr(s+2, e-s-2);
				replace(value, "\\\"", "\"");
				buffer.erase(0, e+1);
				return xmlerr = 0;
			}
		}
		else buffer.erase(); // Shouldn't be reached
	}
	return xmlerr = -6;
}

std::string &XMLFile::lowerstring(std::string &str)
{
	for(unsigned int i=0;i<str.length();i++) str[i] = tolower(str[i]);
	return str;
}

void XMLFile::addsection(std::string sec)
{
	section += "." + lowerstring(sec);
}

void XMLFile::delsection(std::string sec)
{
	size_t f = section.rfind(lowerstring(sec));
	if(f != std::string::npos) section.erase(f-1, section.size());
}

size_t XMLFile::getnextunquoted(std::string str, unsigned int off, std::string q)
{
	size_t e1=0, e2=0;
	bool found = false;

	while(!found && (off < str.size()))
	{
		e1 = str.find(q, off);
		e2 = str.find("\\" + q, off);
		if(e2 == (e1-1)) off = e1+1;
		else if((e2 > (e1-1)) || (e2 == std::string::npos)) found = true;
	}
	return xmlerr = e1;
}

void XMLFile::replace(std::string &haystack, std::string needle, std::string replacer)
{
	size_t pos=0;
	while((pos = haystack.find(needle, pos)) != std::string::npos)
	{
		haystack.replace(pos, needle.length(), replacer);
		pos += replacer.length() - needle.length() + 1;
	}
}

std::string XMLFile::getStrError(int error)
{
	if(xmlerr > 0) xmlerr = 0;
	if(error == -1) error = xmlerr;
	switch(error)
	{
		case 0: return "No error";
		case -1: return "Already opened";
		case -2: return "Can't open file";
		case -3: return "No file opened";
		case -4: return "Could not seek";
		case -5: return "Could not reset";
		case -6: return "End of file reached";
		case -7: return "Read failed";
		default: return "Unknown error";
	}
}

// vim:tabstop=2
