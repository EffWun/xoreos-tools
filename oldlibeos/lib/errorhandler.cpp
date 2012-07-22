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

ErrorHandler::ErrorHandler()
{
	errcode = 0;
}

std::string ErrorHandler::getStrError(int error)
{
	if(error == -1) error = errcode;

	switch(error)
	{
		case 0:
			return "No error";
		case 1:
			return "Can't open file";
		case 2:
			return "Nothing opened";
		case 3:
			return "Unexpected EOF";
		case 4:
			return "Can't allocate memory";
		case 5:
			return "Error writing";
		case 6:
			return "Not opened for reading";
		case 7:
			return "Not opened for writing";
		case 8:
			return "Error reading";
		case 9:
			return "Invalid argument";
		case 10:
			return "Version requierements not met";
		case 11:
			return "Type does not match requested type";
		case 12:
			return "Unsupported language";
		case 13:
			return "Out of range";
		case 14:
			return "Fixed ressources aren't supported";
		case 15:
			return "No top level struct";
		case 16:
			return "Label out of range";
		case 17:
			return "Struct out of range";
		case 18:
			return "Field out of range";
		case 19:
			return "Requested field not found: \"" + aderrstr + "\"";
		case 20:
			return "Expected field not found (\"" + aderrstr + "\"), file corrupt";
		case 21:
			return "Only one top level struct allowed";
		case 22:
			return "List out of range";
		case 23:
			return "Block overflow, data corrupt";
		case 24:
			return "Unknown format";
		case 25:
			return "Missing fields, data corrupt";
		case 26:
			return "Requested column not found";
		case 27:
			return "Row lengths mismatch, data corrupt";
		case 28:
			return "BIF out of range";
		case 29:
			return "Wrong method";
		case 30:
			return "Requested entry not found: \"" + aderrstr + "\"";
		case 31:
			return "All announced resources already written";
		case 32:
			return "Less resources than announced actually written";
		case 33:
			return "Mod_ID has invalid size";
		case 34:
			return "StartList entry out of range";
		case 35:
			return "Reply index out of range";
		case 36:
			return "Types mismatch, file corrupt";
		case 37:
			return "Lengths mismatch, data, corrupt";
		case 38:
			return "Can't find module.ifo, not a module";
		case 39:
			return "Can't find repute.fac, not a module";
		default:
			return "Unknown error";
	}
}

void ErrorHandler::lowerStr(std::string &str)
{
	for(uint32 i=0;i<str.length();i++)
		str[i] = tolower(str[i]);
}

// vim:tabstop=2
