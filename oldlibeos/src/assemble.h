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

#ifndef _ASSEMBLE_H_
#define _ASSEMBLE_H_

#include "xmlstuff.h"

bool assemblegff(char *fname);
bool assemblegff(char *fname, uint8 **mem, uint32 *size);
bool assemblegffstruct(XMLFile &xml, GFFFile &gff, uint32 n, std::string *slbl);
bool assemblegffexolocstr(XMLFile &xml, GFFFile &gff, uint32 n);
bool assemblegfflist(XMLFile &xml, GFFFile &gff, uint32 n, std::string *llbl);
bool assembleerf(char *fname);
bool assembleerf(char *fname, uint8 **mem, uint32 *size);
bool assembleerfdesc(XMLFile &xml, ERFFile &erf);
bool assemblekey(char *fname);
bool assemblekey(char *fname, uint8 **mem, uint32 *size);
bool assemblebif(char *fname);
bool assemblebif(char *fname, uint8 **mem, uint32 *size);
bool assemblessf(char *fname);
bool assemblessf(char *fname, uint8 **mem, uint32 *size);
bool assemblekeybif(char *fname);
bool assemblekeybif(char *fname, uint8 **mem, uint32 *size);
bool assembletlk(char *fname);
bool assembletlk(char *fname, uint8 **mem, uint32 *size);

#endif // _ASSEMBLE_H_

// vim:tabstop=2
