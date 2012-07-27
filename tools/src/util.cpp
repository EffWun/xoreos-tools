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

/** @file util.cpp
 *  General tool utility functions.
 */

#include "common/error.h"
#include "common/ustring.h"
#include "common/stream.h"
#include "common/file.h"

#include "util.h"

void dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName) {
	Common::DumpFile file;
	if (!file.open(fileName))
		throw Common::Exception(Common::kOpenError);

	file.writeStream(stream);
	file.flush();
	if (file.err())
		throw Common::Exception(Common::kWriteError);

	file.close();
}
