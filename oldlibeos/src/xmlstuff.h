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

#ifndef _XMLSTUFF_H_
#define _XMLSTUFF_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class XMLFile
{
	public:
		std::string filename, section, value;
		int line, linepos;
		size_t tagstart;
		bool opened, eof;

		XMLFile(void);
		int open(std::string fname);
		int close(void);
		int clear(void);
		int reset(void);
		int tell(void);
		int seek(size_t off, std::ios_base::seekdir dir);
		std::string getStrError(int error=-1);

		// return:
		// < 0: Error
		//   0: Found value
		//   1: Entered a section
		//   2: Left a section
		int parse(void);

	protected:
		std::ifstream xmlfile;
		std::string buffer;
		char tagstate;

		std::string &lowerstring(std::string &str);
		void addsection(std::string sec);
		void delsection(std::string sec);
		size_t getnextunquoted(std::string str, unsigned int off, std::string q);
		void replace(std::string &haystack, std::string needle, std::string replacer);
};

#endif // _XMLSTUFF_H_

// vim:tabstop=2
