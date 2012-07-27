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

/** @file unkeybif.cpp
 *  Tool to extract KEY/BIF archives.
 */

#include <cstring>
#include <cstdio>
#include <list>
#include <vector>

#include "common/util.h"
#include "common/ustring.h"
#include "common/error.h"
#include "common/file.h"
#include "common/filepath.h"

#include "aurora/util.h"
#include "aurora/keyfile.h"
#include "aurora/biffile.h"

#include "util.h"

enum Command {
	kCommandNone    = -1,
	kCommandList    =  0,
	kCommandExtract     ,
	kCommandMAX
};

const char *kCommandChar[kCommandMAX] = { "l", "e" };

void printUsage(FILE *stream, const char *name);
bool parseCommandLine(int argc, char **argv, int &returnValue, Command &command, std::list<Common::UString> &files);

uint32 getFileID(const Common::UString &fileName);
void identifyFiles(const std::list<Common::UString> &files, std::vector<Common::UString> &keyFiles,
                   std::vector<Common::UString> &bifFiles);

void openKEYs(const std::vector<Common::UString> &keyFiles, std::vector<Aurora::KEYFile> &keys);
void openBIFs(const std::vector<Common::UString> &bifFiles, std::vector<Aurora::BIFFile> &bifs);

void mergeKEYBIF(std::vector<Aurora::KEYFile> &keys, std::vector<Aurora::BIFFile> &bifs,
                 const std::vector<Common::UString> &keyFiles, const std::vector<Common::UString> &bifFiles);

void listFiles(const Aurora::KEYFile &key);
void listFiles(const std::vector<Aurora::KEYFile> &keys, const std::vector<Common::UString> &keyFiles);
void extractFiles(const Aurora::BIFFile &bif);
void extractFiles(const std::vector<Aurora::BIFFile> &bifs, const std::vector<Common::UString> &bifFiles);

int main(int argc, char **argv) {
	int returnValue;
	Command command;
	std::list<Common::UString> files;
	if (!parseCommandLine(argc, argv, returnValue, command, files))
		return returnValue;

	try {
		std::vector<Common::UString> keyFiles, bifFiles;
		identifyFiles(files, keyFiles, bifFiles);

		std::vector<Aurora::KEYFile> keys;
		openKEYs(keyFiles, keys);

		std::vector<Aurora::BIFFile> bifs;
		openBIFs(bifFiles, bifs);

		mergeKEYBIF(keys, bifs, keyFiles, bifFiles);

		if      (command == kCommandList)
			listFiles(keys, keyFiles);
		else if (command == kCommandExtract)
			extractFiles(bifs, bifFiles);

	} catch (Common::Exception &e) {
		Common::printException(e);
		return -1;
	}

	return 0;
}

bool parseCommandLine(int argc, char **argv, int &returnValue, Command &command, std::list<Common::UString> &files) {
	files.clear();

	// No command, just display the help
	if (argc == 1) {
		printUsage(stdout, argv[0]);
		returnValue = 0;

		return false;
	}

	// Wrong number of arguments, display the help
	if (argc < 3) {
		printUsage(stderr, argv[0]);
		returnValue = -1;

		return false;
	}

	// Find out what we should do
	command = kCommandNone;
	for (int i = 0; i < kCommandMAX; i++)
		if (!strcmp(argv[1], kCommandChar[i]))
			command = (Command) i;

	// Unknown command
	if (command == kCommandNone) {
		printUsage(stderr, argv[0]);
		returnValue = -1;

		return false;
	}

// This is a file to use
	for (int i = 2; i < argc; i++)
		files.push_back(argv[i]);

	return true;
}

void printUsage(FILE *stream, const char *name) {
	std::fprintf(stream, "BioWare KEY/BIF archive extractor\n\n");
	std::fprintf(stream, "Usage: %s <command> <file> [...]\n\n", name);
	std::fprintf(stream, "Commands:\n");
	std::fprintf(stream, "  l          List files indexed in KEY archive(s)\n");
	std::fprintf(stream, "  e          Extract BIF archive(s). Needs KEY file(s) indexing these BIF.\n\n");
	std::fprintf(stream, "Examples:\n");
	std::fprintf(stream, "%s l foo.key\n", name);
	std::fprintf(stream, "%s l foo.key bar.key\n", name);
	std::fprintf(stream, "%s e foo.bif bar.key\n", name);
	std::fprintf(stream, "%s e foo.bif quux.bif bar.key\n", name);
	std::fprintf(stream, "%s e foo.bif quux.bif bar.key foobar.key\n", name);
}

uint32 getFileID(const Common::UString &fileName) {
	Common::File file(fileName);

	return file.readUint32BE();
}

