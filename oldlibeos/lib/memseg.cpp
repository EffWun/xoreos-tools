/* libeos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <eos.h>

#include <cstdlib>

MemSeg::MemSeg()
{
	mem = NULL;
	size = 0;
}

MemSeg::~MemSeg()
{
//	fprintf(stderr, "%p (%d): ", mem, size);
//	printmemdata("vor");
	if(mem) free(mem);
//	fprintf(stderr, "%p: ", mem);
//	printmemdata("nach");
}

MemSeg::MemSeg(const MemSeg &src)
{
	*this = src;
}

MemSeg &MemSeg::operator=(const MemSeg &src)
{
	size = src.size;
	mem = NULL;
	if(src.mem)
	{
		mem = (uint8 *) malloc(size);
		memcpy(mem, src.mem, size);
	}
	return *this;
}

int MemSeg::alloc(int size)
{
	if(!(mem = (uint8 *) malloc(size))) return 0;
	return this->size = size;
};

void MemSeg::deAlloc(void)
{
	if(mem) free(mem);
	mem = NULL;
	size = 0;
}

// vim:tabstop=2
