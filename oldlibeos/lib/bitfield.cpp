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
