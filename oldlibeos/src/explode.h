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

#ifndef _EXPLODE_H_
#define _EXPLODE_H_

bool explodegff(char *fname);
bool explodegff(uint8 *gff, uint32 size, std::string fname);
bool explodegff(GFFFile &gff, std::string fname);
bool explodegffstruct(std::ofstream &f, GFFFile &gff, GFFFile::Struct &sct,
		std::string lbl, int indent, std::vector<int> scts);
bool explodeerf(char *fname);
bool explodeerf(uint8 *erf, uint32 size, std::string fname, std::string base="");
bool explodeerf(ERFFile &erf, std::string fname, std::string base="");
bool explodebif(char *fname);
bool explodebif(uint8 *bif, uint32 size, std::string fname, std::string base="");
bool explodebif(BIFFile &bif, std::string fname, std::string base="");
bool explodekey(char *fname);
bool explodekey(uint8 *key, uint32 size, std::string fname);
bool explodekey(KEYFile &key, std::string fname);
bool explodessf(char *fname);
bool explodessf(uint8 *ssf, uint32 size, std::string fname);
bool explodessf(SSFFile &ssf, std::string fname);
bool explodekeybif(char *fname);
bool explodekeybif(uint8 *key, uint32 size, std::string fname, std::string base="");
bool explodekeybif(KEYFile &key, std::string fname, std::string base="");
bool explodetlk(char *fname);
bool explodetlk(uint8 *tlk, uint32 size, std::string fname);
bool explodetlk(TLKFile &tlk, std::string fname);

#endif // _EXPLODE_H_

// vim:tabstop=2
