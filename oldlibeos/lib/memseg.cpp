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
