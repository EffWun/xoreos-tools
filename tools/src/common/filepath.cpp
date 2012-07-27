/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/filepath.cpp
 *  Utility class for manipulating file paths.
 */

#include "common/filepath.h"

namespace Common {

UString FilePath::getExtension(const UString &p) {
	return UString(p.findLast('.'), p.end());
}

UString FilePath::changeExtension(const UString &p, const UString &ext) {
	return UString(p.begin(), p.findLast('.')) + ext;
}

UString FilePath::getFile(const UString &p) {
	UString file = p;

	UString::iterator slash = file.findLast('/');
	if (slash != file.end())
		file = UString(++slash, file.end());

	UString::iterator backslash = file.findLast('\\');
	if (backslash != file.end())
		file = UString(++backslash, file.end());

	return file;
}

} // End of namespace Common
