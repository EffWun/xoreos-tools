/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
