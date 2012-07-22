/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