void identifyFiles(const std::list<Common::UString> &files, std::vector<Common::UString> &keyFiles,
                   std::vector<Common::UString> &bifFiles) {

	keyFiles.reserve(files.size());
	bifFiles.reserve(files.size());

	for (std::list<Common::UString>::const_iterator f = files.begin(); f != files.end(); ++f) {
		uint32 id = getFileID(*f);

		if      (id == MKID_BE('KEY '))
			keyFiles.push_back(*f);
		else if (id == MKID_BE('BIFF'))
			bifFiles.push_back(*f);
		else
			throw Common::Exception("File \"%s\" is neither a KEY nor a BIF", f->c_str());
	}
}

void openKEYs(const std::vector<Common::UString> &keyFiles, std::vector<Aurora::KEYFile> &keys) {
	keys.reserve(keyFiles.size());

	for (std::vector<Common::UString>::const_iterator f = keyFiles.begin(); f != keyFiles.end(); ++f)
		keys.push_back(Aurora::KEYFile(*f));
}

void openBIFs(const std::vector<Common::UString> &bifFiles, std::vector<Aurora::BIFFile> &bifs) {
	bifs.reserve(bifFiles.size());

	for (std::vector<Common::UString>::const_iterator f = bifFiles.begin(); f != bifFiles.end(); ++f)
		bifs.push_back(Aurora::BIFFile(*f));
}

void mergeKEYBIF(std::vector<Aurora::KEYFile> &keys, std::vector<Aurora::BIFFile> &bifs,
                 const std::vector<Common::UString> &keyFiles, const std::vector<Common::UString> &bifFiles) {

	// Go over all KEYs
	for (std::vector<Aurora::KEYFile>::iterator k = keys.begin(); k != keys.end(); ++k) {

		// Go over all BIFs handled by the KEY
		const Aurora::KEYFile::BIFList &keyBifs = k->getBIFs();
		for (uint kb = 0; kb < keyBifs.size(); kb++) {

			// Go over all BIFs
			for (uint b = 0; b < bifFiles.size(); b++) {

				// If they match, merge
				if (Common::FilePath::getFile(keyBifs[kb]).equalsIgnoreCase(Common::FilePath::getFile(bifFiles[b])))
					bifs[b].mergeKEY(*k, kb);

			}

		}

	}

}

void listFiles(const Aurora::KEYFile &key) {
	const Aurora::KEYFile::ResourceList &resources = key.getResources();

	std::printf("              Filename               | BIF\n");
	std::printf("=====================================|=====");

	const Aurora::KEYFile::BIFList &bifs = key.getBIFs();

	uint maxBIFLength = 0;
	for (Aurora::KEYFile::BIFList::const_iterator b = bifs.begin(); b != bifs.end(); ++b)
		maxBIFLength = MAX<uint>(maxBIFLength, b->size());

	for (uint i = 4; i < maxBIFLength; i++)
		std::printf("=");

	std::printf("\n");

	for (Aurora::KEYFile::ResourceList::const_iterator r = resources.begin(); r != resources.end(); ++r)
		std::printf("%32s%s | %s\n", r->name.c_str(), Aurora::setFileType("", r->type).c_str(),
		                             bifs[r->bifIndex].c_str());
}

void listFiles(const std::vector<Aurora::KEYFile> &keys, const std::vector<Common::UString> &keyFiles) {
	for (uint i = 0; i < keys.size(); i++) {
		std::printf("%s: %u files\n\n", keyFiles[i].c_str(), (uint)keys[i].getResources().size());
		listFiles(keys[i]);

		if (i < (keys.size() - 1))
			std::printf("\n");
	}
}

void extractFiles(const Aurora::BIFFile &bif) {
	const Aurora::Archive::ResourceList &resources = bif.getResources();

	uint i = 1;
	for (Aurora::Archive::ResourceList::const_iterator r = resources.begin(); r != resources.end(); ++r, ++i) {
		const Common::UString fileName = Aurora::setFileType(r->name, r->type);

		std::printf("Extracting %u/%u: %s ... ", i, (uint) resources.size(), fileName.c_str());

		Common::SeekableReadStream *stream = 0;
		try {
			stream = bif.getResource(r->index);

			dumpStream(*stream, fileName);

			std::printf("Done\n");
		} catch (Common::Exception &e) {
			Common::printException(e, "");
		}

		delete stream;
	}

}

void extractFiles(const std::vector<Aurora::BIFFile> &bifs, const std::vector<Common::UString> &bifFiles) {
	for (uint i = 0; i < bifs.size(); i++) {
		std::printf("%s: %u indexed files (of %u)\n\n", bifFiles[i].c_str(), (uint)bifs[i].getResources().size(),
                bifs[i].getInternalResourceCount());

		extractFiles(bifs[i]);

		if (i < (bifs.size() - 1))
			std::printf("\n");
	}
}
