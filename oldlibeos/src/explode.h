/* eos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
