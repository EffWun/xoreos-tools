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

BitField::BitField()
{
	field = 0;
}

BitField::BitField(const uint16 &x)
{
	field = x;
}

bool BitField::operator[](uint8 x)
{
	if(x > 15) return false;
	return ((field & (1<<x)) == (1<<x));
}

void BitField::set(uint8 x)
{
	if(x < 16) field |= 1<<x;
}

void BitField::unSet(uint8 x)
{
	if(x < 16) field &= ~(1<<x);
}

void BitField::toggle(uint8 x)
{
	if(x < 16) field ^= 1<<x;
}

void BitField::clear(void)
{
	field = 0;
}

bool BitField::isMax(uint8 x)
{
	return field == ((1<<(x+1))-1);
}

uint16 BitField::operator=(uint16 x)
{
	return field = x;
}

BitField::operator uint16()
{
	return field;
}

// vim:tabstop=2
