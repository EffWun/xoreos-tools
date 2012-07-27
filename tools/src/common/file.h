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

/** @file common/file.h
 *  File classes implementing the stream interfaces.
 */

#ifndef COMMON_FILE_H
#define COMMON_FILE_H

#include <cstdio>

#include "common/types.h"
#include "common/stream.h"
#include "common/noncopyable.h"

namespace Common {

class UString;

/** A simple streaming file class. */
class File : public SeekableReadStream, public NonCopyable {
public:
	File();
	File(const UString &fileName);
	~File();

	/**
	 * Checks if a given file exists.
	 *
	 * @param  fileName the file to check for
	 * @return true if the file exists, false otherwise
	 */
	static bool exists(const UString &fileName);

	/**
	 * Try to open the file with the given fileName.
	 * @note Must not be called if this file already is open (i.e. if isOpen returns true).
	 *
	 * @param  fileName the name of the file to open
	 * @return true if file was opened successfully, false otherwise
	 */
	bool open(const UString &fileName);

	/**
	 * Close the file, if open.
	 */
	void close();

	/**
	 * Checks if the object opened a file successfully.
	 *
	 * @return true if any file is opened, false otherwise.
	 */
	bool isOpen() const;

	bool err() const; // implement abstract Stream method
	void clearErr();  // implement abstract Stream method
	bool eos() const; // implement abstract SeekableReadStream method

	int32 pos() const;  // implement abstract SeekableReadStream method
	int32 size() const; // implement abstract SeekableReadStream method
	bool seek(int32 offs, int whence = SEEK_SET); // implement abstract SeekableReadStream method
	uint32 read(void *dataPtr, uint32 dataSize);  // implement abstract SeekableReadStream method

protected:
	std::FILE *_handle; ///< The actual file handle.
	int32 _size;        ///< The file's size.
};

/** For quickly dumping data into a file.
 *
 *  @note Use only for testing purposes!
 */
class DumpFile : public WriteStream, public NonCopyable {
public:
	DumpFile();
	~DumpFile();

	/**
	 * Try to open the file with the given fileName.
	 * @note Must not be called if this file already is open (i.e. if isOpen returns true).
	 *
	 * @param  fileName the name of the file to open
	 * @return true if file was opened successfully, false otherwise
	 */
	bool open(const UString &fileName);

	/**
	 * Close the file, if open.
	 */
	void close();

	/**
	 * Checks if the object opened a file successfully.
	 *
	 * @return true if any file is opened, false otherwise.
	 */
	bool isOpen() const;

	bool err() const; // implement abstract Stream method
	void clearErr();  // implement abstract Stream method

	bool flush(); // implement abstract WriteStream method

	uint32 write(const void *dataPtr, uint32 dataSize); // implement abstract WriteStream method

protected:
	std::FILE *_handle; ///< The actual file handle.
	int32 _size;        ///< The file's size.
};

} // End of namespace Common

#endif // COMMON_FILE_H
